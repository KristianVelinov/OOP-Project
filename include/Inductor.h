
#pragma once
#include "PassiveComponent.h"

class Inductor : public PassiveComponent {
    double      inductance_;   // Henry
    double      currentRating_;// A
    std::string package_;

public:
    Inductor(const std::string& id, const std::string& name,
             int qty, double inductance, double currentRating,
             double voltageRating = 0.0,
             const std::string& package = "",
             int minStock = 2, double unitPrice = 0.0,
             const std::string& catPath = "Passives/Inductors",
             const std::string& dsPath  = "",
             const std::string& footprint = "",
             int pinCount = 2);

    double             getInductance()   const { return inductance_; }
    double             getCurrentRating()const { return currentRating_; }
    const std::string& getPackage()      const { return package_; }
    void setInductance(double v)               { inductance_ = v; }
    void setCurrentRating(double v)            { currentRating_ = v; }
    void setPackage(const std::string& v)      { package_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "INDUCTOR"; }
    Component*  clone()         const override { return new Inductor(*this); }
    std::string serialize()     const override;
};
