
#pragma once
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>

/* Abstract base for all electronic components.
   Pure-virtual display/getTypeParams/getTypeName/clone force every
   concrete type to provide its own implementation. */
class Component {
protected:
    std::string id_;
    std::string name_;
    int         quantity_;
    int         minStock_;
    double      unitPrice_;
    std::string categoryPath_;
    std::string datasheetPath_;
    std::string footprint_;     // raw KiCad footprint string
    std::string packageType_;   // parsed: "0603", "SOIC", "THT", …
    int         pinCount_;

public:
    Component(const std::string& id, const std::string& name,
              int qty, int minStock = 1, double unitPrice = 0.0,
              const std::string& catPath = "Uncategorized",
              const std::string& dsPath  = "",
              const std::string& footprint = "",
              const std::string& packageType = "",
              int pinCount = 0);
    virtual ~Component() = default;

    // ── Getters ─────────────────────────────────────────────
    const std::string& getId()           const { return id_; }
    const std::string& getName()         const { return name_; }
    int                getQuantity()     const { return quantity_; }
    int                getMinStock()     const { return minStock_; }
    double             getUnitPrice()    const { return unitPrice_; }
    const std::string& getCategoryPath() const { return categoryPath_; }
    const std::string& getDatasheetPath()const { return datasheetPath_; }
    const std::string& getFootprint()    const { return footprint_; }
    const std::string& getPackageType()  const { return packageType_; }
    int                getPinCount()     const { return pinCount_; }

    // ── Setters ─────────────────────────────────────────────
    void setName(const std::string& v)         { name_ = v; }
    void setQuantity(int v)                    { quantity_ = v; }
    void setMinStock(int v)                    { minStock_ = v; }
    void setUnitPrice(double v)                { unitPrice_ = v; }
    void setCategoryPath(const std::string& v) { categoryPath_ = v; }
    void setDatasheetPath(const std::string& v){ datasheetPath_ = v; }
    void setFootprint(const std::string& v)    { footprint_ = v; }
    void setPackageType(const std::string& v)  { packageType_ = v; }
    void setPinCount(int v)                    { pinCount_ = v; }

    // ── Polymorphic interface ────────────────────────────────
    virtual void display() const = 0;
    virtual std::map<std::string,std::string> getTypeParams() const = 0;
    virtual std::string getTypeName() const = 0;
    virtual Component*  clone()       const = 0;

    // Pipe-delimited serialization: TYPE|id|name|qty|min|price|cat|ds|fp|pkg|pins|<type fields>
    virtual std::string serialize()   const;
    void displayBaseInfo()            const;
    bool isLowStock()                 const { return quantity_ <= minStock_; }

protected:
    // Shared base fields for serialization
    std::string baseSerial() const;
};
