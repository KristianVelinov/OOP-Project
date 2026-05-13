
#include "Persistence.h"
#include "ComponentFactory.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

bool Persistence::saveAll(const Inventory& inv,
                           const ProjectManager& pm,
                           const std::string& dir) {
    fs::create_directories(dir);
    bool ok = true;
    ok &= saveInventory(inv,    dir + "/inventory.dat");
    ok &= saveTaxonomy(inv,     dir + "/taxonomy.dat");
    ok &= saveProjects(pm,      dir + "/projects.dat");
    ok &= saveTransactions(pm,  dir + "/transactions.dat");
    return ok;
}

bool Persistence::saveInventory(const Inventory& inv,
                                 const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "# ECIMS Inventory v1\n";
    for (auto* c : inv.getAll())
        f << c->serialize() << "\n";
    return true;
}

bool Persistence::saveTaxonomy(const Inventory& inv,
                                const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "# ECIMS Taxonomy v1\n";
    for (auto& p : inv.getTaxonomy().allPaths())
        f << p << "\n";
    return true;
}

bool Persistence::saveProjects(const ProjectManager& pm,
                                const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "# ECIMS Projects v1\n";
    for (auto& name : pm.listProjects()) {
        const Project& p = const_cast<ProjectManager&>(pm).getProject(name);
        f << p.getName() << "|" << p.getDescription() << "\n";
    }
    return true;
}

bool Persistence::saveTransactions(const ProjectManager& pm,
                                    const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return false;
    f << "# ECIMS Transactions v1\n";
    for (auto& tx : pm.getGlobalLog().getAll())
        f << tx.serialize() << "\n";
    return true;
}

bool Persistence::loadAll(Inventory& inv,
                           ProjectManager& pm,
                           const std::string& dir) {
    if (!fs::exists(dir)) return false;
    loadTaxonomy(inv,  dir + "/taxonomy.dat");
    loadInventory(inv, dir + "/inventory.dat");
    loadProjects(pm,   dir + "/projects.dat");
    loadTransactions(pm, dir + "/transactions.dat");
    return true;
}

bool Persistence::loadInventory(Inventory& inv, const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        Component* c = ComponentFactory::fromLine(line);
        if (c) inv.addComponent(c);
    }
    return true;
}

bool Persistence::loadTaxonomy(Inventory& inv, const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        inv.getTaxonomy().createCategory("", line);
    }
    return true;
}

bool Persistence::loadProjects(ProjectManager& pm, const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto sep = line.find('|');
        std::string name = line.substr(0, sep);
        std::string desc = (sep != std::string::npos) ? line.substr(sep+1) : "";
        pm.createProject(name, desc);
    }
    return true;
}

bool Persistence::loadTransactions(ProjectManager& pm,
                                    const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        try {
            Transaction tx = Transaction::deserialize(line);
            pm.getGlobalLog().addRaw(tx);
        } catch(...) {}
    }
    return true;
}
