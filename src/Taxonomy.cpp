
#include "Taxonomy.h"
#include <sstream>

Taxonomy::Taxonomy() : root_(new TaxonomyNode("__root__")) {
    initDefaultTree();
}

Taxonomy::~Taxonomy() { delete root_; }

void Taxonomy::initDefaultTree() {
    auto mk = [&](const std::string& path){ createCategory("",path); };
    // Passives
    mk("Passives/Resistors/SMD");
    mk("Passives/Resistors/THT");
    mk("Passives/Capacitors/Ceramic");
    mk("Passives/Capacitors/Electrolytic");
    mk("Passives/Capacitors/Tantalum");
    mk("Passives/Capacitors/Film");
    mk("Passives/Inductors/SMD");
    mk("Passives/Inductors/THT");
    mk("Passives/Crystals");
    mk("Passives/Fuses");
    mk("Passives/Potentiometers");
    // Semiconductors
    mk("Semiconductors/Transistors/MOSFET/N-Channel");
    mk("Semiconductors/Transistors/MOSFET/P-Channel");
    mk("Semiconductors/Transistors/BJT/NPN");
    mk("Semiconductors/Transistors/BJT/PNP");
    mk("Semiconductors/Transistors/IGBT");
    mk("Semiconductors/Transistors/JFET");
    mk("Semiconductors/Diodes/Rectifier");
    mk("Semiconductors/Diodes/Schottky");
    mk("Semiconductors/Diodes/Zener");
    mk("Semiconductors/Diodes/TVS");
    mk("Semiconductors/Diodes/LED");
    mk("Semiconductors/ICs/Power/LDO");
    mk("Semiconductors/ICs/Power/DCDC");
    mk("Semiconductors/ICs/Power/DriveGate");
    mk("Semiconductors/ICs/OpAmps");
    mk("Semiconductors/ICs/Comparators");
    mk("Semiconductors/ICs/Microcontrollers");
    mk("Semiconductors/ICs/Logic");
    mk("Semiconductors/ICs/Memory");
    mk("Semiconductors/ICs/Communication");
    mk("Semiconductors/ICs/Protection");
    // Connectors
    mk("Connectors/PinHeaders");
    mk("Connectors/ScrewTerminals");
    mk("Connectors/USB");
    mk("Connectors/RF");
    mk("Connectors/JST");
    mk("Connectors/Molex");
    // Mechanical
    mk("Mechanical/Standoffs");
    mk("Mechanical/Hardware");
    mk("Mechanical/PCB");
    // Other
    mk("Other");
}

std::vector<std::string> Taxonomy::splitPath(const std::string& path) const {
    std::vector<std::string> parts;
    if (path.empty()) return parts;
    std::istringstream ss(path);
    std::string tok;
    while (std::getline(ss, tok, '/'))
        if (!tok.empty()) parts.push_back(tok);
    return parts;
}

TaxonomyNode* Taxonomy::navigate(const std::string& path) const {
    if (path.empty()) return root_;
    auto parts = splitPath(path);
    TaxonomyNode* cur = root_;
    for (auto& p : parts) {
        cur = cur->findChild(p);
        if (!cur) return nullptr;
    }
    return cur;
}

TaxonomyNode* Taxonomy::createCategory(const std::string& parentPath,
                                        const std::string& name) {
    // Support full path in name (e.g. name = "Passives/Resistors/SMD")
    std::string fullPath = parentPath.empty() ? name : parentPath + "/" + name;
    auto parts = splitPath(fullPath);
    TaxonomyNode* cur = root_;
    for (auto& p : parts)
        cur = cur->addChild(p);
    return cur;
}

bool Taxonomy::deleteCategory(const std::string& path) {
    auto parts = splitPath(path);
    if (parts.empty()) return false;
    std::string parentPath;
    for (int i = 0; i < (int)parts.size()-1; i++) {
        parentPath += (i ? "/" : "") + parts[i];
    }
    TaxonomyNode* parent = parentPath.empty() ? root_ : navigate(parentPath);
    if (!parent) return false;
    return parent->removeChild(parts.back());
}

std::vector<std::string> Taxonomy::listChildren(const std::string& path) const {
    TaxonomyNode* node = navigate(path);
    std::vector<std::string> result;
    if (!node) return result;
    for (auto* c : node->getChildren())
        result.push_back(c->getFullPath());
    return result;
}

void Taxonomy::collectPaths(TaxonomyNode* node,
                             const std::string& current,
                             std::vector<std::string>& out) const {
    if (node->getName() == "__root__") {
        for (auto* c : node->getChildren())
            collectPaths(c, c->getName(), out);
        return;
    }
    out.push_back(current);
    for (auto* c : node->getChildren())
        collectPaths(c, current + "/" + c->getName(), out);
}

std::vector<std::string> Taxonomy::allPaths() const {
    std::vector<std::string> out;
    collectPaths(root_, "", out);
    return out;
}
