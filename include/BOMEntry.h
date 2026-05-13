
#pragma once
#include <string>
#include <vector>

/* A single line in a Bill of Materials.
   One BOMEntry may aggregate multiple references (CIN1,COUT1). */
struct BOMEntry {
    std::string partNumber;   // "Value" field from KiCad → model number
    int         quantity;
    std::string description;
    std::string reference;    // "R1", "CIN1,COUT1", …
    std::string footprint;    // raw KiCad footprint string
    std::string datasheet;    // URL or "~"
    std::string packageType;  // parsed
    int         pinCount;     // parsed

    BOMEntry() : quantity(1), pinCount(0) {}
    BOMEntry(const std::string& part, int qty,
             const std::string& ref  = "",
             const std::string& fp   = "",
             const std::string& ds   = "")
        : partNumber(part), quantity(qty),
          description(""), reference(ref),
          footprint(fp), datasheet(ds),
          packageType(""), pinCount(0) {}
};
