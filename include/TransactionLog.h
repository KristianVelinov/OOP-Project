
#pragma once
#include "Transaction.h"
#include <vector>
#include <string>
#include <algorithm>

class TransactionLog {
    std::vector<Transaction> log_;
    int nextTxId_;

public:
    TransactionLog() : nextTxId_(1) {}

    std::string record(const std::string& componentId,
                       const std::string& projectName,
                       int qty, TransactionType type);  // returns txId

    const std::vector<Transaction>& getAll()       const { return log_; }
    std::vector<Transaction> getByProject(const std::string& proj) const;
    std::vector<Transaction> getByComponent(const std::string& id) const;

    bool exportCSV(const std::string& filepath) const;
    void clear() { log_.clear(); }

    // For persistence
    void addRaw(const Transaction& t) { log_.push_back(t); }
    void setNextId(int v)             { nextTxId_ = v; }
    int  getNextId()  const           { return nextTxId_; }
};
