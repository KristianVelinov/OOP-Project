
#include "ParametricSearch.h"

std::vector<Component*> ParametricSearch::execute(const SearchCriteria& c) const {
    std::vector<Component*> out;
    for (auto* comp : inventory_.getAll())
        if (c.matches(comp)) out.push_back(comp);
    return out;
}
