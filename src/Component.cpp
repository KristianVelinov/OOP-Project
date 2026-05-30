
#include "Component.h"
#include <sstream>

Component::Component(const std::string& id, const std::string& name,
                     int qty, int minStock, double unitPrice,
                     const std::string& catPath, const std::string& dsPath,
                     const std::string& footprint, const std::string& packageType,
                     int pinCount)
    : id_(id), name_(name), quantity_(qty), minStock_(minStock),
      unitPrice_(unitPrice), categoryPath_(catPath), datasheetPath_(dsPath),
      footprint_(footprint), packageType_(packageType), pinCount_(pinCount)
{}

void Component::displayBaseInfo() const {
    std::cout << "  ID        : " << id_           << "\n"
              << "  Name      : " << name_          << "\n"
              << "  Type      : " << getTypeName()  << "\n"
              << "  Qty       : " << quantity_
                    << "  (min " << minStock_ << ")"
                    << (isLowStock() ? "  [!LOW STOCK]" : "") << "\n"
              << "  Price     : €" << std::fixed << std::setprecision(4)
                    << unitPrice_ << "\n"
              << "  Category  : " << categoryPath_ << "\n"
              << "  Package   : " << (packageType_.empty() ? "-" : packageType_)
                    << "  Pins: " << pinCount_      << "\n"
              << "  Footprint : " << (footprint_.empty() ? "-" : footprint_) << "\n"
              << "  Datasheet : " << (datasheetPath_.empty() ? "-" : datasheetPath_) << "\n";
}

std::string Component::baseSerial() const {
    std::ostringstream oss;
    oss << id_ << "|" << name_ << "|" << quantity_ << "|" << minStock_
        << "|" << std::fixed << std::setprecision(6) << unitPrice_
        << "|" << categoryPath_ << "|" << datasheetPath_
        << "|" << footprint_   << "|" << packageType_
        << "|" << pinCount_;
    return oss.str();
}

std::string Component::serialize() const {
    return getTypeName() + "|" + baseSerial();
}
