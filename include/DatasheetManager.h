
#pragma once
#include "Inventory.h"
#include <string>

/* Manages datasheet paths/URLs stored directly on Component objects.
   Opening logic is platform-aware (xdg-open / start). */
class DatasheetManager {
    Inventory& inventory_;

public:
    explicit DatasheetManager(Inventory& inv) : inventory_(inv) {}

    void        setDatasheet(const std::string& id,
                             const std::string& pathOrUrl);
    std::string getDatasheet(const std::string& id) const;
    bool        openDatasheet(const std::string& id) const;
    bool        hasDatasheet(const std::string& id)  const;

private:
    static bool isURL(const std::string& s);
    static bool launch(const std::string& target);
};
