
#pragma once
#include "ActiveComponent.h"

/* Second-level abstract for transistors (BJT, MOSFET, etc.).
   Adds type (NPN/PNP/N-CH/P-CH) and DC current gain. */
class Transistor : public ActiveComponent {
protected:
    std::string transistorType_; // NPN / PNP / N-CH / P-CH
    double      hfe_;            // DC current gain (BJTs) or gm (MOSFETs placeholder)

public:
    Transistor(const std::string& id, const std::string& name,
               int qty, const std::string& transistorType,
               double hfe, double maxVoltage, double maxCurrent,
               const std::string& package,
               int minStock = 1, double unitPrice = 0.0,
               const std::string& catPath = "Semiconductors/Transistors",
               const std::string& dsPath  = "",
               const std::string& footprint = "",
               const std::string& packageType = "",
               int pinCount = 3);
    virtual ~Transistor() = default;

    const std::string& getTransistorType() const { return transistorType_; }
    double             getHFE()            const { return hfe_; }
    void setTransistorType(const std::string& v) { transistorType_ = v; }
    void setHFE(double v)                        { hfe_ = v; }

    virtual void display() const override = 0;
    virtual std::map<std::string,std::string> getTypeParams() const override = 0;
    virtual std::string getTypeName() const override = 0;
    virtual Component*  clone()       const override = 0;
};
