
#pragma once
#include <string>
#include <vector>
#include <algorithm>

/* A single node in the category tree.
   TaxonomyNode owns its children (destructor deletes them recursively). */
class TaxonomyNode {
    std::string              name_;
    TaxonomyNode*            parent_;
    std::vector<TaxonomyNode*> children_;

public:
    explicit TaxonomyNode(const std::string& name, TaxonomyNode* parent = nullptr);
    ~TaxonomyNode(); // recursive delete

    TaxonomyNode* addChild(const std::string& name);
    TaxonomyNode* findChild(const std::string& name) const;
    TaxonomyNode* ensureOtherNode();

    std::string   getFullPath() const;
    bool          isLeaf()      const { return children_.empty(); }

    const std::string&              getName()     const { return name_; }
    TaxonomyNode*                   getParent()   const { return parent_; }
    const std::vector<TaxonomyNode*>& getChildren() const { return children_; }

    bool removeChild(const std::string& name);
};
