
#include "TaxonomyNode.h"

TaxonomyNode::TaxonomyNode(const std::string& name, TaxonomyNode* parent)
    : name_(name), parent_(parent)
{}

TaxonomyNode::~TaxonomyNode() {
    for (auto* c : children_) delete c;
}

TaxonomyNode* TaxonomyNode::addChild(const std::string& name) {
    auto* existing = findChild(name);
    if (existing) return existing;
    auto* node = new TaxonomyNode(name, this);
    children_.push_back(node);
    return node;
}

TaxonomyNode* TaxonomyNode::findChild(const std::string& name) const {
    for (auto* c : children_)
        if (c->name_ == name) return c;
    return nullptr;
}

TaxonomyNode* TaxonomyNode::ensureOtherNode() {
    return addChild("Other");
}

std::string TaxonomyNode::getFullPath() const {
    if (!parent_ || parent_->name_ == "__root__") return name_;
    return parent_->getFullPath() + "/" + name_;
}

bool TaxonomyNode::removeChild(const std::string& name) {
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        if ((*it)->name_ == name) {
            delete *it;
            children_.erase(it);
            return true;
        }
    }
    return false;
}
