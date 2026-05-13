
#pragma once
#include "PassiveComponent.h"

class Resistor : public PassiveComponent {
    double      resistance_;  // Ohm
    std::string package_;     // 0402/0603/0805/DIN0207/…

public:
    Resistor(const std::string& id, const std::string& name,
             int qty, double resistance,
             double tolerance  = 1.0,
             double powerRating= 0.1,
             const std::string& package = "",
             int minStock = 2, double unitPrice = 0.0,
             const std::string& catPath = "Passives/Resistors",
             const std::string& dsPath  = "",
             const std::string& footprint = "",
             int pinCount = 2);

    double             getResistance() const { return resistance_; }
    const std::string& getPackage()    const { return package_; }
    void setResistance(double v)             { resistance_ = v; }
    void setPackage(const std::string& v)    { package_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "RESISTOR"; }
    Component*  clone()         const override { return new Resistor(*this); }
    std::string serialize()     const override;
};
