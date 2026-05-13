
#pragma once
#include "ActiveComponent.h"

class DiodeComponent : public ActiveComponent {
    double      forwardVoltage_; // Vf
    double      reverseVoltage_; // VRM
    std::string diodeType_;      // schottky/zener/TVS/rectifier/LED

public:
    DiodeComponent(const std::string& id, const std::string& name,
                   int qty, double forwardVoltage, double reverseVoltage,
                   const std::string& diodeType, double maxCurrent,
                   const std::string& package = "DO-41",
                   int minStock = 1, double unitPrice = 0.0,
                   const std::string& catPath = "Semiconductors/Diodes",
                   const std::string& dsPath  = "",
                   const std::string& footprint = "",
                   const std::string& packageType = "",
                   int pinCount = 2);

    double             getForwardVoltage() const { return forwardVoltage_; }
    double             getReverseVoltage() const { return reverseVoltage_; }
    const std::string& getDiodeType()      const { return diodeType_; }
    void setForwardVoltage(double v)             { forwardVoltage_ = v; }
    void setReverseVoltage(double v)             { reverseVoltage_ = v; }
    void setDiodeType(const std::string& v)      { diodeType_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "DIODE"; }
    Component*  clone()         const override { return new DiodeComponent(*this); }
    std::string serialize()     const override;
};
