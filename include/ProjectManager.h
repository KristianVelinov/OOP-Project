
#pragma once
#include "Project.h"
#include "Inventory.h"
#include <map>
#include <vector>
#include <string>

class ProjectManager {
    std::map<std::string, Project> projects_;
    Inventory&     inventory_;
    TransactionLog globalLog_;

public:
    explicit ProjectManager(Inventory& inv);

    Project& createProject(const std::string& name,
                           const std::string& desc = "");
    bool     hasProject(const std::string& name)  const;
    Project& getProject(const std::string& name);
    bool     deleteProject(const std::string& name);

    // Checkout: decrease inventory, record transaction
    bool checkoutForProject(const std::string& projName,
                            const std::string& compId,
                            int qty);
    // Return: increase inventory, record transaction
    bool returnToInventory(const std::string& projName,
                           const std::string& compId,
                           int qty);
    // Restock
    bool restockComponent(const std::string& compId, int qty);

    double getProjectCost(const std::string& projName) const;
    std::vector<std::string>  listProjects() const;

    TransactionLog&       getGlobalLog()       { return globalLog_; }
    const TransactionLog& getGlobalLog() const { return globalLog_; }
};
