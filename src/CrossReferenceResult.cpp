
#include "CrossReferenceResult.h"

double CrossReferenceResult::totalCost() const {
    double total = 0.0;
    for (auto& m : matched)
        if (m.component)
            total += m.component->getUnitPrice() * m.bomEntry.quantity;
    for (auto& m : shortfall)
        if (m.component)
            total += m.component->getUnitPrice() * m.component->getQuantity();
    return total;
}

int CrossReferenceResult::totalDeficit() const {
    int d = 0;
    for (auto& m : shortfall) d += m.deficit;
    return d;
}
