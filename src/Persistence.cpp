
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
    std::string temp_path = path + ".tmp";
    std::ofstream f(temp_path);
    if (!f.is_open()) return false;
    
    try {
        f << "# ECIMS Inventory v3\n";
        int count = 0;
        int skipped = 0;
        for (auto* c : inv.getAll()) {
            if (!c) {
                skipped++;
                continue;
            }
            std::string line = c->serialize();
            
            // Validate serialization: must have pipe delimiters and match pattern
            if (line.empty()) {
                std::cerr << "Persistence: Empty serialization for " << c->getId() << "\n";
                skipped++;
                continue;
            }
            
            // Quick sanity check: should have at least 10 pipes for base fields
            int pipe_count = 0;
            for (char ch : line) if (ch == '|') pipe_count++;
            if (pipe_count < 10) {
                std::cerr << "Persistence: Bad serialization (only " << pipe_count 
                          << " pipes) for " << c->getId() << ": " << line.substr(0, 50) << "\n";
                skipped++;
                continue;
            }
            
            f << line << "\n";
            if (!f.good()) {
                std::cerr << "Persistence: Write error at component " << count << "\n";
                f.close();
                fs::remove(temp_path);
                return false;
            }
            count++;
        }
        
        if (skipped > 0) {
            std::cerr << "Persistence: Skipped " << skipped << " corrupted components\n";
        }
        
        f.close();
        if (!f.good()) {
            std::cerr << "Persistence: File close failed\n";
            fs::remove(temp_path);
            return false;
        }
        
        // Atomic rename: backup old file, move new file into place
        if (fs::exists(path)) {
            std::string backup = path + ".bak";
            fs::remove(backup);  // Remove old backup if it exists
            fs::rename(path, backup);
        }
        fs::rename(temp_path, path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Persistence: Exception during saveInventory: " << e.what() << "\n";
        f.close();
        fs::remove(temp_path);
        return false;
    }
}

bool Persistence::saveTaxonomy(const Inventory& inv,
                                const std::string& path) {
    std::string temp_path = path + ".tmp";
    std::ofstream f(temp_path);
    if (!f.is_open()) return false;
    try {
        f << "# ECIMS Taxonomy v3\n";
        for (auto& p : inv.getTaxonomy().allPaths()) {
            f << p << "\n";
            if (!f.good()) {
                f.close();
                fs::remove(temp_path);
                return false;
            }
        }
        f.close();
        if (fs::exists(path)) {
            std::string backup = path + ".bak";
            fs::remove(backup);
            fs::rename(path, backup);
        }
        fs::rename(temp_path, path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Persistence: Exception during saveTaxonomy: " << e.what() << "\n";
        f.close();
        fs::remove(temp_path);
        return false;
    }
}

bool Persistence::saveProjects(const ProjectManager& pm,
                                const std::string& path) {
    std::string temp_path = path + ".tmp";
    std::ofstream f(temp_path);
    if (!f.is_open()) return false;
    try {
        f << "# ECIMS Projects v3\n";
        for (auto& name : pm.listProjects()) {
            const Project& p = const_cast<ProjectManager&>(pm).getProject(name);
            f << p.getName() << "|" << p.getDescription() << "\n";
            if (!f.good()) {
                f.close();
                fs::remove(temp_path);
                return false;
            }
        }
        f.close();
        if (fs::exists(path)) {
            std::string backup = path + ".bak";
            fs::remove(backup);
            fs::rename(path, backup);
        }
        fs::rename(temp_path, path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Persistence: Exception during saveProjects: " << e.what() << "\n";
        f.close();
        fs::remove(temp_path);
        return false;
    }
}

bool Persistence::saveTransactions(const ProjectManager& pm,
                                    const std::string& path) {
    std::string temp_path = path + ".tmp";
    std::ofstream f(temp_path);
    if (!f.is_open()) return false;
    try {
        f << "# ECIMS Transactions v3\n";
        for (auto& tx : pm.getGlobalLog().getAll()) {
            f << tx.serialize() << "\n";
            if (!f.good()) {
                f.close();
                fs::remove(temp_path);
                return false;
            }
        }
        f.close();
        if (fs::exists(path)) {
            std::string backup = path + ".bak";
            fs::remove(backup);
            fs::rename(path, backup);
        }
        fs::rename(temp_path, path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Persistence: Exception during saveTransactions: " << e.what() << "\n";
        f.close();
        fs::remove(temp_path);
        return false;
    }
}

bool Persistence::loadAll(Inventory& inv,
                           ProjectManager& pm,
                           const std::string& dir) {
    bool ok = true;
    ok &= loadInventory(inv,    dir + "/inventory.dat");
    ok &= loadTaxonomy(inv,     dir + "/taxonomy.dat");
    ok &= loadProjects(pm,      dir + "/projects.dat");
    ok &= loadTransactions(pm,  dir + "/transactions.dat");
    
    if (ok) {
        saveAll(inv, pm, dir);
    }
    
    return ok;
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
