
#pragma once
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

enum class TransactionType { CHECKOUT, RETURN, RESTOCK };

std::string txTypeToStr(TransactionType t);
TransactionType txTypeFromStr(const std::string& s);

class Transaction {
    std::string     txId_;
    std::string     componentId_;
    std::string     projectName_;
    int             quantity_;
    std::time_t     timestamp_;
    TransactionType type_;

public:
    Transaction(const std::string& txId,
                const std::string& componentId,
                const std::string& projectName,
                int qty,
                TransactionType type,
                std::time_t ts = 0);

    const std::string& getTxId()         const { return txId_; }
    const std::string& getComponentId()  const { return componentId_; }
    const std::string& getProjectName()  const { return projectName_; }
    int                getQuantity()     const { return quantity_; }
    std::time_t        getTimestamp()    const { return timestamp_; }
    TransactionType    getType()         const { return type_; }

    std::string timestampStr() const;
    std::string toString()     const;
    std::string serialize()    const;
    static Transaction deserialize(const std::string& line);
};
