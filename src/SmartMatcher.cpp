#include "SmartMatcher.h"
#include <cctype>

std::string SmartMatcher::normalise(const std::string& s) {
    std::string r;
    for (char c : s)
        if (!std::isspace(static_cast<unsigned char>(c)))
            r += std::toupper(static_cast<unsigned char>(c));
    return r;
}

std::string SmartMatcher::stripPackageSuffix(const std::string& s) {
    std::string r = s;

    // 1) Strip '#' trailer (e.g. #PBF, #TRPBF)
    auto hash = r.find('#');
    if (hash != std::string::npos) r = r.substr(0, hash);

    // 2) Strip trailing letter groups that follow the last digit.
    //    E.g. LT1764AET → LT1764,  IRLZ44N → IRLZ44.
    //    We iterate up to 3 times to peel compound suffixes (e.g. -ADJ → keep '-ADJ'
    //    but LT1764A → LT1764, LT1764ET → LT1764).
    for (int pass = 0; pass < 3; pass++) {
        if (r.empty()) break;
        int lastDigit = -1;
        for (int i = (int)r.size() - 1; i >= 0; i--)
            if (std::isdigit(static_cast<unsigned char>(r[i]))) { lastDigit = i; break; }
        if (lastDigit < 0) break;
        bool allAlpha = true;
        for (int i = lastDigit + 1; i < (int)r.size(); i++)
            if (!std::isalpha(static_cast<unsigned char>(r[i]))) { allAlpha = false; break; }
        if (allAlpha && lastDigit + 1 < (int)r.size())
            r = r.substr(0, lastDigit + 1);
        else
            break;
    }
    return r;
}

std::string SmartMatcher::extractBaseModel(const std::string& partNumber) const {
    return stripPackageSuffix(normalise(partNumber));
}

bool SmartMatcher::match(const std::string& bomPart,
                          const std::string& inventoryName) const {
    std::string base = extractBaseModel(bomPart);
    std::string invN = normalise(inventoryName);
    std::string invB = stripPackageSuffix(invN);

    // Rule 1: exact match after normalisation
    if (base == invN || base == invB) return true;

    // Rule 2: BOM base is a prefix of the inventory name
    // (e.g. "LT1764" matches "LT1764AET#PBF" in inventory).
    // Guard: require the prefix to be at least 4 characters to avoid
    // short numeric strings ("1", "10", "100") matching unrelated parts.
    if (base.size() >= 4 && !invN.empty() && invN.find(base) == 0) return true;

    // Rule 3: Inventory base is a prefix of the BOM base
    // (rare case where the inventory entry is a truncated variant name).
    // Guard: same minimum-length requirement to prevent, e.g., "1"
    // (from "1k Resistor" stripped to "1") matching "100", "100k", etc.
    if (invB.size() >= 4 && !base.empty() && base.find(invB) == 0) return true;

    return false;
}