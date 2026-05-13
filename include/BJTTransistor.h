
#pragma once
#include "Transistor.h"

class BJTTransistor : public Transistor {
    double vceo_;  // Collector-Emitter voltage V
    double icMax_; // Max collector current A

public:
    BJTTransistor(const std::string& id, const std::string& name,
                  int qty, const std::string& transistorType,
                  double hfe, double vceo, double icMax,
                  const std::string& package = "TO-92",
                  int minStock = 1, double unitPrice = 0.0,
                  const std::string& catPath = "Semiconductors/Transistors/BJT",
                  const std::string& dsPath  = "",
                  const std::string& footprint = "",
                  const std::string& packageType = "",
                  int pinCount = 3);

    double getVceo()  const { return vceo_; }
    double getIcMax() const { return icMax_; }
    void setVceo(double v)  { vceo_ = v; }
    void setIcMax(double v) { icMax_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "BJT"; }
    Component*  clone()         const override { return new BJTTransistor(*this); }
    std::string serialize()     const override;
};
