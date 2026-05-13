
#include "ConnectorComponent.h"
#include <sstream>

ConnectorComponent::ConnectorComponent(
    const std::string& id, const std::string& name,
    int qty, const std::string& connectorType,
    double pitch, const std::string& gender,
    int pinCount, int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType)
    : Component(id, name, qty, minStock, unitPrice, catPath, dsPath,
                footprint, packageType, pinCount),
      connectorType_(connectorType), pitch_(pitch), gender_(gender)
{}

void ConnectorComponent::display() const {
    std::cout << "┌─ CONNECTOR ────────────────────────────────\n";
    displayBaseInfo();
    std::cout << "  Type  : " << connectorType_ << "\n"
              << "  Pitch : " << pitch_          << " mm\n"
              << "  Gender: " << gender_         << "\n"
              << "  Pins  : " << pinCount_       << "\n"
              << "└────────────────────────────────────────────\n";
}

std::map<std::string,std::string> ConnectorComponent::getTypeParams() const {
    return {
        {"connectorType", connectorType_},
        {"pitch",         std::to_string(pitch_)},
        {"gender",        gender_},
        {"pinCount",      std::to_string(pinCount_)},
        {"packageType",   packageType_}
    };
}

std::string ConnectorComponent::serialize() const {
    std::ostringstream oss;
    oss << "CONNECTOR|" << baseSerial()
        << "|" << connectorType_
        << "|" << std::fixed << std::setprecision(4) << pitch_
        << "|" << gender_;
    return oss.str();
}
