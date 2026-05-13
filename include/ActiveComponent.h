
#pragma once
#include "Component.h"

/* Intermediate abstract class for active components.
   Groups max electrical ratings common to transistors, diodes, ICs. */
class ActiveComponent : public Component {
protected:
    double      maxVoltage_;
    double      maxCurrent_;
    std::string package_;

public:
    ActiveComponent(const std::string& id, const std::string& name,
                    int qty, double maxVoltage, double maxCurrent,
                    const std::string& package,
                    int minStock = 1, double unitPrice = 0.0,
                    const std::string& catPath = "Semiconductors",
                    const std::string& dsPath  = "",
                    const std::string& footprint = "",
                    const std::string& packageType = "",
                    int pinCount = 3);
    virtual ~ActiveComponent() = default;

    double             getMaxVoltage() const { return maxVoltage_; }
    double             getMaxCurrent() const { return maxCurrent_; }
    const std::string& getPackage()    const { return package_; }

    void setMaxVoltage(double v)           { maxVoltage_ = v; }
    void setMaxCurrent(double v)           { maxCurrent_ = v; }
    void setPackage(const std::string& v)  { package_ = v; }

    virtual void display() const override = 0;
    virtual std::map<std::string,std::string> getTypeParams() const override = 0;
    virtual std::string getTypeName() const override = 0;
    virtual Component*  clone()       const override = 0;
};
