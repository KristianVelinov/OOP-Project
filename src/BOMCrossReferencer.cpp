#include "BOMCrossReferencer.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// Value parsing
// ─────────────────────────────────────────────────────────────────────────────

struct ParsedValue {
    double      siValue = 0.0;  // normalised to SI base unit (F, H, Ohm)
    std::string unit;           // "F", "H", "Ohm"  — empty = ambiguous / not a passive
    bool        valid   = false;
};

static double siMult(char c) {
    switch (c) {
        case 'p': return 1e-12;
        case 'n': return 1e-9;
        case 'u': return 1e-6;
        case 'm': return 1e-3;
        case 'k': return 1e3;
        case 'M': return 1e6;
        case 'G': return 1e9;
        default:  return 0.0;  // not a prefix
    }
}

// Strip whitespace; normalise µ (UTF-8 U+00B5 and U+03BC) → ASCII 'u'.
static std::string cleanRaw(const std::string& raw) {
    std::string s;
    for (size_t i = 0; i < raw.size(); ) {
        unsigned char c = raw[i];
        if (c == 0xC2 && i+1 < raw.size() && (unsigned char)raw[i+1] == 0xB5)
            { s += 'u'; i += 2; }
        else if (c == 0xCE && i+1 < raw.size() && (unsigned char)raw[i+1] == 0xBC)
            { s += 'u'; i += 2; }
        else if (c == 0xB5)
            { s += 'u'; i++; }
        else if (c != ' ')
            { s += (char)c; i++; }
        else
            { i++; }
    }
    return s;
}

