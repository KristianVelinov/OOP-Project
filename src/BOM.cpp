#include "BOM.h"

/**
 * BOM Implementation
 * * Simple container for a collection of BOMEntry objects.
 * Used by the BOMImporter to store results and by the Project
 * class to manage the bill of materials.
 */

BOM::BOM(const std::string& projectName)
    : projectName_(projectName) 
{
    // Constructor implementation
}

// Most methods are inline in the header, 
// but the constructor must be defined here.