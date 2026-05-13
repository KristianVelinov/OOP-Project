
/*  ECIMS — Electronic Component Inventory Management System
    Console UI */

#include <windows.h>
#include <conio.h>

#include "Inventory.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "MOSFETTransistor.h"
#include "BJTTransistor.h"
#include "DiodeComponent.h"
#include "IntegratedCircuit.h"
#include "ConnectorComponent.h"
#include "ArbitraryComponent.h"
#include "BOM.h"
#include "BOMImporter.h"
#include "BOMCrossReferencer.h"
#include "CrossReferenceResult.h"
#include "SearchCriteria.h"
#include "ParametricSearch.h"
#include "ProjectManager.h"
#include "LowStockAlert.h"
#include "DatasheetManager.h"
#include "Persistence.h"
#include "FootprintParser.h"

#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>

// ── ANSI colours ──────────────────────────────────────────────
#ifdef _WIN32
#define CLR_RESET "\x1b[0m"
#define CLR_BOLD "\x1b[1m"
#define CLR_CYAN "\x1b[36m"
#define CLR_GREEN "\x1b[32m"
#define CLR_YELLOW "\x1b[33m"
#define CLR_RED "\x1b[31m"
#define CLR_BLUE "\x1b[34m"
#define CLR_MAG "\x1b[35m"
#define CLR_SELECT "\x1b[30;47m"
#else
#define CLR_RESET "\033[0m"
#define CLR_BOLD "\033[1m"
#define CLR_CYAN "\033[36m"
#define CLR_GREEN "\033[32m"
#define CLR_YELLOW "\033[33m"
#define CLR_RED "\033[31m"
#define CLR_BLUE "\033[34m"
#define CLR_MAG "\033[35m"
#define CLR_SELECT "\033[30;47m"
#endif

static const std::string DATA_DIR = "data";

// ── I/O helpers ───────────────────────────────────────────────
static void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void enableANSI()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

static void pause()
{
    std::cout << "\n"
              << CLR_YELLOW
              << "  [Press ENTER]"
              << CLR_RESET;

    while (true)
    {
        int ch = _getch();

        if (ch == 13)
            break;
    }
}

