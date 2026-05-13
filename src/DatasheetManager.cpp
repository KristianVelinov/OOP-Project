
#include "DatasheetManager.h"
#include <cstdlib>
#include <algorithm>

void DatasheetManager::setDatasheet(const std::string& id,
                                     const std::string& pathOrUrl) {
    Component* c = inventory_.getComponent(id);
    if (c) c->setDatasheetPath(pathOrUrl);
}

std::string DatasheetManager::getDatasheet(const std::string& id) const {
    Component* c = inventory_.getComponent(id);
    return c ? c->getDatasheetPath() : "";
}

bool DatasheetManager::hasDatasheet(const std::string& id) const {
    Component* c = inventory_.getComponent(id);
    return c && !c->getDatasheetPath().empty()
             && c->getDatasheetPath() != "~";
}

bool DatasheetManager::isURL(const std::string& s) {
    return s.find("http://") == 0 || s.find("https://") == 0;
}

bool DatasheetManager::launch(const std::string& target) {
#ifdef _WIN32
    std::string cmd = "start \"\" \"" + target + "\"";
#elif __APPLE__
    std::string cmd = "open \"" + target + "\"";
#else
    std::string cmd = "xdg-open \"" + target + "\" &";
#endif
    return std::system(cmd.c_str()) == 0;
}

bool DatasheetManager::openDatasheet(const std::string& id) const {
    std::string path = getDatasheet(id);
    if (path.empty() || path == "~") {
        std::cout << "  No datasheet registered for " << id << "\n";
        return false;
    }
    std::cout << "  Opening: " << path << "\n";
    return DatasheetManager::launch(path);
}
