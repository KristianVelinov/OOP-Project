
#pragma once
#include "PassiveComponent.h"

class Capacitor : public PassiveComponent {
    double      capacitance_; // Farads
    std::string capType_;     // ceramic/electrolytic/tantalum/film
    std::string package_;

public:
    Capacitor(const std::string& id, const std::string& name,
              int qty, double capacitance,
              const std::string& capType   = "ceramic",
              double tolerance   = 10.0,
              double voltageRating = 10.0,
              const std::string& package = "",
              int minStock = 2, double unitPrice = 0.0,
              const std::string& catPath = "Passives/Capacitors",
              const std::string& dsPath  = "",
              const std::string& footprint = "",
              int pinCount = 2);

    double             getCapacitance() const { return capacitance_; }
    const std::string& getCapType()     const { return capType_; }
    const std::string& getPackage()     const { return package_; }
    void setCapacitance(double v)             { capacitance_ = v; }
    void setCapType(const std::string& v)     { capType_ = v; }
    void setPackage(const std::string& v)     { package_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "CAPACITOR"; }
    Component*  clone()         const override { return new Capacitor(*this); }
    std::string serialize()     const override;
};
