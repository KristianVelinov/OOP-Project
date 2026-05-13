
#include "MOSFETTransistor.h"
#include <sstream>

MOSFETTransistor::MOSFETTransistor(
    const std::string& id, const std::string& name,
    int qty, const std::string& transistorType,
    double vgsThreshold, double rdsOn, double gateCharge,
    double maxVoltage, double maxCurrent,
    const std::string& package, int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : Transistor(id, name, qty, transistorType, 0.0,
                 maxVoltage, maxCurrent, package,
                 minStock, unitPrice, catPath, dsPath,
                 footprint, packageType, pinCount),
      vgsThreshold_(vgsThreshold), rdsOn_(rdsOn), gateCharge_(gateCharge)
{}

void MOSFETTransistor::display() const {
    std::cout << "┌─ MOSFET ───────────────────────────────────\n";
    displayBaseInfo();
    std::cout << "  Type    : " << transistorType_ << "\n"
              << "  Vgs(th) : " << vgsThreshold_   << " V\n"
              << "  Rds(on) : " << rdsOn_           << " Ω\n"
              << "  Qg      : " << gateCharge_      << " nC\n"
              << "  Vmax    : " << maxVoltage_       << " V\n"
              << "  Imax    : " << maxCurrent_       << " A\n"
              << "  Package : " << package_          << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> MOSFETTransistor::getTypeParams() const {
    return {
        {"transistorType", transistorType_},
        {"vgsThreshold",   std::to_string(vgsThreshold_)},
        {"rdsOn",          std::to_string(rdsOn_)},
        {"gateCharge",     std::to_string(gateCharge_)},
        {"maxVoltage",     std::to_string(maxVoltage_)},
        {"maxCurrent",     std::to_string(maxCurrent_)},
        {"package",        package_},
        {"pinCount",       std::to_string(pinCount_)},
        {"packageType",    packageType_}
    };
}

std::string MOSFETTransistor::serialize() const {
    std::ostringstream oss;
    oss << "MOSFET|" << baseSerial()
        << "|" << transistorType_
        << "|" << std::fixed << std::setprecision(6)
        << vgsThreshold_ << "|" << rdsOn_
        << "|" << gateCharge_
        << "|" << maxVoltage_ << "|" << maxCurrent_
        << "|" << package_;
    return oss.str();
}
