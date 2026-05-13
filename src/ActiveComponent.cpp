
#include "ActiveComponent.h"

ActiveComponent::ActiveComponent(
    const std::string& id, const std::string& name,
    int qty, double maxVoltage, double maxCurrent,
    const std::string& package,
    int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : Component(id, name, qty, minStock, unitPrice, catPath, dsPath,
                footprint, packageType, pinCount),
      maxVoltage_(maxVoltage), maxCurrent_(maxCurrent), package_(package)
{}
