
#pragma once
#include "Component.h"
#include "Taxonomy.h"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

/* Central storage.  Owns all Component objects via unique_ptr.
   External callers receive raw (non-owning) Component* pointers. */
class Inventory {
    std::map<std::string, std::unique_ptr<Component>> components_;
    Taxonomy taxonomy_;
    int      nextId_;

public:
    Inventory();

    // ── CRUD ────────────────────────────────────────────────
    std::string addComponent(Component* c);       // takes ownership; returns id
    bool        removeComponent(const std::string& id);
    Component*  getComponent(const std::string& id) const;
    bool        updateQuantity(const std::string& id, int delta); // +add / -remove

    // ── Queries ──────────────────────────────────────────────
    std::vector<Component*> getAll()              const;
    std::vector<Component*> getByCategory(const std::string& path) const;
    std::vector<Component*> searchByName(const std::string& q)     const;
    std::vector<Component*> getLowStock()                           const;
    std::vector<Component*> getByType(const std::string& typeName) const;
    std::vector<Component*> getByPackageType(const std::string& pkg)const;
    std::vector<Component*> getByPinCount(int pins)                 const;

    // ── Taxonomy access ──────────────────────────────────────
    Taxonomy&       getTaxonomy()       { return taxonomy_; }
    const Taxonomy& getTaxonomy() const { return taxonomy_; }

    int  size() const { return static_cast<int>(components_.size()); }

    // ── ID generation ────────────────────────────────────────
    std::string generateId(const std::string& prefix = "CMP");

private:
    static std::string toLower(const std::string& s);
};
