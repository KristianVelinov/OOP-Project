
#include "LowStockAlert.h"
#include <fstream>
#include <sstream>
#include <algorithm>

std::vector<Component*> LowStockAlert::generate(const Inventory& inv) {
    auto comps = inv.getLowStock();
    std::sort(comps.begin(), comps.end(), [](Component* a, Component* b){
        return (a->getMinStock() - a->getQuantity()) >
               (b->getMinStock() - b->getQuantity());
    });
    return comps;
}

std::string LowStockAlert::generateReport(const Inventory& inv) {
    auto comps = generate(inv);
    std::ostringstream oss;
    oss << "═══════════════════════════════════════════════════════\n"
        << "  LOW STOCK ALERT REPORT  (" << comps.size() << " components)\n"
        << "═══════════════════════════════════════════════════════\n"
        << std::left
        << std::setw(10) << "ID"
        << std::setw(28) << "Name"
        << std::setw(8)  << "Qty"
        << std::setw(8)  << "Min"
        << std::setw(8)  << "Need"
        << "Price\n"
        << std::string(70, '-') << "\n";
    for (auto* c : comps) {
        int need = c->getMinStock() - c->getQuantity();
        oss << std::setw(10) << c->getId()
            << std::setw(28) << c->getName().substr(0,27)
            << std::setw(8)  << c->getQuantity()
            << std::setw(8)  << c->getMinStock()
            << std::setw(8)  << need
            << "$" << std::fixed << std::setprecision(4)
                   << c->getUnitPrice() << "\n";
    }
    oss << "═══════════════════════════════════════════════════════\n";
    return oss.str();
}

bool LowStockAlert::exportReport(const Inventory& inv,
                                  const std::string& filepath) {
    auto comps = generate(inv);
    std::ofstream f(filepath);
    if (!f.is_open()) return false;
    f << "ID,Name,Qty,MinStock,Deficit,UnitPrice,Category\n";
    for (auto* c : comps)
        f << c->getId() << ","
          << c->getName() << ","
          << c->getQuantity() << ","
          << c->getMinStock() << ","
          << (c->getMinStock()-c->getQuantity()) << ","
          << c->getUnitPrice() << ","
          << c->getCategoryPath() << "\n";
    return true;
}
