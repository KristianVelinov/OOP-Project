
#include "BOMImporter.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

namespace fs = std::filesystem;

std::string BOMImporter::stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size()-2);
    return s;
}

bool BOMImporter::isHeaderLine(const std::string& line) {
    return line.find("Reference") != std::string::npos
        && line.find("Qty") != std::string::npos;
}

std::vector<std::string> BOMImporter::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    bool inQuote = false;
    std::string cur;
    for (char c : line) {
        if (c == '"') { inQuote = !inQuote; }
        else if (c == ',' && !inQuote) {
            fields.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    fields.push_back(cur);
    return fields;
}

BOM BOMImporter::importKiCadCSV(const std::string& filepath,
                                  const std::string& projectName) const {
    if (!fs::exists(filepath))
        throw std::runtime_error("BOM file not found: " + filepath);

    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open BOM file: " + filepath);

    // Derive project name from filename if not given
    std::string projName = projectName;
    if (projName.empty()) {
        fs::path p(filepath);
        projName = p.stem().string();
    }

    BOM bom(projName);
    std::string line;
    bool headerFound = false;

    // Column indices (default KiCad 7 layout)
    int colRef=0, colQty=1, colValue=2, colDNP=3,
        colExclBOM=4, colExclBoard=5, colFP=6, colDS=7;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (isHeaderLine(line)) {
            headerFound = true;
            auto hdr = parseCSVLine(line);
            for (int i = 0; i < (int)hdr.size(); i++) {
                std::string h = stripQuotes(hdr[i]);
                if      (h == "Reference")       colRef  = i;
                else if (h == "Qty")             colQty  = i;
                else if (h == "Value")           colValue= i;
                else if (h == "DNP")             colDNP  = i;
                else if (h == "Exclude from BOM")colExclBOM=i;
                else if (h == "Footprint")       colFP   = i;
                else if (h == "Datasheet")       colDS   = i;
            }
            continue;
        }
        if (!headerFound) continue;

        auto fields = parseCSVLine(line);
        if ((int)fields.size() <= colValue) continue;

        auto get = [&](int idx) -> std::string {
            if (idx < (int)fields.size()) return stripQuotes(fields[idx]);
            return "";
        };

        // Skip DNP and excluded-from-BOM parts
        if (get(colDNP) == "x" || get(colDNP) == "1") continue;
        if (get(colExclBOM) == "x" || get(colExclBOM) == "1") continue;

        BOMEntry entry;
        entry.reference  = get(colRef);
        entry.partNumber = get(colValue);   // "Value" = model / part number
        entry.footprint  = get(colFP);
        entry.datasheet  = get(colDS);
        entry.description= "";              // KiCad default BOM has no description col

        // Parse qty – KiCad puts the integer count directly
        try { entry.quantity = std::stoi(get(colQty)); }
        catch(...) { entry.quantity = 1; }

        // Parse footprint metadata
        entry.packageType = FootprintParser::extractPackageType(entry.footprint);
        entry.pinCount    = FootprintParser::extractPinCount(entry.footprint);

        if (!entry.partNumber.empty())
            bom.addEntry(entry);
    }
    return bom;
}
