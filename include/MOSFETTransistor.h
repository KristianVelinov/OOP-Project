
#pragma once
#include "Transistor.h"

class MOSFETTransistor : public Transistor {
    double vgsThreshold_; // V
    double rdsOn_;        // Ohm
    double gateCharge_;   // nC

public:
    MOSFETTransistor(const std::string& id, const std::string& name,
                     int qty, const std::string& transistorType,
                     double vgsThreshold, double rdsOn, double gateCharge,
                     double maxVoltage, double maxCurrent,
                     const std::string& package = "SOT-23",
                     int minStock = 1, double unitPrice = 0.0,
                     const std::string& catPath = "Semiconductors/Transistors/MOSFET",
                     const std::string& dsPath  = "",
                     const std::string& footprint = "",
                     const std::string& packageType = "",
                     int pinCount = 3);

    double getVgsThreshold() const { return vgsThreshold_; }
    double getRdsOn()        const { return rdsOn_; }
    double getGateCharge()   const { return gateCharge_; }
    void setVgsThreshold(double v) { vgsThreshold_ = v; }
    void setRdsOn(double v)        { rdsOn_ = v; }
    void setGateCharge(double v)   { gateCharge_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "MOSFET"; }
    Component*  clone()         const override { return new MOSFETTransistor(*this); }
    std::string serialize()     const override;
};