// Parse a KiCad BOM value string into a normalised SI value.
//
// Handled formats:
//   Standard       100nF   4.7uH   10k   47R   22pF   1M
//   European       4k7     1M5     4R7   3n3
//   Bare integer   470     100     1000   → treated as Ohms (KiCad convention)
//   Bare SI pfx    100n    4.7u           → unit left empty; caller resolves from type
//
// IC/part-number strings (no leading digit) → returns invalid (valid=false).
//
static ParsedValue parseComponentValue(const std::string& raw) {
    ParsedValue result;
    std::string s = cleanRaw(raw);
    if (s.empty()) return result;

    size_t i = 0;

    // 1) Leading digits / decimal point
    std::string numStr;
    while (i < s.size() && (std::isdigit((unsigned char)s[i]) || s[i] == '.'))
        numStr += s[i++];

    if (numStr.empty()) return result;  // starts with letter → IC / part-number

    double baseNum;
    try { baseNum = std::stod(numStr); } catch (...) { return result; }

    // 2) Nothing after the digits → bare integer (e.g. "470", "100").
    //    KiCad convention: a resistor value with no suffix is in Ohms.
    if (i >= s.size()) {
        result.valid   = true;
        result.siValue = baseNum;
        result.unit    = "Ohm";
        return result;
    }

    char c = s[i++];
    double mult = siMult(c);
    bool hadPrefix = (mult != 0.0);

    // 3) No SI prefix — must be a direct unit or European decimal separator
    if (!hadPrefix) {
        if (c == 'F') {
            result = {baseNum, "F", true};
        } else if (c == 'H') {
            result = {baseNum, "H", true};
        } else if (c == 'R' || c == 'E') {
            // European ohm separator: 4R7 = 4.7 Ω, 47R = 47 Ω
            std::string frac;
            while (i < s.size() && std::isdigit((unsigned char)s[i])) frac += s[i++];
            if (!frac.empty())
                baseNum += std::stod(frac) / std::pow(10.0, (double)frac.size());
            result = {baseNum, "Ohm", true};
        }
        // Anything else (V, A, W, IC suffix letter) → not a passive value
        return result;
    }

    // 4) Had an SI prefix.
    //    European decimal continuation: 4k7 → 4.7, 1M5 → 1.5, 3n3 → 3.3
    std::string frac;
    while (i < s.size() && std::isdigit((unsigned char)s[i])) frac += s[i++];
    if (!frac.empty())
        baseNum += std::stod(frac) / std::pow(10.0, (double)frac.size());

    // 5) Optional explicit unit after the prefix: 100nF, 4.7uH, 10kΩ (→ 'R'/'E'), …
    std::string unit;
    if (i < s.size()) {
        char uc = s[i];
        if      (uc == 'F')              { unit = "F";   i++; }
        else if (uc == 'H')              { unit = "H";   i++; }
        else if (uc == 'R' || uc == 'E') { unit = "Ohm"; i++; }
    }

    // 6) k / M / G alone → Ohm by convention.  p / n / u / m alone → ambiguous
    //    (could be F or H); leave unit empty so the caller can resolve from the
    //    inferred component type (footprint).
    if (unit.empty() && (c == 'k' || c == 'M' || c == 'G'))
        unit = "Ohm";

    result.valid   = true;
    result.siValue = baseNum * mult;
    result.unit    = unit;
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// Component-type inference from KiCad footprint + value string
// ─────────────────────────────────────────────────────────────────────────────

static std::string inferBOMType(const BOMEntry& e) {
    auto lc = [](const std::string& s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(), ::tolower);
        return r;
    };
    std::string fp = lc(e.footprint);

    if (fp.find("capacitor")     != std::string::npos) return "CAPACITOR";
    if (fp.find("resistor")      != std::string::npos) return "RESISTOR";
    if (fp.find("inductor")      != std::string::npos) return "INDUCTOR";
    if (fp.find(":l_")           != std::string::npos) return "INDUCTOR";
    if (fp.find("led")           != std::string::npos) return "DIODE";
    if (fp.find("diode")         != std::string::npos) return "DIODE";
    if (fp.find(":d_")           != std::string::npos) return "DIODE";
    if (fp.find("transistor")    != std::string::npos) return "TRANSISTOR";
    if (fp.find("mosfet")        != std::string::npos) return "TRANSISTOR";
    if (fp.find("crystal")       != std::string::npos) return "CRYSTAL";
    if (fp.find("connector")     != std::string::npos) return "CONNECTOR";
    if (fp.find("pinheader")     != std::string::npos) return "CONNECTOR";
    if (fp.find("terminalblock") != std::string::npos) return "CONNECTOR";

    // Fall back to the parsed value's unit
    ParsedValue pv = parseComponentValue(e.partNumber);
    if (pv.valid) {
        if (pv.unit == "F")   return "CAPACITOR";
        if (pv.unit == "H")   return "INDUCTOR";
        if (pv.unit == "Ohm") return "RESISTOR";
    }

    return "";
}

static bool typeCompatible(const std::string& bomType, const Component* c) {
    if (bomType.empty()) return true;
    const std::string& t = c->getTypeName();
    if (bomType == "CAPACITOR"  && t == "CAPACITOR")               return true;
    if (bomType == "RESISTOR"   && t == "RESISTOR")                return true;
    if (bomType == "INDUCTOR"   && t == "INDUCTOR")                return true;
    if (bomType == "DIODE"      && t == "DIODE")                   return true;
    if (bomType == "TRANSISTOR" && (t == "MOSFET" || t == "BJT")) return true;
    if (bomType == "IC"         && t == "IC")                      return true;
    if (bomType == "CONNECTOR"  && t == "CONNECTOR")               return true;
    return false;
}

// Compare parsed BOM SI value against a component's stored parametric value.
// Tolerance ±2 % covers E-series rounding without false positives.
static bool parametricMatch(const ParsedValue& pv,
                             const std::string& resolvedUnit,
                             const Component*   c)
{
    if (!pv.valid || resolvedUnit.empty()) return false;

    std::string paramKey;
    if      (resolvedUnit == "F")   paramKey = "capacitance";
    else if (resolvedUnit == "H")   paramKey = "inductance";
    else if (resolvedUnit == "Ohm") paramKey = "resistance";
    else return false;

    auto params = c->getTypeParams();
    auto it = params.find(paramKey);
    if (it == params.end()) return false;

    double stored;
    try { stored = std::stod(it->second); } catch (...) { return false; }
    if (stored <= 0.0) return false;

    double ratio = pv.siValue / stored;
    return (ratio > 0.995 && ratio < 1.005); // allows tolerance of up to ±0.5 % (e.g. 4.7uF matching 4.68uF) without false positives across E-series values
}

