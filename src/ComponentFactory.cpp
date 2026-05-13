
#include "ComponentFactory.h"
#include <sstream>

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

Component* ComponentFactory::fromLine(const std::string& line) {
    if (line.empty() || line[0] == '#') return nullptr;
    auto f = split(line, '|');
    if (f.size() < 11) return nullptr;

    // Common base fields:
    // [0]TYPE [1]id [2]name [3]qty [4]min [5]price [6]cat [7]ds [8]fp [9]pkg [10]pins
    std::string type = f[0];
    std::string id   = f[1];
    std::string name = f[2];
    int    qty  = toI(f[3]);
    int    mns  = toI(f[4]);
    double prc  = toD(f[5]);
    std::string cat = f[6];
    std::string ds  = f[7];
    std::string fp  = f[8];
    std::string pkg = f[9];
    int    pins = toI(f[10]);

    // Type-specific fields start at index 11
    if (type == "RESISTOR" && f.size() >= 15) {
        double res = toD(f[11]);
        double tol = toD(f[12]);
        double pwr = toD(f[13]);
        std::string pack = f[14];
        return new Resistor(id, name, qty, res, tol, pwr, pack,
                            mns, prc, cat, ds, fp, pins);
    }
    if (type == "CAPACITOR" && f.size() >= 16) {
        double cap   = toD(f[11]);
        std::string ct = f[12];
        double tol   = toD(f[13]);
        double vr    = toD(f[14]);
        std::string pack = f[15];
        return new Capacitor(id, name, qty, cap, ct, tol, vr, pack,
                             mns, prc, cat, ds, fp, pins);
    }
    if (type == "INDUCTOR" && f.size() >= 15) {
        double ind  = toD(f[11]);
        double irat = toD(f[12]);
        double vr   = toD(f[13]);
        std::string pack = f[14];
        return new Inductor(id, name, qty, ind, irat, vr, pack,
                            mns, prc, cat, ds, fp, pins);
    }
    if (type == "MOSFET" && f.size() >= 19) {
        std::string tt = f[11];
        double vgs = toD(f[12]);
        double rds = toD(f[13]);
        double qg  = toD(f[14]);
        double mv  = toD(f[15]);
        double mc  = toD(f[16]);
        std::string pack = f[17];
        // f[18] could be extra
        return new MOSFETTransistor(id, name, qty, tt, vgs, rds, qg,
                                    mv, mc, pack, mns, prc, cat, ds, fp, pkg, pins);
    }
    if (type == "BJT" && f.size() >= 17) {
        std::string tt = f[11];
        double hfe  = toD(f[12]);
        double vceo = toD(f[13]);
        double icm  = toD(f[14]);
        std::string pack = f[15];
        return new BJTTransistor(id, name, qty, tt, hfe, vceo, icm,
                                  pack, mns, prc, cat, ds, fp, pkg, pins);
    }
    if (type == "DIODE" && f.size() >= 17) {
        double vf  = toD(f[11]);
        double vrm = toD(f[12]);
        std::string dt = f[13];
        double ic  = toD(f[14]);
        std::string pack = f[15];
        return new DiodeComponent(id, name, qty, vf, vrm, dt, ic,
                                   pack, mns, prc, cat, ds, fp, pkg, pins);
    }
    if (type == "IC" && f.size() >= 17) {
        std::string func = f[11];
        double sv  = toD(f[12]);
        double mv  = toD(f[13]);
        double mc  = toD(f[14]);
        std::string pack = f[15];
        return new IntegratedCircuit(id, name, qty, func, sv, mv, mc,
                                      pack, pins, mns, prc, cat, ds, fp, pkg);
    }
    if (type == "CONNECTOR" && f.size() >= 14) {
        std::string ct = f[11];
        double pitch   = toD(f[12]);
        std::string gen= f[13];
        return new ConnectorComponent(id, name, qty, ct, pitch, gen,
                                       pins, mns, prc, cat, ds, fp, pkg);
    }
    if (type == "ARBITRARY") {
        auto* a = new ArbitraryComponent(id, name, qty,
                                          f.size() > 11 ? f[11] : "",
                                          mns, prc, cat, ds, fp, pkg, pins);
        for (int i = 12; i < (int)f.size(); i++) {
            auto eq = f[i].find('=');
            if (eq != std::string::npos)
                a->addParam(f[i].substr(0,eq), f[i].substr(eq+1));
        }
        return a;
    }
    return nullptr;
}
