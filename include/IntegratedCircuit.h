
#pragma once
#include "ActiveComponent.h"

class IntegratedCircuit : public ActiveComponent {
    std::string functionDesc_;
    double      supplyVoltage_; // V

public:
    IntegratedCircuit(const std::string& id, const std::string& name,
                      int qty, const std::string& functionDesc,
                      double supplyVoltage, double maxVoltage,
                      double maxCurrent,
                      const std::string& package = "SOIC-8",
                      int pinCount = 8,
                      int minStock = 1, double unitPrice = 0.0,
                      const std::string& catPath = "Semiconductors/ICs",
                      const std::string& dsPath  = "",
                      const std::string& footprint = "",
                      const std::string& packageType = "");

    const std::string& getFunctionDesc()  const { return functionDesc_; }
    double             getSupplyVoltage() const { return supplyVoltage_; }
    void setFunctionDesc(const std::string& v)  { functionDesc_ = v; }
    void setSupplyVoltage(double v)              { supplyVoltage_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "IC"; }
    Component*  clone()         const override { return new IntegratedCircuit(*this); }
    std::string serialize()     const override;
};
