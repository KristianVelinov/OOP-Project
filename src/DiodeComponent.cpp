
#include "DiodeComponent.h"
#include <sstream>

DiodeComponent::DiodeComponent(
    const std::string& id, const std::string& name,
    int qty, double forwardVoltage, double reverseVoltage,
    const std::string& diodeType, double maxCurrent,
    const std::string& package, int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : ActiveComponent(id, name, qty, reverseVoltage, maxCurrent, package,
                      minStock, unitPrice, catPath, dsPath,
                      footprint, packageType, pinCount),
      forwardVoltage_(forwardVoltage), reverseVoltage_(reverseVoltage),
      diodeType_(diodeType)
{}

void DiodeComponent::display() const {
    std::cout << "┌─ DIODE ────────────────────────────────────\n";
    displayBaseInfo();
    std::cout << "  Type   : " << diodeType_      << "\n"
              << "  Vf     : " << forwardVoltage_  << " V\n"
              << "  Vrm    : " << reverseVoltage_  << " V\n"
              << "  If(max): " << maxCurrent_       << " A\n"
              << "  Package: " << package_          << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> DiodeComponent::getTypeParams() const {
    return {
        {"forwardVoltage", std::to_string(forwardVoltage_)},
        {"reverseVoltage", std::to_string(reverseVoltage_)},
        {"diodeType",      diodeType_},
        {"maxCurrent",     std::to_string(maxCurrent_)},
        {"package",        package_},
        {"pinCount",       std::to_string(pinCount_)},
        {"packageType",    packageType_}
    };
}

std::string DiodeComponent::serialize() const {
    std::ostringstream oss;
    oss << "DIODE|" << baseSerial()
        << "|" << std::fixed << std::setprecision(4)
        << forwardVoltage_ << "|" << reverseVoltage_
        << "|" << diodeType_ << "|" << maxCurrent_
        << "|" << package_;
    return oss.str();
}
