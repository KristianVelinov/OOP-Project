
#pragma once
#include "BOM.h"
#include "TransactionLog.h"
#include "Inventory.h"
#include <string>

class Project {
    std::string    name_;
    std::string    description_;
    BOM            bom_;
    TransactionLog log_;

public:
    Project(const std::string& name, const std::string& description = "");

    const std::string& getName()       const { return name_; }
    const std::string& getDescription()const { return description_; }
    BOM&               getBOM()              { return bom_; }
    const BOM&         getBOM()        const { return bom_; }
    TransactionLog&    getLog()              { return log_; }
    const TransactionLog& getLog()     const { return log_; }

    void setDescription(const std::string& v) { description_ = v; }

    double calculateCost(const Inventory& inv) const;
};
