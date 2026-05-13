
#include "Resistor.h"
#include <sstream>

Resistor::Resistor(const std::string& id, const std::string& name,
                   int qty, double resistance,
                   double tolerance, double powerRating,
                   const std::string& package,
                   int minStock, double unitPrice,
                   const std::string& catPath, const std::string& dsPath,
                   const std::string& footprint, int pinCount)
    : PassiveComponent(id, name, qty, tolerance, 0.0, powerRating,
                       minStock, unitPrice, catPath, dsPath,
                       footprint, package, pinCount),
      resistance_(resistance), package_(package)
{}

void Resistor::display() const {
    std::cout << "┌─ RESISTOR ─────────────────────────────────\n";
    displayBaseInfo();
    std::cout << "  Resistance: " << resistance_ << " Ω\n"
              << "  Tolerance : " << tolerance_  << " %\n"
              << "  Power     : " << powerRating_ << " W\n"
              << "  Package   : " << package_    << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> Resistor::getTypeParams() const {
    return {
        {"resistance",   std::to_string(resistance_)},
        {"tolerance",    std::to_string(tolerance_)},
        {"powerRating",  std::to_string(powerRating_)},
        {"package",      package_},
        {"pinCount",     std::to_string(pinCount_)},
        {"packageType",  packageType_}
    };
}

std::string Resistor::serialize() const {
    std::ostringstream oss;
    oss << "RESISTOR|" << baseSerial()
        << "|" << std::fixed << std::setprecision(6)
        << resistance_ << "|" << tolerance_
        << "|" << powerRating_ << "|" << package_;
    return oss.str();
}