// ─────────────────────────────────────────────────────────────────────────────
// BOMCrossReferencer
// ─────────────────────────────────────────────────────────────────────────────

BOMCrossReferencer::BOMCrossReferencer(Inventory& inv) : inventory_(inv) {}

// Resolve a BOM entry to an inventory component.
//
// ┌─ Stage A  (passive components — any entry with a known SI unit) ─────────┐
// │  Parametric SI-value comparison only.                                     │
// │                                                                           │
// │  WHY no string fallback:                                                  │
// │  SmartMatcher strips unit suffixes so it cannot distinguish values —      │
// │  "4.7uF" and "4.7nF" both become "4.7" and would match each other.       │
// │  Similarly "100k" and "1k" both reduce to short strings that satisfy     │
// │  the prefix rules.  The parametric path compares real SI numbers and      │
// │  is immune to all of these problems.                                      │
// │                                                                           │
// │  If no parametric match is found → entry is MISSING (correct).           │
// └───────────────────────────────────────────────────────────────────────────┘
//
// ┌─ Stage B  (ICs, transistors, connectors, unknown type) ────────────────── ┐
// │  SmartMatcher string match (original behaviour).                          │
// │  Passives never reach this stage.                                         │
// └───────────────────────────────────────────────────────────────────────────┘
//
Component* BOMCrossReferencer::resolveEntry(const BOMEntry& e) const {
    auto all = inventory_.getAll();

    std::string bomType = inferBOMType(e);
    ParsedValue pv      = parseComponentValue(e.partNumber);

    // Promote bare-SI-prefix unit using footprint type context.
    // e.g. "100n" with Capacitor_ footprint → 100 nF.
    std::string resolvedUnit = pv.unit;
    if (resolvedUnit.empty() && pv.valid) {
        if      (bomType == "CAPACITOR") resolvedUnit = "F";
        else if (bomType == "INDUCTOR")  resolvedUnit = "H";
        else if (bomType == "RESISTOR")  resolvedUnit = "Ohm";
    }

    // ── Stage A: passive / value-based ───────────────────────────────────────
    if (pv.valid && !resolvedUnit.empty()) {
        for (auto* c : all) {
            if (!bomType.empty() && !typeCompatible(bomType, c)) continue;
            if (parametricMatch(pv, resolvedUnit, c)) return c;
        }
        // No match within the correct type bucket.
        // Never fall through to string matching for passives.
        return nullptr;
    }

    // ── Stage B: IC / discrete / connector ───────────────────────────────────
    if (!bomType.empty()) {
        for (auto* c : all) {
            if (!typeCompatible(bomType, c)) continue;
            if (matcher_.match(e.partNumber, c->getName())) return c;
        }
    }
    for (auto* c : all)
        if (matcher_.match(e.partNumber, c->getName())) return c;

    return nullptr;
}

CrossReferenceResult BOMCrossReferencer::compare(const BOM& bom) const {
    CrossReferenceResult result;
    result.projectName = bom.getProjectName();

    for (const auto& entry : bom.getEntries()) {
        Component* comp = resolveEntry(entry);
        if (!comp) {
            result.missing.push_back(entry);
        } else {
            int deficit = entry.quantity - comp->getQuantity();
            MatchedPart mp{ entry, comp, (deficit > 0 ? deficit : 0) };
            if (deficit <= 0) result.matched.push_back(mp);
            else              result.shortfall.push_back(mp);
        }
    }
    return result;
}

std::vector<BOMEntry> BOMCrossReferencer::getMissing(const BOM& bom) const {
    return compare(bom).missing;
}

std::vector<MatchedPart> BOMCrossReferencer::getShortfall(const BOM& bom) const {
    return compare(bom).shortfall;
}