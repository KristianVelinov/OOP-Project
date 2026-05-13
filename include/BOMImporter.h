
#pragma once
#include "BOM.h"
#include "FootprintParser.h"
#include <string>
#include <stdexcept>

/* Imports KiCad-generated BOM CSV files.
   Expected header (KiCad 7 default):
   "Reference","Qty","Value","DNP","Exclude from BOM","Exclude from Board","Footprint","Datasheet" */
class BOMImporter {
public:
    BOM importKiCadCSV(const std::string& filepath,
                       const std::string& projectName = "") const;

private:
    static std::string stripQuotes(const std::string& s);
    static std::vector<std::string> parseCSVLine(const std::string& line);
    static bool isHeaderLine(const std::string& line);
};
