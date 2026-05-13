
#pragma once
#include "Component.h"
#include <vector>

/* Stores components with completely unknown / user-defined parameters.
   Uses a map<string,string> for maximum runtime flexibility.
   Participates in parametric search via getTypeParams(). */
class ArbitraryComponent : public Component {
    std::string description_;
    std::map<std::string,std::string> customParams_;

public:
    ArbitraryComponent(const std::string& id, const std::string& name,
                       int qty, const std::string& description = "",
                       int minStock = 1, double unitPrice = 0.0,
                       const std::string& catPath = "Other",
                       const std::string& dsPath  = "",
                       const std::string& footprint = "",
                       const std::string& packageType = "",
                       int pinCount = 0);

    void        addParam(const std::string& key, const std::string& val){ customParams_[key]=val; }
    std::string getParam(const std::string& key) const;
    bool        hasParam(const std::string& key) const;
    const std::map<std::string,std::string>& getAllParams() const { return customParams_; }
    const std::string& getDescription() const { return description_; }
    void setDescription(const std::string& v)  { description_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override { return customParams_; }
    std::string getTypeName()   const override { return "ARBITRARY"; }
    Component*  clone()         const override { return new ArbitraryComponent(*this); }
    std::string serialize()     const override;
};
