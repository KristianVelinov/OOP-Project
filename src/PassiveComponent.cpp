
#include "PassiveComponent.h"

PassiveComponent::PassiveComponent(
    const std::string& id, const std::string& name,
    int qty, double tolerance, double voltageRating, double powerRating,
    int minStock, double unitPrice,
    const std::string& catPath, const std::string& dsPath,
    const std::string& footprint, const std::string& packageType, int pinCount)
    : Component(id, name, qty, minStock, unitPrice, catPath, dsPath,
                footprint, packageType, pinCount),
      tolerance_(tolerance), voltageRating_(voltageRating),
      powerRating_(powerRating)
{}
