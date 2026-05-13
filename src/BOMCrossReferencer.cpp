
#include "BOMCrossReferencer.h"

BOMCrossReferencer::BOMCrossReferencer(Inventory& inv) : inventory_(inv) {}

Component* BOMCrossReferencer::resolveEntry(const BOMEntry& e) const {
    // First try exact name match
    auto all = inventory_.getAll();
    for (auto* c : all)
        if (matcher_.match(e.partNumber, c->getName()))
            return c;
    return nullptr;
}

CrossReferenceResult BOMCrossReferencer::compare(const BOM& bom) const {
    CrossReferenceResult result;
    result.projectName = bom.getProjectName();

    for (const auto& entry : bom.getEntries()) {
        Component* comp = resolveEntry(entry);
        if (!comp) {
            result.missing.push_back(entry);
        } else {
            int deficit = entry.quantity - comp->getQuantity();
            MatchedPart mp{ entry, comp, (deficit > 0 ? deficit : 0) };
            if (deficit <= 0) result.matched.push_back(mp);
            else              result.shortfall.push_back(mp);
        }
    }
    return result;
}

std::vector<BOMEntry> BOMCrossReferencer::getMissing(const BOM& bom) const {
    return compare(bom).missing;
}

std::vector<MatchedPart> BOMCrossReferencer::getShortfall(const BOM& bom) const {
    return compare(bom).shortfall;
}
