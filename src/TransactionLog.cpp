
#include "TransactionLog.h"
#include <fstream>
#include <sstream>

std::string TransactionLog::record(const std::string& componentId,
                                   const std::string& projectName,
                                   int qty, TransactionType type) {
    std::ostringstream id;
    id << "TX" << std::setw(6) << std::setfill('0') << nextTxId_++;
    log_.emplace_back(id.str(), componentId, projectName, qty, type);
    return id.str();
}

std::vector<Transaction> TransactionLog::getByProject(const std::string& proj) const {
    std::vector<Transaction> out;
    for (auto& t : log_)
        if (t.getProjectName() == proj) out.push_back(t);
    return out;
}

std::vector<Transaction> TransactionLog::getByComponent(const std::string& id) const {
    std::vector<Transaction> out;
    for (auto& t : log_)
        if (t.getComponentId() == id) out.push_back(t);
    return out;
}

bool TransactionLog::exportCSV(const std::string& filepath) const {
    std::ofstream f(filepath);
    if (!f.is_open()) return false;
    f << "TxID,ComponentID,Project,Qty,Timestamp,Type\n";
    for (auto& t : log_)
        f << t.getTxId() << ","
          << t.getComponentId() << ","
          << t.getProjectName() << ","
          << t.getQuantity()    << ","
          << t.timestampStr()   << ","
          << txTypeToStr(t.getType()) << "\n";
    return true;
}
