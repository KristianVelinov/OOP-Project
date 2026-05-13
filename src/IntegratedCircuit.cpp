
#include "IntegratedCircuit.h"
#include <sstream>

IntegratedCircuit::IntegratedCircuit(
    const std::string& id, const std::string& name,
    int qty, const std::string& functionDesc,
    double supplyVoltage, double maxVoltage, double maxCurrent,
    const std::string& package, int pinCount,
    int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType)
    : ActiveComponent(id, name, qty, maxVoltage, maxCurrent, package,
                      minStock, unitPrice, catPath, dsPath,
                      footprint, packageType, pinCount),
      functionDesc_(functionDesc), supplyVoltage_(supplyVoltage)
{}

void IntegratedCircuit::display() const {
    std::cout << "┌─ INTEGRATED CIRCUIT ───────────────────────\n";
    displayBaseInfo();
    std::cout << "  Function: " << functionDesc_  << "\n"
              << "  Vsupply : " << supplyVoltage_ << " V\n"
              << "  Vmax    : " << maxVoltage_    << " V\n"
              << "  Imax    : " << maxCurrent_    << " A\n"
              << "  Pins    : " << pinCount_      << "\n"
              << "  Package : " << package_       << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> IntegratedCircuit::getTypeParams() const {
    return {
        {"functionDesc",  functionDesc_},
        {"supplyVoltage", std::to_string(supplyVoltage_)},
        {"maxVoltage",    std::to_string(maxVoltage_)},
        {"maxCurrent",    std::to_string(maxCurrent_)},
        {"pinCount",      std::to_string(pinCount_)},
        {"package",       package_},
        {"packageType",   packageType_}
    };
}

std::string IntegratedCircuit::serialize() const {
    std::ostringstream oss;
    oss << "IC|" << baseSerial()
        << "|" << functionDesc_
        << "|" << std::fixed << std::setprecision(4)
        << supplyVoltage_ << "|" << maxVoltage_
        << "|" << maxCurrent_ << "|" << package_;
    return oss.str();
}