static std::string inputLine(const std::string &prompt)
{
    std::cout << CLR_CYAN << "  " << prompt << CLR_RESET;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static int inputInt(const std::string &prompt, int def = 0)
{
    std::cout << CLR_CYAN << "  " << prompt
              << " [" << def << "]: " << CLR_RESET;
    std::string s;
    std::getline(std::cin, s);
    if (s.empty())
        return def;
    try
    {
        return std::stoi(s);
    }
    catch (...)
    {
        return def;
    }
}

static double inputDouble(const std::string &prompt, double def = 0.0)
{
    std::cout << CLR_CYAN << "  " << prompt
              << " [" << def << "]: " << CLR_RESET;
    std::string s;
    std::getline(std::cin, s);
    if (s.empty())
        return def;
    try
    {
        return std::stod(s);
    }
    catch (...)
    {
        return def;
    }
}

static void banner()
{
    std::cout << CLR_BOLD << CLR_BLUE
              << "\n╔══════════════════════════════════════════════════════╗\n"
                 "║   ECIMS - Electronic Component Inventory Manager     ║\n"
                 "╚══════════════════════════════════════════════════════╝\n"
              << CLR_RESET;
}

static void hdr(const std::string &title)
{
    std::cout << CLR_BOLD << CLR_CYAN
              << "\n  ── " << title << " ──\n"
              << CLR_RESET;
}

static int arrowMenu(
    const std::string &title,
    const std::vector<std::string> &items,
    int indent = 2)
{
    int selected = 0;

    while (true)
    {
        clearScreen();
        banner();
        hdr(title);

        std::cout << "\n";

        for (int i = 0; i < (int)items.size(); ++i)
        {
            std::cout << std::string(indent, ' ');

            if (i == selected)
            {
                std::cout
                    << CLR_SELECT
                    << "  > " << items[i] << "  "
                    << CLR_RESET;
            }
            else
            {
                std::cout
                    << "    " << items[i];
            }

            std::cout << "\n\n";
        }

        int ch = _getch();

        if (ch == 224)
        {
            ch = _getch();

            if (ch == 72)
            {
                selected--;
                if (selected < 0)
                    selected = (int)items.size() - 1;
            }
            else if (ch == 80)
            {
                selected++;
                if (selected >= (int)items.size())
                    selected = 0;
            }
        }
        else if (ch == 13)
        {
            return selected;
        }
    }
}

static void printCompList(const std::vector<Component *> &list)
{
    if (list.empty())
    {
        std::cout << "  (none)\n";
        return;
    }
    std::cout << CLR_BOLD
              << std::left
              << "  " << std::setw(10) << "ID"
              << std::setw(28) << "Name"
              << std::setw(10) << "Type"
              << std::setw(8) << "Qty"
              << std::setw(8) << "Min"
              << std::setw(8) << "Pins"
              << std::setw(10) << "Package"
              << "Price\n"
              << CLR_RESET
              << "  " << std::string(84, '-') << "\n";
    for (auto *c : list)
    {
        std::cout << "  "
                  << std::left
                  << std::setw(10) << c->getId()
                  << std::setw(28) << c->getName().substr(0, 27)
                  << std::setw(10) << c->getTypeName().substr(0, 9)
                  << std::setw(8) << c->getQuantity()
                  << std::setw(8) << c->getMinStock()
                  << std::setw(8) << c->getPinCount()
                  << std::setw(10) << c->getPackageType().substr(0, 9)
                  << "€" << std::fixed << std::setprecision(4)
                  << c->getUnitPrice()
                  << (c->isLowStock() ? CLR_RED " [LOW]" CLR_RESET : "")
                  << "\n";
    }
    std::cout << "  " << list.size() << " component(s)\n";
}

// ══════════════════════════════════════════════════════════════
// GENERATE ID HELPER
// ══════════════════════════════════════════════════════════════
static std::string genId(Inventory &inv, const std::string &prefix)
{
    static int ctr = 1;
    std::ostringstream oss;
    do
    {
        oss.str("");
        oss << prefix << std::setw(4) << std::setfill('0') << ctr++;
    } while (inv.getComponent(oss.str()));
    return oss.str();
}

// ══════════════════════════════════════════════════════════════
// MODULE 1 – INVENTORY CRUD
// ══════════════════════════════════════════════════════════════
static void moduleAddComponent(Inventory &inv)
{
    hdr("Add Component");
    std::cout << "  Types: 1=Resistor  2=Capacitor  3=Inductor\n"
                 "         4=MOSFET    5=BJT        6=Diode\n"
                 "         7=IC        8=Connector  9=Arbitrary\n";
    int t = inputInt("Type number", 1);

    std::string name = inputLine("Name (e.g. 10k, LM2596T-ADJ): ");
    int qty = inputInt("Quantity", 10);
    int mns = inputInt("Min stock threshold", 2);
    double prc = inputDouble("Unit price (€)", 0.0);
    std::string cat;

    // footprint input
    std::string fp = inputLine("Footprint (optional, KiCad format or leave blank): ");
    std::string pkg = FootprintParser::extractPackageType(fp);
    int pc = FootprintParser::extractPinCount(fp);
    if (pkg.empty())
        pkg = inputLine("Package type (e.g. 0603, SOIC, THT): ");
    if (pc == 0)
        pc = inputInt("Pin count", 2);

    std::string ds = inputLine("Datasheet path/URL (leave blank to skip): ");

    Component *c = nullptr;

    if (t == 1)
    { // RESISTOR
        cat = inputLine("Category [Passives/Resistors/SMD]: ");
        if (cat.empty())
            cat = "Passives/Resistors/SMD";
        double res = inputDouble("Resistance (Ohm)", 1000.0);
        double tol = inputDouble("Tolerance (%)", 1.0);
        double pwr = inputDouble("Power rating (W)", 0.1);
        std::string id = genId(inv, "RES");
        c = new Resistor(id, name, qty, res, tol, pwr, pkg,
                         mns, prc, cat, ds, fp, pc);
    }
    else if (t == 2)
    { // CAPACITOR
        cat = inputLine("Category [Passives/Capacitors/Ceramic]: ");
        if (cat.empty())
            cat = "Passives/Capacitors/Ceramic";
        double cap = inputDouble("Capacitance (F, e.g. 100e-9 for 100nF)", 100e-9);
        std::string ct = inputLine("Type (ceramic/electrolytic/tantalum/film) [ceramic]: ");
        if (ct.empty())
            ct = "ceramic";
        double tol = inputDouble("Tolerance (%)", 10.0);
        double vr = inputDouble("Voltage rating (V)", 10.0);
        std::string id = genId(inv, "CAP");
        c = new Capacitor(id, name, qty, cap, ct, tol, vr, pkg,
                          mns, prc, cat, ds, fp, pc);
    }
    else if (t == 3)
    { // INDUCTOR
        cat = inputLine("Category [Passives/Inductors/THT]: ");
        if (cat.empty())
            cat = "Passives/Inductors/THT";
        double ind = inputDouble("Inductance (H, e.g. 33e-6 for 33uH)", 33e-6);
        double irat = inputDouble("Current rating (A)", 1.0);
        std::string id = genId(inv, "IND");
        c = new Inductor(id, name, qty, ind, irat, 0.0, pkg,
                         mns, prc, cat, ds, fp, pc);
    }
    else if (t == 4)
    { // MOSFET
        cat = inputLine("Category [Semiconductors/Transistors/MOSFET/N-Channel]: ");
        if (cat.empty())
            cat = "Semiconductors/Transistors/MOSFET/N-Channel";
        std::string tt = inputLine("Type (N-CH / P-CH) [N-CH]: ");
        if (tt.empty())
            tt = "N-CH";
        double vgs = inputDouble("Vgs threshold (V)", 2.0);
        double rds = inputDouble("Rds(on) (Ohm)", 0.01);
        double qg = inputDouble("Gate charge (nC)", 10.0);
        double mv = inputDouble("Max Vds (V)", 30.0);
        double mc = inputDouble("Max Id (A)", 5.0);
        std::string id = genId(inv, "MOS");
        c = new MOSFETTransistor(id, name, qty, tt, vgs, rds, qg,
                                 mv, mc, pkg, mns, prc, cat, ds, fp, pkg, pc);
    }
    else if (t == 5)
    { // BJT
        cat = inputLine("Category [Semiconductors/Transistors/BJT/NPN]: ");
        if (cat.empty())
            cat = "Semiconductors/Transistors/BJT/NPN";
        std::string tt = inputLine("Type (NPN / PNP) [NPN]: ");
        if (tt.empty())
            tt = "NPN";
        double hfe = inputDouble("hFE (gain)", 100.0);
        double vceo = inputDouble("Vceo (V)", 40.0);
        double icm = inputDouble("Ic max (A)", 0.1);
        std::string id = genId(inv, "BJT");
        c = new BJTTransistor(id, name, qty, tt, hfe, vceo, icm,
                              pkg, mns, prc, cat, ds, fp, pkg, pc);
    }
    else if (t == 6)
    { // DIODE
        cat = inputLine("Category [Semiconductors/Diodes/Rectifier]: ");
        if (cat.empty())
            cat = "Semiconductors/Diodes/Rectifier";
        double vf = inputDouble("Forward voltage Vf (V)", 0.7);
        double vrm = inputDouble("Reverse voltage VRM (V)", 40.0);
        std::string dt = inputLine("Type (rectifier/schottky/zener/TVS/LED) [rectifier]: ");
        if (dt.empty())
            dt = "rectifier";
        double imax = inputDouble("If max (A)", 1.0);
        std::string id = genId(inv, "DIO");
        c = new DiodeComponent(id, name, qty, vf, vrm, dt, imax,
                               pkg, mns, prc, cat, ds, fp, pkg, pc);
    }
    else if (t == 7)
    { // IC
        cat = inputLine("Category [Semiconductors/ICs/Power/DCDC]: ");
        if (cat.empty())
            cat = "Semiconductors/ICs/Power/DCDC";
        std::string func = inputLine("Function description: ");
        double sv = inputDouble("Supply voltage (V)", 3.3);
        double mv = inputDouble("Max voltage (V)", 40.0);
        double mc = inputDouble("Max current (A)", 0.01);
        std::string id = genId(inv, "IC_");
        c = new IntegratedCircuit(id, name, qty, func, sv, mv, mc,
                                  pkg, pc, mns, prc, cat, ds, fp, pkg);
    }
    else if (t == 8)
    { // CONNECTOR
        cat = inputLine("Category [Connectors/PinHeaders]: ");
        if (cat.empty())
            cat = "Connectors/PinHeaders";
        std::string ct = inputLine("Connector type (PinHeader/JST/Molex/USB-C/SMA): ");
        double pitch = inputDouble("Pitch (mm)", 2.54);
        std::string gen = inputLine("Gender (male/female) [male]: ");
        if (gen.empty())
            gen = "male";
        std::string id = genId(inv, "CON");
        c = new ConnectorComponent(id, name, qty, ct, pitch, gen,
                                   pc, mns, prc, cat, ds, fp, pkg);
    }
    else
    { // ARBITRARY
        cat = inputLine("Category [Other]: ");
        if (cat.empty())
            cat = "Other";
        std::string desc = inputLine("Description: ");
        std::string id = genId(inv, "ARB");
        auto *arb = new ArbitraryComponent(id, name, qty, desc,
                                           mns, prc, cat, ds, fp, pkg, pc);
        std::cout << "  Add custom parameters? (key=value, blank to stop)\n";
        while (true)
        {
            std::string kv = inputLine("  param (key=value): ");
            if (kv.empty())
                break;
            auto eq = kv.find('=');
            if (eq != std::string::npos)
                arb->addParam(kv.substr(0, eq), kv.substr(eq + 1));
        }
        c = arb;
    }

    if (c)
    {
        inv.addComponent(c);
        std::cout << CLR_GREEN << "\n  ✓ Added: " << c->getId()
                  << " — " << c->getName() << CLR_RESET << "\n";
    }
}

static void moduleEditComponent(Inventory &inv)
{
    hdr("Edit Component");
    std::string id = inputLine("Component ID to edit: ");
    Component *c = inv.getComponent(id);
    if (!c)
    {
        std::cout << CLR_RED "  Not found.\n" CLR_RESET;
        return;
    }
    c->display();
    std::cout << "\n  1=Name  2=Quantity  3=MinStock  4=UnitPrice  "
                 "5=Category  6=Datasheet  7=Package  8=PinCount\n";
    int ch = inputInt("Field to edit", 0);
    if (ch == 1)
        c->setName(inputLine("New name: "));
    else if (ch == 2)
        c->setQuantity(inputInt("New quantity"));
    else if (ch == 3)
        c->setMinStock(inputInt("New min stock"));
    else if (ch == 4)
        c->setUnitPrice(inputDouble("New unit price (€)"));
    else if (ch == 5)
        c->setCategoryPath(inputLine("New category path: "));
    else if (ch == 6)
        c->setDatasheetPath(inputLine("New datasheet path/URL: "));
    else if (ch == 7)
        c->setPackageType(inputLine("New package type: "));
    else if (ch == 8)
        c->setPinCount(inputInt("New pin count"));
    else
    {
        std::cout << "  Cancelled.\n";
        return;
    }
    std::cout << CLR_GREEN "  ✓ Updated.\n" CLR_RESET;
}

static void moduleDeleteComponent(Inventory &inv)
{
    hdr("Delete Component");
    std::string id = inputLine("Component ID to delete: ");
    Component *c = inv.getComponent(id);
    if (!c)
    {
        std::cout << CLR_RED "  Not found.\n" CLR_RESET;
        return;
    }
    std::cout << "  Delete: " << c->getName() << " ? (y/N): ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans == "y" || ans == "Y")
    {
        inv.removeComponent(id);
        std::cout << CLR_GREEN "  ✓ Deleted.\n" CLR_RESET;
    }
    else
        std::cout << "  Cancelled.\n";
}

static void menuInventory(Inventory &inv)
{
    while (true)
    {
        hdr("Inventory Management");
        std::vector<std::string> items = {
            "List all components",
            "View component detail",
            "Add component",
            "Edit component",
            "Delete component",
            "List by category",
            "Quick name search",
            "List by package type",
            "List by pin count",
            "Back"};

        int ch = arrowMenu("Inventory Management", items, 6);

        if (ch == 9)
            return;

        ch += 1;

        if (ch == 1)
        {
            hdr("All Components");
            printCompList(inv.getAll());
            pause();
        }
        else if (ch == 2)
        {
            std::string id = inputLine("Component ID: ");
            Component *c = inv.getComponent(id);
            if (c)
                c->display();
            else
                std::cout << CLR_RED "  Not found.\n" CLR_RESET;
            pause();
        }
        else if (ch == 3)
        {
            moduleAddComponent(inv);
            pause();
        }
        else if (ch == 4)
        {
            moduleEditComponent(inv);
            pause();
        }
        else if (ch == 5)
        {
            moduleDeleteComponent(inv);
            pause();
        }
        else if (ch == 6)
        {
            std::string path = inputLine("Category path (e.g. Passives/Resistors): ");
            printCompList(inv.getByCategory(path));
            pause();
        }
        else if (ch == 7)
        {
            std::string q = inputLine("Search query: ");
            printCompList(inv.searchByName(q));
            pause();
        }
        else if (ch == 8)
        {
            std::string pkg = inputLine("Package type (e.g. 0603, SOIC, THT, QFN): ");
            printCompList(inv.getByPackageType(pkg));
            pause();
        }
        else if (ch == 9)
        {
            int pins = inputInt("Pin count", 2);
            printCompList(inv.getByPinCount(pins));
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════
// MODULE 2 – TAXONOMY BROWSER
// ══════════════════════════════════════════════════════════════
static void printTree(TaxonomyNode *node, int depth = 0)
{
    if (!node)
        return;
    if (node->getName() == "__root__")
    {
        for (auto *c : node->getChildren())
            printTree(c, 0);
        return;
    }
    std::cout << std::string(depth * 2, ' ')
              << (depth ? "├─ " : "") << node->getName()
              << (node->isLeaf() ? "" : "/") << "\n";
    for (auto *c : node->getChildren())
        printTree(c, depth + 1);
}

static void menuTaxonomy(Inventory &inv)
{
    while (true)
    {
        hdr("Category Browser (Taxonomy)");
        std::cout
            << "  1  Browse full tree\n"
            << "  2  List children of path\n"
            << "  3  Create new sub-category\n"
            << "  4  Delete category (leaf only)\n"
            << "  0  Back\n";
        int ch = inputInt("Choice", 0);
        if (ch == 0)
            return;

        if (ch == 1)
        {
            std::cout << "\n";
            printTree(inv.getTaxonomy().getRoot());
            pause();
        }
        else if (ch == 2)
        {
            std::string path = inputLine("Path (leave blank for root): ");
            for (auto &p : inv.getTaxonomy().listChildren(path))
                std::cout << "  " << p << "\n";
            pause();
        }
        else if (ch == 3)
        {
            std::string parent = inputLine("Parent path (blank = root): ");
            std::string name = inputLine("New category name: ");
            auto *node = inv.getTaxonomy().createCategory(parent, name);
            std::cout << CLR_GREEN "  ✓ Created: " << node->getFullPath()
                      << CLR_RESET "\n";
            pause();
        }
        else if (ch == 4)
        {
            std::string path = inputLine("Path to delete: ");
            if (inv.getTaxonomy().deleteCategory(path))
                std::cout << CLR_GREEN "  ✓ Deleted.\n" CLR_RESET;
            else
                std::cout << CLR_RED "  Not found or has children.\n" CLR_RESET;
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════
// MODULE 3 – BOM IMPORT & CROSS-REFERENCE
// ══════════════════════════════════════════════════════════════
static void menuBOM(Inventory &inv)
{
    BOMCrossReferencer xref(inv);
    while (true)
    {
        hdr("BOM Import & Cross-Reference");
        std::cout
            << "  1  Import KiCad BOM CSV and cross-reference\n"
            << "  2  Show last cross-reference result details\n"
            << "  0  Back\n";
        int ch = inputInt("Choice", 0);
        if (ch == 0)
            return;

        if (ch == 1)
        {
            std::string fp = inputLine("BOM CSV file path: ");
            std::string pn = inputLine("Project name (blank = from filename): ");
            try
            {
                BOMImporter importer;
                BOM bom = importer.importKiCadCSV(fp, pn);
                std::cout << CLR_GREEN "  ✓ Imported " << bom.size()
                          << " BOM line(s) for project: " << bom.getProjectName()
                          << CLR_RESET "\n\n";

                // Show BOM entries
                std::cout << CLR_BOLD
                          << std::left
                          << "  " << std::setw(30) << "Part"
                          << std::setw(6) << "Qty"
                          << std::setw(12) << "Package"
                          << std::setw(6) << "Pins"
                          << "Reference\n"
                          << CLR_RESET
                          << "  " << std::string(70, '-') << "\n";
                for (auto &e : bom.getEntries())
                {
                    std::cout << "  "
                              << std::left
                              << std::setw(30) << e.partNumber.substr(0, 29)
                              << std::setw(6) << e.quantity
                              << std::setw(12) << e.packageType.substr(0, 11)
                              << std::setw(6) << e.pinCount
                              << e.reference << "\n";
                }

                std::cout << "\n  Cross-referencing with inventory...\n\n";
                CrossReferenceResult res = xref.compare(bom);

                std::cout << CLR_GREEN "  ✓ MATCHED (" << res.matched.size() << "):\n" CLR_RESET;
                for (auto &m : res.matched)
                    std::cout << "    " << m.bomEntry.partNumber
                              << "  →  " << m.component->getId()
                              << " " << m.component->getName()
                              << " (have=" << m.component->getQuantity()
                              << ", need=" << m.bomEntry.quantity << ")\n";

                std::cout << CLR_YELLOW "\n  ⚠ SHORTFALL (" << res.shortfall.size() << "):\n" CLR_RESET;
                for (auto &m : res.shortfall)
                    std::cout << "    " << m.bomEntry.partNumber
                              << "  deficit=" << m.deficit
                              << "  (have=" << m.component->getQuantity()
                              << ", need=" << m.bomEntry.quantity << ")\n";

                std::cout << CLR_RED "\n  ✗ MISSING (" << res.missing.size() << "):\n" CLR_RESET;
                for (auto &e : res.missing)
                    std::cout << "    " << e.partNumber
                              << "  qty=" << e.quantity
                              << "  ref=" << e.reference << "\n";

                std::cout << "\n  Estimated cost of available parts: €"
                          << std::fixed << std::setprecision(2)
                          << res.totalCost() << "\n";
            }
            catch (std::exception &ex)
            {
                std::cout << CLR_RED "  Error: " << ex.what() << CLR_RESET "\n";
            }
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════
// MODULE 4 – PARAMETRIC SEARCH
// ══════════════════════════════════════════════════════════════
static void menuSearch(Inventory &inv)
{
    ParametricSearch ps(inv);
    while (true)
    {
        hdr("Parametric Search");
        std::cout
            << "  1  Quick name/ID search\n"
            << "  2  Filter by type\n"
            << "  3  Filter by package type\n"
            << "  4  Filter by pin count\n"
            << "  5  Filter by pin count range\n"
            << "  6  Advanced: numeric parameter range\n"
            << "  7  Compound search (combine filters)\n"
            << "  0  Back\n";
        int ch = inputInt("Choice", 0);
        if (ch == 0)
            return;

        SearchCriteria sc;

        if (ch == 1)
        {
            sc.setNameFilter(inputLine("Search query: "));
        }
        else if (ch == 2)
        {
            std::cout << "  Types: RESISTOR CAPACITOR INDUCTOR MOSFET BJT DIODE IC CONNECTOR ARBITRARY\n";
            sc.setTypeFilter(inputLine("Type name: "));
        }
        else if (ch == 3)
        {
            std::cout << "  e.g.: 0402 0603 0805 1206 SOIC MSOP QFN DIP TO-220 SOT-23 THT\n";
            sc.setPackageType(inputLine("Package type: "));
        }
        else if (ch == 4)
        {
            sc.setPinCount(inputInt("Exact pin count", 2));
        }
        else if (ch == 5)
        {
            int lo = inputInt("Min pins", 2);
            int hi = inputInt("Max pins", 32);
            sc.setPinCountRange(lo, hi);
        }
        else if (ch == 6)
        {
            std::cout << "  Param examples: resistance, capacitance, inductance,\n"
                         "                  vgsThreshold, rdsOn, vceo, hfe,\n"
                         "                  forwardVoltage, supplyVoltage, maxVoltage\n";
            std::string param = inputLine("Parameter name: ");
            double lo = inputDouble("Min value", 0.0);
            double hi = inputDouble("Max value", 1e12);
            sc.addParamRange(param, lo, hi);
        }
        else if (ch == 7)
        {
            std::cout << "  Compound search — set filters (leave blank to skip):\n";
            std::string nf = inputLine("Name filter (blank=skip): ");
            if (!nf.empty())
                sc.setNameFilter(nf);
            std::string tf = inputLine("Type filter (blank=skip): ");
            if (!tf.empty())
                sc.setTypeFilter(tf);
            std::string pf = inputLine("Package type filter (blank=skip): ");
            if (!pf.empty())
                sc.setPackageType(pf);
            std::string pc_s = inputLine("Exact pin count (blank=skip): ");
            if (!pc_s.empty())
            {
                try
                {
                    sc.setPinCount(std::stoi(pc_s));
                }
                catch (...)
                {
                }
            }
            std::string cat = inputLine("Category filter (blank=skip): ");
            if (!cat.empty())
                sc.setCategory(cat);
            std::string mp = inputLine("Max unit price € (blank=skip): ");
            if (!mp.empty())
            {
                try
                {
                    sc.setMaxUnitPrice(std::stod(mp));
                }
                catch (...)
                {
                }
            }
            std::string ls = inputLine("Low stock only? (y/N): ");
            if (ls == "y" || ls == "Y")
                sc.onlyLowStock();
        }
        else
            continue;

        auto results = ps.execute(sc);
        hdr("Search Results");
        printCompList(results);
        pause();
    }
}

// ══════════════════════════════════════════════════════════════
// MODULE 5 – PROJECT MANAGEMENT
// ══════════════════════════════════════════════════════════════
static void menuProjects(Inventory &inv, ProjectManager &pm)
{
    while (true)
    {
        hdr("Project Management");
        std::cout
            << "  1  List projects\n"
            << "  2  Create project\n"
            << "  3  Checkout components for project\n"
            << "  4  Return components to inventory\n"
            << "  5  View project transaction log\n"
            << "  6  View project cost\n"
            << "  7  Export transaction log to CSV\n"
            << "  8  Restock component\n"
            << "  0  Back\n";
        int ch = inputInt("Choice", 0);
        if (ch == 0)
            return;

        if (ch == 1)
        {
            hdr("Projects");
            for (auto &n : pm.listProjects())
                std::cout << "  • " << n
                          << "  cost=€" << std::fixed << std::setprecision(2)
                          << pm.getProjectCost(n) << "\n";
            pause();
        }
        else if (ch == 2)
        {
            std::string name = inputLine("Project name: ");
            std::string desc = inputLine("Description: ");
            pm.createProject(name, desc);
            std::cout << CLR_GREEN "  ✓ Project '" << name << "' created.\n" CLR_RESET;
            pause();
        }
        else if (ch == 3)
        {
            std::string proj = inputLine("Project name: ");
            if (!pm.hasProject(proj))
            {
                std::cout << CLR_RED "  Project not found.\n" CLR_RESET;
                pause();
                continue;
            }
            std::string id = inputLine("Component ID: ");
            int qty = inputInt("Quantity to checkout", 1);
            if (pm.checkoutForProject(proj, id, qty))
                std::cout << CLR_GREEN "  ✓ Checked out " << qty << " × " << id << "\n" CLR_RESET;
            else
                std::cout << CLR_RED "  Failed (insufficient stock or not found).\n" CLR_RESET;
            pause();
        }
        else if (ch == 4)
        {
            std::string proj = inputLine("Project name: ");
            std::string id = inputLine("Component ID: ");
            int qty = inputInt("Quantity to return", 1);
            pm.returnToInventory(proj, id, qty);
            std::cout << CLR_GREEN "  ✓ Returned.\n" CLR_RESET;
            pause();
        }
        else if (ch == 5)
        {
            std::string proj = inputLine("Project name: ");
            if (!pm.hasProject(proj))
            {
                pause();
                continue;
            }
            hdr("Transaction Log: " + proj);
            for (auto &tx : pm.getProject(proj).getLog().getAll())
                std::cout << "  " << tx.toString() << "\n";
            pause();
        }
        else if (ch == 6)
        {
            std::string proj = inputLine("Project name: ");
            double cost = pm.getProjectCost(proj);
            std::cout << "  Total cost for '" << proj << "': €"
                      << std::fixed << std::setprecision(4) << cost << "\n";
            pause();
        }
        else if (ch == 7)
        {
            std::string proj = inputLine("Project name: ");
            std::string path = inputLine("Output CSV path: ");
            if (pm.hasProject(proj))
            {
                pm.getProject(proj).getLog().exportCSV(path);
                std::cout << CLR_GREEN "  ✓ Exported to " << path << "\n" CLR_RESET;
            }
            pause();
        }
        else if (ch == 8)
        {
            std::string id = inputLine("Component ID: ");
            int qty = inputInt("Quantity to add", 10);
            pm.restockComponent(id, qty);
            std::cout << CLR_GREEN "  ✓ Restocked.\n" CLR_RESET;
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════
// MODULE 6 – DATASHEET MANAGER
// ══════════════════════════════════════════════════════════════
static void menuDatasheet(Inventory &inv)
{
    DatasheetManager dm(inv);
    while (true)
    {
        hdr("Datasheet Manager");
        std::cout
            << "  1  Open datasheet for component\n"
            << "  2  Set/update datasheet path or URL\n"
            << "  3  Show datasheet path\n"
            << "  0  Back\n";
        int ch = inputInt("Choice", 0);
        if (ch == 0)
            return;
        std::string id = inputLine("Component ID: ");

        if (ch == 1)
        {
            dm.openDatasheet(id);
        }
        else if (ch == 2)
        {
            std::string path = inputLine("Datasheet path or URL: ");
            dm.setDatasheet(id, path);
            std::cout << CLR_GREEN "  ✓ Set.\n" CLR_RESET;
        }
        else if (ch == 3)
        {
            std::cout << "  " << dm.getDatasheet(id) << "\n";
        }
        pause();
    }
}

// ══════════════════════════════════════════════════════════════
// MODULE 7 – LOW STOCK ALERTS
// ══════════════════════════════════════════════════════════════
static void menuLowStock(Inventory &inv)
{
    while (true)
    {
        hdr("Low Stock Alerts");
        std::cout
            << "  1  Show low stock report\n"
            << "  2  Export low stock report to CSV\n"
            << "  0  Back\n";
        int ch = inputInt("Choice", 0);
        if (ch == 0)
            return;

        if (ch == 1)
        {
            std::cout << "\n"
                      << LowStockAlert::generateReport(inv);
        }
        else if (ch == 2)
        {
            std::string path = inputLine("Output CSV path: ");
            if (LowStockAlert::exportReport(inv, path))
                std::cout << CLR_GREEN "  ✓ Exported to " << path << "\n" CLR_RESET;
            else
                std::cout << CLR_RED "  Failed to write file.\n" CLR_RESET;
        }
        pause();
    }
}

// ══════════════════════════════════════════════════════════════
// SAMPLE DATA
// ══════════════════════════════════════════════════════════════
static void loadSampleData(Inventory &inv)
{
    // Resistors
    inv.addComponent(new Resistor("RES0001", "1k Resistor", 100, 1.0, 1.0, 0.1, "DIN0207", 5, 0.02, "Passives/Resistors/THT", "", "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal", 2));
    inv.addComponent(new Resistor("RES0002", "47k Resistor", 50, 1.0, 1.0, 0.1, "DIN0207", 5, 0.02, "Passives/Resistors/THT", "", "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal", 2));
    inv.addComponent(new Resistor("RES0003", "4.7k Resistor", 80, 1.0, 1.0, 0.1, "DIN0207", 5, 0.02, "Passives/Resistors/THT", "", "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal", 2));
    inv.addComponent(new Resistor("RES0004", "10k SMD 0603", 200, 1.0, 1.0, 0.1, "0603", 10, 0.01, "Passives/Resistors/SMD", "", "Resistor_SMD:R_0603_1608Metric", 2));
    inv.addComponent(new Resistor("RES0005", "100R SMD 0402", 50, 1.0, 5.0, 0.063, "0402", 10, 0.008, "Passives/Resistors/SMD", "", "Resistor_SMD:R_0402_1005Metric", 2));
    // Capacitors
    inv.addComponent(new Capacitor("CAP0001", "4.7nF Disc", 10, 4.7e-9, "ceramic", 10.0, 100.0, "Disc", 2, 0.05, "Passives/Capacitors/Ceramic", "", "Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm", 2));
    inv.addComponent(new Capacitor("CAP0002", "1000uF Electro", 3, 1000e-6, "electrolytic", 20.0, 35.0, "Radial", 2, 0.30, "Passives/Capacitors/Electrolytic", "", "Capacitor_THT:CP_Radial_D16.0mm_P7.50mm", 2));
    inv.addComponent(new Capacitor("CAP0003", "100nF SMD 0603", 500, 100e-9, "ceramic", 10.0, 25.0, "0603", 10, 0.015, "Passives/Capacitors/Ceramic", "", "Capacitor_SMD:C_0603_1608Metric", 2));
    inv.addComponent(new Capacitor("CAP0004", "10uF SMD 0805", 100, 10e-6, "ceramic", 20.0, 10.0, "0805", 10, 0.05, "Passives/Capacitors/Ceramic", "", "Capacitor_SMD:C_0805_2012Metric", 2));
    // Inductors
    inv.addComponent(new Inductor("IND0001", "33uH Toroid THT", 5, 33e-6, 3.0, 40.0, "Toroid", 2, 0.80, "Passives/Inductors/THT", "", "Inductor_THT:L_Toroid_Vertical_L13.0mm_W6.5mm_P5.60mm", 2));
    inv.addComponent(new Inductor("IND0002", "10uH SMD", 20, 10e-6, 1.5, 0.0, "1210", 5, 0.25, "Passives/Inductors/SMD", "", "", 2));
    // Diodes
    inv.addComponent(new DiodeComponent("DIO0001", "1N5822 Schottky", 8, 0.45, 40.0, "schottky", 3.0, "DO-201AD", 2, 0.35, "Semiconductors/Diodes/Schottky", "http://www.vishay.com/docs/88526/1n5820.pdf", "Diode_THT:D_DO-201AD_P15.24mm_Horizontal", "THT-DO201", 2));
    inv.addComponent(new DiodeComponent("DIO0002", "1N4007 Rectifier", 50, 0.7, 1000.0, "rectifier", 1.0, "DO-41", 5, 0.05, "Semiconductors/Diodes/Rectifier", "", "", "DO-41", 2));
    inv.addComponent(new DiodeComponent("DIO0003", "BZX55C5V1 Zener", 30, 5.1, 5.1, "zener", 0.2, "DO-35", 5, 0.08, "Semiconductors/Diodes/Zener", "", "", "DO-35", 2));
    // ICs
    inv.addComponent(new IntegratedCircuit("IC_0001", "LM2596T-ADJ", 2, "Step-down switching regulator", 3.0, 40.0, 3.0, "TO-220-5", 5, 2, 2.50, "Semiconductors/ICs/Power/DCDC", "http://www.ti.com/lit/ds/symlink/lm2596.pdf", "Package_TO_SOT_THT:TO-220-5_P3.4x3.7mm_StaggerOdd_Lead3.8mm_Vertical", "TO-220"));
    inv.addComponent(new IntegratedCircuit("IC_0002", "LT1764AET#PBF", 3, "Low-dropout regulator 3A", 1.5, 20.0, 3.0, "TO-263-5", 5, 2, 3.80, "Semiconductors/ICs/Power/LDO", "https://www.analog.com/media/en/technical-documentation/data-sheets/1764f.pdf", "", "TO-263"));
    inv.addComponent(new IntegratedCircuit("IC_0003", "LTC4365", 1, "OV/UV/Reverse Protection", 2.5, 18.0, 0.01, "MSOP-10", 10, 2, 4.20, "Semiconductors/ICs/Protection", "https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4365.pdf", "", "MSOP"));
    inv.addComponent(new IntegratedCircuit("IC_0004", "ESP32-WROOM-32", 1, "WiFi+BT SoC", 3.3, 3.6, 0.5, "Module", 38, 2, 5.00, "Semiconductors/ICs/Microcontrollers", "", "", "Module"));
    // BJTs
    inv.addComponent(new BJTTransistor("BJT0001", "BC547", 20, "NPN", 200, 45, 0.1, "TO-92", 5, 0.08, "Semiconductors/Transistors/BJT/NPN", "", "", "TO-92", 3));
    inv.addComponent(new BJTTransistor("BJT0002", "BC557", 10, "PNP", 200, 45, 0.1, "TO-92", 5, 0.08, "Semiconductors/Transistors/BJT/PNP", "", "", "TO-92", 3));
    // MOSFETs
    inv.addComponent(new MOSFETTransistor("MOS0001", "IRLZ44N", 5, "N-CH", 2.0, 0.022, 48.0, 55.0, 47.0, "TO-220", 2, 0.65, "Semiconductors/Transistors/MOSFET/N-Channel", "", "Package_TO_SOT_THT:TO-220-3_P3.4mm_StaggerOdd_Vertical", "TO-220", 3));
    inv.addComponent(new MOSFETTransistor("MOS0002", "AO4407", 5, "P-CH", 1.8, 0.018, 30.0, 30.0, 12.0, "SOP-8", 2, 0.90, "Semiconductors/Transistors/MOSFET/P-Channel", "", "", "SOP", 3));
    // Connectors
    inv.addComponent(new ConnectorComponent("CON0001", "PinHeader 1x08 2.54mm", 4, "PinHeader", 2.54, "male", 8, 2, 0.15, "Connectors/PinHeaders", "", "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical", "PinHeader"));
    inv.addComponent(new ConnectorComponent("CON0002", "PinHeader 1x02 2.54mm", 10, "PinHeader", 2.54, "male", 2, 2, 0.08, "Connectors/PinHeaders", "", "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical", "PinHeader"));
    inv.addComponent(new ConnectorComponent("CON0003", "Screw Terminal 1x02", 6, "TerminalBlock", 5.08, "female", 2, 2, 0.35, "Connectors/ScrewTerminals", "", "TerminalBlock_MetzConnect:TerminalBlock_MetzConnect_Type101_RT01602HBWC_1x02_P5.08mm_Horizontal", "TerminalBlock"));
    inv.addComponent(new ConnectorComponent("CON0004", "PinHeader 1x01 2.54mm", 20, "PinHeader", 2.54, "male", 1, 5, 0.05, "Connectors/PinHeaders", "", "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical", "PinHeader"));
    // Arbitrary
    auto *fuse = new ArbitraryComponent("ARB0001", "Inline Fuse 2A", 2, "Glass fuse 5x20mm", 3, 0.40, "Passives/Fuses", "", "AO4407:MyFuse_Inline_20x5mm_L20mm_P5.5mm", "THT", 2);
    fuse->addParam("rating_A", "2");
    fuse->addParam("size_mm", "5x20");
    fuse->addParam("type", "fast-blow");
    inv.addComponent(fuse);
    auto *pot = new ArbitraryComponent("ARB0002", "Potentiometer 10k", 1, "Bourns 3296W trim pot", 2, 0.85, "Passives/Potentiometers", "", "Potentiometer_THT:Potentiometer_Bourns_3296W_Vertical", "THT", 3);
    pot->addParam("resistance_Ohm", "10000");
    pot->addParam("turns", "25");
    pot->addParam("power_W", "0.5");
    inv.addComponent(pot);

    std::cout << CLR_GREEN "  ✓ Sample data loaded ("
              << inv.size() << " components)\n" CLR_RESET;
}

// ══════════════════════════════════════════════════════════════
// MAIN MENU
// ══════════════════════════════════════════════════════════════
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    enableANSI();
    Inventory inv;
    ProjectManager pm(inv);

    std::vector<std::string> bootMenu = {
        "Load saved data",
        "Load sample data (demonstration)",
        "Start empty"};

    int boot = arrowMenu(
        "Welcome to ECIMS"
        "\n\n  Data directory: " +
            DATA_DIR,
        bootMenu,
        4);

    boot += 1;

    if (boot == 1)
    {
        if (Persistence::loadAll(inv, pm, DATA_DIR))
            std::cout << CLR_GREEN "  ✓ Data loaded. " << inv.size() << " components.\n" CLR_RESET;
        else
            std::cout << CLR_YELLOW "  No data found — starting empty.\n" CLR_RESET;
    }
    else if (boot == 2)
    {
        loadSampleData(inv);
    }

    while (true)
    {
        std::vector<std::string> mainMenu = {
            "Inventory (CRUD)",
            "Category Browser",
            "BOM Import & Cross-Reference",
            "Parametric Search",
            "Project Management",
            "Datasheet Manager",
            "Low Stock Alerts",
            "Save to disk",
            "Load from disk",
            "Exit"};

        int ch = arrowMenu(
            "Main Menu"
            "\n\n  Inventory: " +
                std::to_string(inv.size()) +
                " components"
                "\n  Low stock: " +
                std::to_string(inv.getLowStock().size()),
            mainMenu,
            4);

        if (ch == 0)
            menuInventory(inv);
        else if (ch == 1)
            menuTaxonomy(inv);
        else if (ch == 2)
            menuBOM(inv);
        else if (ch == 3)
            menuSearch(inv);
        else if (ch == 4)
            menuProjects(inv, pm);
        else if (ch == 5)
            menuDatasheet(inv);
        else if (ch == 6)
            menuLowStock(inv);
        else if (ch == 7)
        {
            if (Persistence::saveAll(inv, pm, DATA_DIR))
                std::cout << CLR_GREEN "\n  ✓ Saved to '" << DATA_DIR << "/'\n" CLR_RESET;
            else
                std::cout << CLR_RED "\n  Save failed.\n" CLR_RESET;
            pause();
        }
        else if (ch == 8)
        {
            Persistence::loadAll(inv, pm, DATA_DIR);
            std::cout << CLR_GREEN "\n  ✓ Loaded.\n" CLR_RESET;
            pause();
        }
        else if (ch == 9)
        {
            break;
        }
    }

    std::cout << CLR_BOLD CLR_BLUE "\n  Goodbye.\n\n" CLR_RESET;
    return 0;
}
