
#pragma once
#include <string>
#include <regex>
#include <algorithm>
#include <cctype>

/* Utility for parsing KiCad footprint strings.
   Footprint format:  LibraryName:FootprintName
   Examples:
     Resistor_SMD:R_0603_1608Metric
     Package_TO_SOT_THT:TO-220-5_P3.4x3.7mm_Vertical
     Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical */
class FootprintParser {
public:
    static std::string extractPackageType(const std::string& footprint);
    static int         extractPinCount  (const std::string& footprint);

private:
    static std::string toUpper(const std::string& s);
};
