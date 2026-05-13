
#pragma once
#include "BOMEntry.h"
#include "Component.h"
#include <vector>
#include <string>

struct MatchedPart {
    BOMEntry   bomEntry;
    Component* component;
    int        deficit;
};

struct CrossReferenceResult {
    std::string              projectName;
    std::vector<MatchedPart> matched;
    std::vector<MatchedPart> shortfall;
    std::vector<BOMEntry>    missing;

    double totalCost()    const;
    int    totalDeficit() const;
};
