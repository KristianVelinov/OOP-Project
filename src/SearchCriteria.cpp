
#include "SearchCriteria.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>

std::string SearchCriteria::toLower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return r;
}

bool SearchCriteria::containsCI(const std::string& h, const std::string& n) {
    return toLower(h).find(toLower(n)) != std::string::npos;
}

SearchCriteria& SearchCriteria::setCategory(const std::string& p)    { categoryFilter_=p;    return *this; }
SearchCriteria& SearchCriteria::setNameFilter(const std::string& p)  { nameFilter_=p;         return *this; }
SearchCriteria& SearchCriteria::setTypeFilter(const std::string& p)  { typeFilter_=p;         return *this; }
SearchCriteria& SearchCriteria::setPackageType(const std::string& p) { packageTypeFilter_=p;  return *this; }
SearchCriteria& SearchCriteria::setMaxUnitPrice(double v)            { maxUnitPrice_=v;       return *this; }
SearchCriteria& SearchCriteria::setMinQuantity(int v)                { minQuantity_=v;        return *this; }
SearchCriteria& SearchCriteria::onlyLowStock(bool v)                 { lowStockOnly_=v;       return *this; }

SearchCriteria& SearchCriteria::setPinCount(int exact) {
    pinCountExact_ = exact;
    pinCountLo_ = pinCountHi_ = -1;
    return *this;
}

SearchCriteria& SearchCriteria::setPinCountRange(int lo, int hi) {
    pinCountLo_ = lo; pinCountHi_ = hi; pinCountExact_ = -1;
    return *this;
}

SearchCriteria& SearchCriteria::addParamRange(const std::string& p, double lo, double hi) {
    paramRanges_[p] = {lo, hi};
    return *this;
}

bool SearchCriteria::matches(const Component* c) const {
    if (!categoryFilter_.empty() &&
        c->getCategoryPath().find(categoryFilter_) == std::string::npos)
        return false;
    if (!nameFilter_.empty() && !containsCI(c->getName(), nameFilter_)
                             && !containsCI(c->getId(),   nameFilter_))
        return false;
    if (!typeFilter_.empty() && !containsCI(c->getTypeName(), typeFilter_))
        return false;
    if (!packageTypeFilter_.empty() &&
        !containsCI(c->getPackageType(), packageTypeFilter_))
        return false;
    if (pinCountExact_ >= 0 && c->getPinCount() != pinCountExact_)
        return false;
    if (pinCountLo_ >= 0 && c->getPinCount() < pinCountLo_)  return false;
    if (pinCountHi_ >= 0 && c->getPinCount() > pinCountHi_)  return false;
    if (maxUnitPrice_ >= 0 && c->getUnitPrice() > maxUnitPrice_) return false;
    if (minQuantity_  >= 0 && c->getQuantity()  < minQuantity_)  return false;
    if (lowStockOnly_ && !c->isLowStock())  return false;

    // Numeric parameter ranges
    if (!paramRanges_.empty()) {
        auto params = c->getTypeParams();
        for (auto& [key, range] : paramRanges_) {
            auto it = params.find(key);
            if (it == params.end()) return false;
            try {
                double val = std::stod(it->second);
                if (val < range.lo || val > range.hi) return false;
            } catch (...) { return false; }
        }
    }
    return true;
}
