
#include "Inductor.h"
#include <sstream>

Inductor::Inductor(const std::string& id, const std::string& name,
                   int qty, double inductance, double currentRating,
                   double voltageRating,
                   const std::string& package,
                   int minStock, double unitPrice,
                   const std::string& catPath, const std::string& dsPath,
                   const std::string& footprint, int pinCount)
    : PassiveComponent(id, name, qty, 0.0, voltageRating, 0.0,
                       minStock, unitPrice, catPath, dsPath,
                       footprint, package, pinCount),
      inductance_(inductance), currentRating_(currentRating), package_(package)
{}

void Inductor::display() const {
    std::cout << "┌─ INDUCTOR ─────────────────────────────────\n";
    displayBaseInfo();
    double l = inductance_;
    std::string unit = "H";
    if (l < 1e-6) { l *= 1e9; unit = "nH"; }
    else if (l < 1e-3) { l *= 1e6; unit = "uH"; }
    else if (l < 1.0)  { l *= 1e3; unit = "mH"; }
    std::cout << "  Inductance  : " << l << " " << unit << "\n"
              << "  Imax        : " << currentRating_ << " A\n"
              << "  Package     : " << package_      << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> Inductor::getTypeParams() const {
    return {
        {"inductance",    std::to_string(inductance_)},
        {"currentRating", std::to_string(currentRating_)},
        {"package",       package_},
        {"pinCount",      std::to_string(pinCount_)},
        {"packageType",   packageType_}
    };
}

std::string Inductor::serialize() const {
    std::ostringstream oss;
    oss << "INDUCTOR|" << baseSerial()
        << "|" << std::fixed << std::setprecision(9)
        << inductance_ << "|" << currentRating_
        << "|" << voltageRating_ << "|" << package_;
    return oss.str();
}
