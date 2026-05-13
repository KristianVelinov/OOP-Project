
#include "ProjectManager.h"
#include <stdexcept>

ProjectManager::ProjectManager(Inventory& inv) : inventory_(inv) {}

Project& ProjectManager::createProject(const std::string& name,
                                        const std::string& desc) {
    projects_.emplace(name, Project(name, desc));
    return projects_.at(name);
}

bool ProjectManager::hasProject(const std::string& name) const {
    return projects_.count(name) > 0;
}

Project& ProjectManager::getProject(const std::string& name) {
    auto it = projects_.find(name);
    if (it == projects_.end())
        throw std::runtime_error("Project not found: " + name);
    return it->second;
}

bool ProjectManager::deleteProject(const std::string& name) {
    return projects_.erase(name) > 0;
}

bool ProjectManager::checkoutForProject(const std::string& projName,
                                         const std::string& compId, int qty) {
    if (!hasProject(projName)) return false;
    if (!inventory_.updateQuantity(compId, -qty)) return false;
    Project& proj = getProject(projName);
    proj.getLog().record(compId, projName, qty, TransactionType::CHECKOUT);
    globalLog_.record(compId, projName, qty, TransactionType::CHECKOUT);
    return true;
}

bool ProjectManager::returnToInventory(const std::string& projName,
                                        const std::string& compId, int qty) {
    if (!hasProject(projName)) return false;
    inventory_.updateQuantity(compId, qty);
    Project& proj = getProject(projName);
    proj.getLog().record(compId, projName, qty, TransactionType::RETURN);
    globalLog_.record(compId, projName, qty, TransactionType::RETURN);
    return true;
}

bool ProjectManager::restockComponent(const std::string& compId, int qty) {
    if (!inventory_.updateQuantity(compId, qty)) return false;
    globalLog_.record(compId, "RESTOCK", qty, TransactionType::RESTOCK);
    return true;
}

double ProjectManager::getProjectCost(const std::string& projName) const {
    auto it = projects_.find(projName);
    if (it == projects_.end()) return 0.0;
    return it->second.calculateCost(inventory_);
}

std::vector<std::string> ProjectManager::listProjects() const {
    std::vector<std::string> out;
    for (auto& [name, _] : projects_) out.push_back(name);
    return out;
}
