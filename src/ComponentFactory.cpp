#include "ComponentFactory.h"
#include <sstream>
#include <algorithm>

std::vector<std::string> ComponentFactory::split(const std::string& s, char d) {
    std::vector<std::string> out;
    std::istringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, d)) out.push_back(tok);
    return out;
}

double ComponentFactory::toD(const std::string& s) {
    try { return std::stod(s); } catch(...) { return 0.0; }
}

int ComponentFactory::toI(const std::string& s) {
    try { return std::stoi(s); } catch(...) { return 0; }
}

// Fill missing/blank fields with safe defaults and robustly trim whitespace
static void normalizeFields(std::vector<std::string>& fields, int minSize) {
    // Ensure minimum size with empty strings
    if (fields.size() < (size_t)minSize) {
        fields.resize(minSize, "");
    }
    // Replace blank fields with sensible defaults
    for (auto& f : fields) {
        if (f.empty()) continue;
        
        size_t start = f.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            f = ""; // String is all whitespace
        } else {
            f.erase(0, start);
            f.erase(f.find_last_not_of(" \t\r\n") + 1);
        }
    }
}

Component* ComponentFactory::fromLine(const std::string& line) {
    if (line.empty() || line[0] == '#') return nullptr;
    
    auto f = split(line, '|');
    
    // CRITICAL: Need minimum 11 base fields
    if (f.size() < 11) return nullptr;
    
    // Normalize all fields: trim whitespace, resize to match expected size
    normalizeFields(f, f.size());

    // Validate critical base fields first
    std::string type = f[0];
    std::string id   = f[1];
    std::string name = f[2];
    
    if (type.empty() || id.empty() || name.empty()) return nullptr;

    // Parse base fields
    int    qty  = toI(f[3]);
    int    mns  = toI(f[4]);
    double prc  = toD(f[5]);
    
    // Apply defaults to missing base string fields
    std::string cat = f[6].empty() ? "Uncategorized" : f[6];
    std::string ds  = f[7].empty() ? "https://www.youtube.com/watch?v=dQw4w9WgXcQ" : f[7];
    std::string fp  = f[8].empty() ? "Unknown" : f[8];
    std::string pkg = f[9].empty() ? "Unknown" : f[9];
    int    pins = toI(f[10]);

    // Type-specific parsing with normalization and validation
    if (type == "RESISTOR" && f.size() >= 15) {
        normalizeFields(f, 15);
        double res = toD(f[11]);
        double tol = toD(f[12]);
        double pwr = toD(f[13]);
        std::string pack = f[14].empty() ? pkg : f[14];
        
        if (res < 0.0 || tol < 0.0 || pwr < 0.0) return nullptr;
        return new Resistor(id, name, qty, res, tol, pwr, pack,
                            mns, prc, cat, ds, fp, pins);
    }
    
    if (type == "CAPACITOR" && f.size() >= 16) {
        normalizeFields(f, 16);
        double cap   = toD(f[11]);
        std::string ct = f[12].empty() ? "ceramic" : f[12];
        double tol   = toD(f[13]);
        double vr    = toD(f[14]);
        std::string pack = f[15].empty() ? pkg : f[15];
        
        if (cap < 0.0 || tol < 0.0 || vr < 0.0) return nullptr;
        return new Capacitor(id, name, qty, cap, ct, tol, vr, pack,
                             mns, prc, cat, ds, fp, pins);
    }
    
    if (type == "INDUCTOR" && f.size() >= 15) {
        normalizeFields(f, 15);
        double ind  = toD(f[11]);
        double irat = toD(f[12]);
        double vr   = toD(f[13]);
        std::string pack = f[14].empty() ? pkg : f[14];
        
        if (ind < 0.0 || irat < 0.0 || vr < 0.0) return nullptr;
        return new Inductor(id, name, qty, ind, irat, vr, pack,
                            mns, prc, cat, ds, fp, pins);
    }
    
    if (type == "MOSFET" && f.size() >= 18) {
        normalizeFields(f, 18);
        std::string tt = f[11].empty() ? "N-CH" : f[11];
        double vgs = toD(f[12]);
        double rds = toD(f[13]);
        double qg  = toD(f[14]);
        double mv  = toD(f[15]);
        double mc  = toD(f[16]);
        std::string pack = f[17].empty() ? pkg : f[17];
        
        // Removed validation for negative voltages to allow P-CH MOSFETs if required later
        if (rds < 0.0 || qg < 0.0 || mc < 0.0) return nullptr;
        return new MOSFETTransistor(id, name, qty, tt, vgs, rds, qg,
                                    mv, mc, pack, mns, prc, cat, ds, fp, pkg, pins);
    }
    
    if (type == "BJT" && f.size() >= 16) {
        normalizeFields(f, 16);
        std::string tt = f[11].empty() ? "NPN" : f[11];
        double hfe  = toD(f[12]);
        double vceo = toD(f[13]);
        double icm  = toD(f[14]);
        std::string pack = f[15].empty() ? pkg : f[15];
        
        // Removed vceo < 0.0 check to allow negative breakdown voltages (PNP)
        if (hfe < 0.0 || icm < 0.0) return nullptr;
        return new BJTTransistor(id, name, qty, tt, hfe, vceo, icm,
                                  pack, mns, prc, cat, ds, fp, pkg, pins);
    }
    
    if (type == "DIODE" && f.size() >= 16) {
        normalizeFields(f, 16);
        double vf  = toD(f[11]);
        double vrm = toD(f[12]);
        std::string dt = f[13].empty() ? "rectifier" : f[13];
        double ic  = toD(f[14]);
        std::string pack = f[15].empty() ? pkg : f[15];
        
        if (vf < 0.0 || vrm < 0.0 || ic < 0.0) return nullptr;
        return new DiodeComponent(id, name, qty, vf, vrm, dt, ic,
                                   pack, mns, prc, cat, ds, fp, pkg, pins);
    }
    
    if (type == "IC" && f.size() >= 16) {
        normalizeFields(f, 16);
        std::string func = f[11].empty() ? "General IC" : f[11];
        double sv  = toD(f[12]);
        double mv  = toD(f[13]);
        double mc  = toD(f[14]);
        std::string pack = f[15].empty() ? pkg : f[15];

        if (mc < 0.0) return nullptr; 
        return new IntegratedCircuit(id, name, qty, func, sv, mv, mc,
                                      pack, pins, mns, prc, cat, ds, fp, pkg);
    }
    
    if (type == "CONNECTOR" && f.size() >= 14) {
        normalizeFields(f, 14);
        std::string ct = f[11].empty() ? "Generic" : f[11];
        double pitch   = toD(f[12]);
        std::string gen= f[13].empty() ? "N/A" : f[13];
        
        // FIX: Changed from pitch <= 0.0 to pitch < 0.0 to allow 0.0 placeholders
        if (pitch < 0.0) return nullptr; 
        return new ConnectorComponent(id, name, qty, ct, pitch, gen,
                                       pins, mns, prc, cat, ds, fp, pkg);
    }
    
    if (type == "ARBITRARY") {
        auto* a = new ArbitraryComponent(id, name, qty,
                                          f.size() > 11 && !f[11].empty() ? f[11] : "Miscellaneous",
                                          mns, prc, cat, ds, fp, pkg, pins);
        for (int i = 12; i < (int)f.size(); i++) {
            if (f[i].empty()) continue;
            auto eq = f[i].find('=');
            if (eq != std::string::npos) {
                std::string key = f[i].substr(0, eq);
                std::string val = f[i].substr(eq + 1);
                
                // Safe string trims
                auto k_start = key.find_first_not_of(" \t\r\n");
                if (k_start != std::string::npos) {
                    key.erase(0, k_start);
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                } else key = "";
                
                auto v_start = val.find_first_not_of(" \t\r\n");
                if (v_start != std::string::npos) {
                    val.erase(0, v_start);
                    val.erase(val.find_last_not_of(" \t\r\n") + 1);
                } else val = "";
                
                if (!key.empty() && !val.empty()) {
                    a->addParam(key, val);
                }
            }
        }
        return a;
    }
    
    return nullptr;
}