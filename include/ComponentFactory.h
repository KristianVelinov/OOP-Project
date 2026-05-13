
#pragma once
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "MOSFETTransistor.h"
#include "BJTTransistor.h"
#include "DiodeComponent.h"
#include "IntegratedCircuit.h"
#include "ConnectorComponent.h"
#include "ArbitraryComponent.h"
#include <string>
#include <vector>
#include <stdexcept>

/* Deserializes a pipe-separated line from inventory.dat
   back into the correct concrete Component subclass. */
class ComponentFactory {
public:
    static Component* fromLine(const std::string& line);

private:
    static std::vector<std::string> split(const std::string& s, char delim);
    static double    toD(const std::string& s);
    static int       toI(const std::string& s);
};
