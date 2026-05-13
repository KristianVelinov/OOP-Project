
#include "BJTTransistor.h"
#include <sstream>

BJTTransistor::BJTTransistor(
    const std::string& id, const std::string& name,
    int qty, const std::string& transistorType,
    double hfe, double vceo, double icMax,
    const std::string& package, int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : Transistor(id, name, qty, transistorType, hfe,
                 vceo, icMax, package,
                 minStock, unitPrice, catPath, dsPath,
                 footprint, packageType, pinCount),
      vceo_(vceo), icMax_(icMax)
{}

void BJTTransistor::display() const {
    std::cout << "┌─ BJT TRANSISTOR ───────────────────────────\n";
    displayBaseInfo();
    std::cout << "  Type    : " << transistorType_ << "\n"
              << "  hFE     : " << hfe_             << "\n"
              << "  Vceo    : " << vceo_             << " V\n"
              << "  Ic(max) : " << icMax_            << " A\n"
              << "  Package : " << package_          << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> BJTTransistor::getTypeParams() const {
    return {
        {"transistorType", transistorType_},
        {"hfe",            std::to_string(hfe_)},
        {"vceo",           std::to_string(vceo_)},
        {"icMax",          std::to_string(icMax_)},
        {"maxVoltage",     std::to_string(maxVoltage_)},
        {"maxCurrent",     std::to_string(maxCurrent_)},
        {"package",        package_},
        {"pinCount",       std::to_string(pinCount_)},
        {"packageType",    packageType_}
    };
}

std::string BJTTransistor::serialize() const {
    std::ostringstream oss;
    oss << "BJT|" << baseSerial()
        << "|" << transistorType_
        << "|" << std::fixed << std::setprecision(4)
        << hfe_ << "|" << vceo_ << "|" << icMax_
        << "|" << package_;
    return oss.str();
}
