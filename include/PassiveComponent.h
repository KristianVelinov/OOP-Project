
#pragma once
#include "Component.h"

/* Intermediate abstract class for passive components (R, C, L).
   Groups shared electrical properties to avoid duplication across
   three concrete classes.  */
class PassiveComponent : public Component {
protected:
    double tolerance_;    // percent
    double voltageRating_;// V
    double powerRating_;  // W

public:
    PassiveComponent(const std::string& id, const std::string& name,
                     int qty, double tolerance, double voltageRating,
                     double powerRating,
                     int minStock = 1, double unitPrice = 0.0,
                     const std::string& catPath = "Passives",
                     const std::string& dsPath  = "",
                     const std::string& footprint = "",
                     const std::string& packageType = "",
                     int pinCount = 2);
    virtual ~PassiveComponent() = default;

    double getTolerance()    const { return tolerance_; }
    double getVoltageRating()const { return voltageRating_; }
    double getPowerRating()  const { return powerRating_; }

    void setTolerance(double v)    { tolerance_ = v; }
    void setVoltageRating(double v){ voltageRating_ = v; }
    void setPowerRating(double v)  { powerRating_ = v; }

    virtual void display() const override = 0;
    virtual std::map<std::string,std::string> getTypeParams() const override = 0;
    virtual std::string getTypeName() const override = 0;
    virtual Component*  clone()       const override = 0;
};
