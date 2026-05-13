
#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

/* Prefix-based part number matcher.
   Strips packaging / temperature / RoHS suffixes before comparing.
   Example: BOM "LT1764" matches inventory "LT1764AET#PBF". */
class SmartMatcher {
public:
    SmartMatcher() = default;

    std::string extractBaseModel(const std::string& partNumber) const;
    bool        match(const std::string& bomPart,
                      const std::string& inventoryName) const;

private:
    static std::string normalise(const std::string& s);
    static std::string stripPackageSuffix(const std::string& s);
};
