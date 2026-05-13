
#pragma once
#include "CrossReferenceResult.h"
#include "BOM.h"
#include "Inventory.h"
#include "SmartMatcher.h"

class BOMCrossReferencer {
    Inventory&   inventory_;
    SmartMatcher matcher_;

public:
    explicit BOMCrossReferencer(Inventory& inv);

    CrossReferenceResult compare(const BOM& bom) const;

    // Convenience wrappers
    std::vector<BOMEntry>  getMissing(const BOM& bom)  const;
    std::vector<MatchedPart> getShortfall(const BOM& bom) const;

private:
    Component* resolveEntry(const BOMEntry& e) const;
};
