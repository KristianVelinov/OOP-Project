
#pragma once
#include "Inventory.h"
#include "ProjectManager.h"
#include <string>

/* Saves/loads the complete system state to/from a directory.
   Files: inventory.dat, taxonomy.dat, transactions.dat, projects.dat */
class Persistence {
public:
    static bool saveAll(const Inventory& inv,
                        const ProjectManager& pm,
                        const std::string& dir);

    static bool loadAll(Inventory& inv,
                        ProjectManager& pm,
                        const std::string& dir);

private:
    static bool saveInventory(const Inventory& inv,
                              const std::string& path);
    static bool saveTaxonomy(const Inventory& inv,
                              const std::string& path);
    static bool saveProjects(const ProjectManager& pm,
                              const std::string& path);
    static bool saveTransactions(const ProjectManager& pm,
                                  const std::string& path);

    static bool loadInventory(Inventory& inv,  const std::string& path);
    static bool loadTaxonomy(Inventory& inv,   const std::string& path);
    static bool loadProjects(ProjectManager& pm,const std::string& path);
    static bool loadTransactions(ProjectManager& pm,const std::string& path);
};
