
#include "Inventory.h"
#include <cctype>
#include <sstream>

Inventory::Inventory() : nextId_(1) {}

std::string Inventory::generateId(const std::string& prefix) {
    std::ostringstream oss;
    oss << prefix << std::setw(4) << std::setfill('0') << nextId_++;
    while (components_.count(oss.str())) {
        oss.str("");
        oss << prefix << std::setw(4) << std::setfill('0') << nextId_++;
    }
    return oss.str();
}

std::string Inventory::addComponent(Component* c) {
    if (c->getId().empty()) {
        // generate id based on type prefix
        std::string pfx = c->getTypeName().substr(0,3);
        std::string id = generateId(pfx);
        // We can't change id directly; reconstruct? – just set via ugly cast
        // We store it as-is; if empty, auto-assign
        // Better: id is set before add; enforce externally.
    }
    std::string id = c->getId();
    if (id.empty()) id = generateId("CMP");
    components_[id] = std::unique_ptr<Component>(c);
    // Ensure category exists
    taxonomy_.createCategory("", c->getCategoryPath());
    return id;
}

bool Inventory::removeComponent(const std::string& id) {
    return components_.erase(id) > 0;
}

Component* Inventory::getComponent(const std::string& id) const {
    auto it = components_.find(id);
    return (it != components_.end()) ? it->second.get() : nullptr;
}

bool Inventory::updateQuantity(const std::string& id, int delta) {
    Component* c = getComponent(id);
    if (!c) return false;
    int newQty = c->getQuantity() + delta;
    if (newQty < 0) return false;
    c->setQuantity(newQty);
    return true;
}

std::vector<Component*> Inventory::getAll() const {
    std::vector<Component*> out;
    out.reserve(components_.size());
    for (auto& [id, ptr] : components_)
        out.push_back(ptr.get());
    return out;
}

std::vector<Component*> Inventory::getByCategory(const std::string& path) const {
    std::vector<Component*> out;
    for (auto& [id, ptr] : components_)
        if (ptr->getCategoryPath().find(path) == 0)
            out.push_back(ptr.get());
    return out;
}

std::string Inventory::toLower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return r;
}

std::vector<Component*> Inventory::searchByName(const std::string& q) const {
    std::string ql = toLower(q);
    std::vector<Component*> out;
    for (auto& [id, ptr] : components_)
        if (toLower(ptr->getName()).find(ql) != std::string::npos ||
            toLower(ptr->getId()).find(ql)   != std::string::npos)
            out.push_back(ptr.get());
    return out;
}

std::vector<Component*> Inventory::getLowStock() const {
    std::vector<Component*> out;
    for (auto& [id, ptr] : components_)
        if (ptr->isLowStock()) out.push_back(ptr.get());
    return out;
}

std::vector<Component*> Inventory::getByType(const std::string& typeName) const {
    std::vector<Component*> out;
    for (auto& [id, ptr] : components_)
        if (ptr->getTypeName() == typeName) out.push_back(ptr.get());
    return out;
}

std::vector<Component*> Inventory::getByPackageType(const std::string& pkg) const {
    std::string pkgu = pkg;
    std::transform(pkgu.begin(), pkgu.end(), pkgu.begin(), ::toupper);
    std::vector<Component*> out;
    for (auto& [id, ptr] : components_) {
        std::string ptu = ptr->getPackageType();
        std::transform(ptu.begin(), ptu.end(), ptu.begin(), ::toupper);
        if (ptu.find(pkgu) != std::string::npos)
            out.push_back(ptr.get());
    }
    return out;
}

std::vector<Component*> Inventory::getByPinCount(int pins) const {
    std::vector<Component*> out;
    for (auto& [id, ptr] : components_)
        if (ptr->getPinCount() == pins) out.push_back(ptr.get());
    return out;
}
