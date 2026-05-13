
#pragma once
#include "BOMEntry.h"
#include <vector>
#include <string>

class BOM {
    std::string          projectName_;
    std::vector<BOMEntry> entries_;

public:
    explicit BOM(const std::string& projectName = "unnamed");

    void             addEntry(const BOMEntry& e) { entries_.push_back(e); }
    const std::vector<BOMEntry>& getEntries()    const { return entries_; }
    const std::string& getProjectName()          const { return projectName_; }
    int              size()                      const { return (int)entries_.size(); }

    // total unique parts (not summing quantities)
    int uniqueParts() const { return size(); }
};
