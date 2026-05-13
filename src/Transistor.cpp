
#include "Transistor.h"

Transistor::Transistor(
    const std::string& id, const std::string& name,
    int qty, const std::string& transistorType,
    double hfe, double maxVoltage, double maxCurrent,
    const std::string& package,
    int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : ActiveComponent(id, name, qty, maxVoltage, maxCurrent, package,
                      minStock, unitPrice, catPath, dsPath,
                      footprint, packageType, pinCount),
      transistorType_(transistorType), hfe_(hfe)
{}
