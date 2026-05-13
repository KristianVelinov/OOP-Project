
#pragma once
#include "SearchCriteria.h"
#include "Inventory.h"
#include <vector>

class ParametricSearch {
    Inventory& inventory_;

public:
    explicit ParametricSearch(Inventory& inv) : inventory_(inv) {}

    std::vector<Component*> execute(const SearchCriteria& criteria) const;
};
