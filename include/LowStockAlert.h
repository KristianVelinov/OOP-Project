
#pragma once
#include "Inventory.h"
#include <vector>
#include <string>
#include <algorithm>

class LowStockAlert {
public:
    static std::vector<Component*> generate(const Inventory& inv);
    static std::string             generateReport(const Inventory& inv);
    static bool                    exportReport(const Inventory& inv,
                                                const std::string& filepath);
};
