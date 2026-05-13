
#pragma once
#include "Component.h"

class ConnectorComponent : public Component {
    std::string connectorType_; // JST/Molex/USB-C/SMA/PinHeader/…
    double      pitch_;         // mm
    std::string gender_;        // male/female/hermaphroditic

public:
    ConnectorComponent(const std::string& id, const std::string& name,
                       int qty, const std::string& connectorType,
                       double pitch, const std::string& gender,
                       int pinCount = 2,
                       int minStock = 1, double unitPrice = 0.0,
                       const std::string& catPath = "Connectors",
                       const std::string& dsPath  = "",
                       const std::string& footprint = "",
                       const std::string& packageType = "");

    const std::string& getConnectorType() const { return connectorType_; }
    double             getPitch()          const { return pitch_; }
    const std::string& getGender()         const { return gender_; }
    void setConnectorType(const std::string& v)  { connectorType_ = v; }
    void setPitch(double v)                      { pitch_ = v; }
    void setGender(const std::string& v)         { gender_ = v; }

    void        display()       const override;
    std::map<std::string,std::string> getTypeParams() const override;
    std::string getTypeName()   const override { return "CONNECTOR"; }
    Component*  clone()         const override { return new ConnectorComponent(*this); }
    std::string serialize()     const override;
};
