
#include "Transaction.h"
#include <sstream>

std::string txTypeToStr(TransactionType t) {
    switch (t) {
        case TransactionType::CHECKOUT: return "CHECKOUT";
        case TransactionType::RETURN:   return "RETURN";
        case TransactionType::RESTOCK:  return "RESTOCK";
    }
    return "CHECKOUT";
}

TransactionType txTypeFromStr(const std::string& s) {
    if (s == "RETURN")  return TransactionType::RETURN;
    if (s == "RESTOCK") return TransactionType::RESTOCK;
    return TransactionType::CHECKOUT;
}

Transaction::Transaction(const std::string& txId,
                         const std::string& componentId,
                         const std::string& projectName,
                         int qty, TransactionType type, std::time_t ts)
    : txId_(txId), componentId_(componentId), projectName_(projectName),
      quantity_(qty), timestamp_(ts ? ts : std::time(nullptr)), type_(type)
{}

std::string Transaction::timestampStr() const {
    char buf[32];
    std::tm* tm = std::localtime(&timestamp_);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return buf;
}

std::string Transaction::toString() const {
    return txId_ + " | " + txTypeToStr(type_) + " | "
         + componentId_ + " | qty=" + std::to_string(quantity_)
         + " | proj=" + projectName_ + " | " + timestampStr();
}

std::string Transaction::serialize() const {
    return txId_ + "|" + componentId_ + "|" + projectName_
         + "|" + std::to_string(quantity_)
         + "|" + std::to_string(static_cast<long long>(timestamp_))
         + "|" + txTypeToStr(type_);
}

Transaction Transaction::deserialize(const std::string& line) {
    std::istringstream ss(line);
    std::string txId, compId, proj, qtyStr, tsStr, typeStr;
    std::getline(ss, txId,   '|');
    std::getline(ss, compId, '|');
    std::getline(ss, proj,   '|');
    std::getline(ss, qtyStr, '|');
    std::getline(ss, tsStr,  '|');
    std::getline(ss, typeStr,'|');
    int qty = std::stoi(qtyStr);
    std::time_t ts = static_cast<std::time_t>(std::stoll(tsStr));
    return Transaction(txId, compId, proj, qty, txTypeFromStr(typeStr), ts);
}
