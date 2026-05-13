
#pragma once
#include "TaxonomyNode.h"
#include <string>
#include <vector>

/* Manages the full category tree.
   All paths are slash-separated strings: "Passives/Resistors/SMD". */
class Taxonomy {
    TaxonomyNode* root_;

public:
    Taxonomy();
    ~Taxonomy();

    // Navigate to node; returns nullptr if not found
    TaxonomyNode* navigate(const std::string& path) const;
    // Create category (parent must exist); returns new node
    TaxonomyNode* createCategory(const std::string& parentPath,
                                 const std::string& name);
    // Delete category leaf; returns false if not found or has children
    bool          deleteCategory(const std::string& path);
    // List direct children paths of a given path
    std::vector<std::string> listChildren(const std::string& path) const;
    // Return all paths in the tree (DFS)
    std::vector<std::string> allPaths()   const;

    TaxonomyNode* getRoot() const { return root_; }

private:
    void initDefaultTree();
    std::vector<std::string> splitPath(const std::string& path) const;
    void collectPaths(TaxonomyNode* node,
                      const std::string& current,
                      std::vector<std::string>& out) const;
};
