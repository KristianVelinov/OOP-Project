
#include "ArbitraryComponent.h"
#include <sstream>

ArbitraryComponent::ArbitraryComponent(
    const std::string& id, const std::string& name,
    int qty, const std::string& description,
    int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : Component(id, name, qty, minStock, unitPrice, catPath, dsPath,
                footprint, packageType, pinCount),
      description_(description)
{}

std::string ArbitraryComponent::getParam(const std::string& key) const {
    auto it = customParams_.find(key);
    return (it != customParams_.end()) ? it->second : "";
}

bool ArbitraryComponent::hasParam(const std::string& key) const {
    return customParams_.count(key) > 0;
}

void ArbitraryComponent::display() const {
    std::cout << "┌─ ARBITRARY COMPONENT ──────────────────────\n";
    displayBaseInfo();
    std::cout << "  Desc  : " << description_ << "\n";
    for (auto& [k,v] : customParams_)
        std::cout << "  " << k << " : " << v << "\n";
    std::cout << "└────────────────────────────────────────────\n";
}

std::string ArbitraryComponent::serialize() const {
    std::ostringstream oss;
    oss << "ARBITRARY|" << baseSerial()
        << "|" << description_;
    for (auto& [k,v] : customParams_)
        oss << "|" << k << "=" << v;
    return oss.str();
}
