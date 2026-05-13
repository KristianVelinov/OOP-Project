
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
    // Work backwards from end:
    //  – strip everything after '#'  (e.g. #PBF, #TRPBF)
    //  – strip everything after a letter suffix following digits
    //    (e.g. LT1764AET → LT1764,  IRLZ44N → IRLZ44)
    std::string r = s;

    // 1) Strip #… suffix
    auto hash = r.find('#');
    if (hash != std::string::npos) r = r.substr(0, hash);

    // 2) Strip trailing package / temp / grade letter groups
    //    Pattern: strip suffix that starts with a letter following a digit
    //    and contains only A-Z digits until end.
    //    We try incrementally removing trailing alpha runs.
    for (int pass = 0; pass < 3; pass++) {
        if (r.empty()) break;
        // find last digit position
        int lastDigit = -1;
        for (int i = (int)r.size()-1; i >= 0; i--)
            if (std::isdigit(static_cast<unsigned char>(r[i]))) { lastDigit=i; break; }
        if (lastDigit < 0) break;
        // if there are only letters after lastDigit, strip them
        bool allAlpha = true;
        for (int i = lastDigit+1; i < (int)r.size(); i++)
            if (!std::isalpha(static_cast<unsigned char>(r[i]))) { allAlpha=false; break; }
        if (allAlpha && lastDigit+1 < (int)r.size())
            r = r.substr(0, lastDigit+1);
        else break;
    }
    return r;
}

std::string SmartMatcher::extractBaseModel(const std::string& partNumber) const {
    return stripPackageSuffix(normalise(partNumber));
}

bool SmartMatcher::match(const std::string& bomPart,
                          const std::string& inventoryName) const {
    std::string base  = extractBaseModel(bomPart);
    std::string invN  = normalise(inventoryName);
    std::string invB  = stripPackageSuffix(invN);

    // 1) Exact match (after normalise)
    if (base == invN || base == invB) return true;

    // 2) BOM base is a prefix of inventory name (LT1764 → LT1764AET#PBF)
    if (!base.empty() && invN.find(base) == 0) return true;

    // 3) Inventory base is a prefix of BOM (rare but handle)
    if (!invB.empty() && base.find(invB) == 0) return true;

    return false;
}
