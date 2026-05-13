
#pragma once
#include "Component.h"
#include <string>
#include <map>
#include <utility>
#include <functional>

/* Builder-pattern search criteria.
   Numeric params are matched via getTypeParams() on each component. */
class SearchCriteria {
public:
    struct NumRange { double lo, hi; };

    SearchCriteria& setCategory(const std::string& path);
    SearchCriteria& setNameFilter(const std::string& pattern);
    SearchCriteria& setTypeFilter(const std::string& typeName);
    SearchCriteria& setPackageType(const std::string& pkg);
    SearchCriteria& setPinCount(int exact);
    SearchCriteria& setPinCountRange(int lo, int hi);
    SearchCriteria& setMaxUnitPrice(double price);
    SearchCriteria& setMinQuantity(int qty);
    SearchCriteria& addParamRange(const std::string& param,
                                  double lo, double hi);
    SearchCriteria& onlyLowStock(bool v = true);

    bool matches(const Component* c) const;

private:
    std::string categoryFilter_;
    std::string nameFilter_;
    std::string typeFilter_;
    std::string packageTypeFilter_;
    int  pinCountExact_ = -1;
    int  pinCountLo_    = -1;
    int  pinCountHi_    = -1;
    double maxUnitPrice_ = -1.0;
    int    minQuantity_  = -1;
    bool   lowStockOnly_ = false;
    std::map<std::string, NumRange> paramRanges_;

    static std::string toLower(const std::string& s);
    static bool containsCI(const std::string& haystack,
                            const std::string& needle);
};
