
#include "Capacitor.h"
#include <sstream>

Capacitor::Capacitor(const std::string& id, const std::string& name,
                     int qty, double capacitance,
                     const std::string& capType,
                     double tolerance, double voltageRating,
                     const std::string& package,
                     int minStock, double unitPrice,
                     const std::string& catPath, const std::string& dsPath,
                     const std::string& footprint, int pinCount)
    : PassiveComponent(id, name, qty, tolerance, voltageRating, 0.0,
                       minStock, unitPrice, catPath, dsPath,
                       footprint, package, pinCount),
      capacitance_(capacitance), capType_(capType), package_(package)
{}

void Capacitor::display() const {
    std::cout << "┌─ CAPACITOR ────────────────────────────────\n";
    displayBaseInfo();
    // Nice unit display
    double c = capacitance_;
    std::string unit = "F";
    if (c < 1e-9)       { c *= 1e12; unit = "pF"; }
    else if (c < 1e-6)  { c *= 1e9;  unit = "nF"; }
    else if (c < 1e-3)  { c *= 1e6;  unit = "uF"; }
    else if (c < 1.0)   { c *= 1e3;  unit = "mF"; }
    std::cout << "  Capacitance: " << c << " " << unit << "\n"
              << "  Type       : " << capType_       << "\n"
              << "  Tolerance  : " << tolerance_     << " %\n"
              << "  Voltage    : " << voltageRating_  << " V\n"
              << "  Package    : " << package_       << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> Capacitor::getTypeParams() const {
    return {
        {"capacitance",   std::to_string(capacitance_)},
        {"tolerance",     std::to_string(tolerance_)},
        {"voltageRating", std::to_string(voltageRating_)},
        {"capType",       capType_},
        {"package",       package_},
        {"pinCount",      std::to_string(pinCount_)},
        {"packageType",   packageType_}
    };
}

std::string Capacitor::serialize() const {
    std::ostringstream oss;
    oss << "CAPACITOR|" << baseSerial()
        << "|" << std::fixed << std::setprecision(12)
        << capacitance_ << "|" << capType_
        << "|" << tolerance_ << "|" << voltageRating_
        << "|" << package_;
    return oss.str();
}
