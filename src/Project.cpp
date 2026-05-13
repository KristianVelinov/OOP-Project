
#include "Project.h"

Project::Project(const std::string& name, const std::string& description)
    : name_(name), description_(description), bom_(name)
{}

double Project::calculateCost(const Inventory& inv) const {
    double total = 0.0;
    for (auto& tx : log_.getAll()) {
        Component* c = inv.getComponent(tx.getComponentId());
        if (!c) continue;
        if (tx.getType() == TransactionType::CHECKOUT)
            total += c->getUnitPrice() * tx.getQuantity();
        else if (tx.getType() == TransactionType::RETURN)
            total -= c->getUnitPrice() * tx.getQuantity();
    }
    return total;
}
