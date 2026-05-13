
#include "FootprintParser.h"
#include <regex>

std::string FootprintParser::toUpper(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return r;
}

std::string FootprintParser::extractPackageType(const std::string& footprint) {
    if (footprint.empty() || footprint == "~") return "";
    std::string fp = toUpper(footprint);

    // SMD passives – check before broader patterns
    auto hasSMD = [&](const std::string& sz) {
        return fp.find("_" + sz + "_") != std::string::npos
            || fp.find(":" + sz + "_") != std::string::npos
            || fp.find("_" + sz + "M") != std::string::npos;
    };
    for (const char* sz : {"0201","0402","0603","0805","1206","1210",
                            "1812","2010","2512","2816","3216","3225"})
        if (hasSMD(sz)) return sz;

    // IC packages – ordered from most-specific to least
    struct { const char* key; const char* pkg; } checks[] = {
        {"TQFP",    "TQFP"},  {"LQFP",   "LQFP"},  {"QFP",    "QFP"},
        {"QFN",     "QFN"},   {"DFN",     "DFN"},   {"WSON",   "WSON"},
        {"LGA",     "LGA"},   {"BGA",     "BGA"},   {"MSOP",   "MSOP"},
        {"TSSOP",   "TSSOP"}, {"SSOP",   "SSOP"},   {"HSOP",   "HSOP"},
        {"SOIC",    "SOIC"},  {"SOP",    "SOP"},
        // Transistor / discrete
        {"TO-247",  "TO-247"},{"TO-263", "TO-263"},  {"TO-252", "TO-252"},
        {"TO-220",  "TO-220"},{"TO-251", "TO-251"},  {"TO-92",  "TO-92"},
        {"SOT-363", "SOT-363"},{"SOT-223","SOT-223"},
        {"SOT-89",  "SOT-89"},{"SOT-23", "SOT-23"},  {"SOT",    "SOT"},
        // Connectors / through-hole
        {"PINHEADER","PinHeader"},{"TERMINALBLOCK","TerminalBlock"},
        {"DIP",     "DIP"},
        // Generic THT markers
        {"RADIAL",  "THT-Radial"},{"AXIAL","THT-Axial"},
        {"DIN",     "THT-DIN"},   {"THT", "THT"},
        {"TOROID",  "THT-Toroid"},
    };
    for (auto& c : checks)
        if (fp.find(c.key) != std::string::npos) return c.pkg;

    return "SMD"; // fallback
}

int FootprintParser::extractPinCount(const std::string& footprint) {
    if (footprint.empty() || footprint == "~") return 0;

    // Try NxM pattern (connectors): 1x08 → 8, 2x10 → 20
    {
        std::regex rx(R"((\d+)x(\d+))", std::regex::icase);
        std::smatch m;
        if (std::regex_search(footprint, m, rx)) {
            int rows = std::stoi(m[1].str());
            int cols = std::stoi(m[2].str());
            return rows * cols;
        }
    }
    // Package with pin count: TO-220-5, SOT-23-5, SOIC-8, QFN-32, DIP-16
    {
        std::regex rx(R"((?:TO|SOT|SOIC|SOP|DIP|QFN|QFP|TQFP|LQFP|MSOP|TSSOP|SSOP|DFN|BGA|LGA)-\d+-?(\d+))",
                      std::regex::icase);
        std::smatch m;
        if (std::regex_search(footprint, m, rx)) {
            // last capture is pin count suffix if present
            if (m[1].matched) return std::stoi(m[1].str());
        }
    }
    // Simple -N at end or _PN_
    {
        std::regex rx(R"([-_](\d+)(?:_|$))");
        std::smatch m;
        std::string s = footprint;
        // search from end of library: portion after ':'
        auto colon = s.find(':');
        if (colon != std::string::npos) s = s.substr(colon+1);
        if (std::regex_search(s, m, rx))
            return std::stoi(m[1].str());
    }
    return 0;
}
