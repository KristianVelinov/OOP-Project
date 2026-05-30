// /*  ECIMS — Electronic Component Inventory Management System
//     Console UI  */

// #include <windows.h>
// #include <conio.h>

// #include "Inventory.h"
// #include "Resistor.h"
// #include "Capacitor.h"
// #include "Inductor.h"
// #include "MOSFETTransistor.h"
// #include "BJTTransistor.h"
// #include "DiodeComponent.h"
// #include "IntegratedCircuit.h"
// #include "ConnectorComponent.h"
// #include "ArbitraryComponent.h"
// #include "BOM.h"
// #include "BOMImporter.h"
// #include "BOMCrossReferencer.h"
// #include "CrossReferenceResult.h"
// #include "SearchCriteria.h"
// #include "ParametricSearch.h"
// #include "ProjectManager.h"
// #include "LowStockAlert.h"
// #include "DatasheetManager.h"
// #include "Persistence.h"
// #include "FootprintParser.h"

// #include <iostream>
// #include <fstream>
// #include <string>
// #include <limits>
// #include <iomanip>
// #include <sstream>
// #include <algorithm>
// #include <vector>
// #include <functional>

// // ── ANSI colours ──────────────────────────────────────────────────────────────
// #ifdef _WIN32
// #  define CLR_RESET   "\x1b[0m"
// #  define CLR_BOLD    "\x1b[1m"
// #  define CLR_DIM     "\x1b[2m"
// #  define CLR_CYAN    "\x1b[36m"
// #  define CLR_GREEN   "\x1b[32m"
// #  define CLR_YELLOW  "\x1b[33m"
// #  define CLR_RED     "\x1b[31m"
// #  define CLR_BLUE    "\x1b[34m"
// #  define CLR_MAG     "\x1b[35m"
// #  define CLR_WHITE   "\x1b[37m"
// #  define CLR_SELECT  "\x1b[30;47m"
// #  define CLR_SETTINGON  "\x1b[32m"   // green  = active / enabled
// #  define CLR_SETTINGOFF "\x1b[90m"   // dark   = inactive / disabled
// #else
// #  define CLR_RESET   "\033[0m"
// #  define CLR_BOLD    "\033[1m"
// #  define CLR_DIM     "\033[2m"
// #  define CLR_CYAN    "\033[36m"
// #  define CLR_GREEN   "\033[32m"
// #  define CLR_YELLOW  "\033[33m"
// #  define CLR_RED     "\033[31m"
// #  define CLR_BLUE    "\033[34m"
// #  define CLR_MAG     "\033[35m"
// #  define CLR_WHITE   "\033[37m"
// #  define CLR_SELECT  "\033[30;47m"
// #  define CLR_SETTINGON  "\033[32m"
// #  define CLR_SETTINGOFF "\033[90m"
// #endif

// // ── Global settings ───────────────────────────────────────────────────────────
// enum class SaveMode { MANUAL, AUTO, PROMPTED };

// struct AppSettings {
//     SaveMode    saveMode          = SaveMode::MANUAL;
//     bool        confirmDelete     = true;
//     bool        valueAwareBOM     = true;   // use type/unit-aware BOM matching
//     bool        bomTypeWarnings   = true;   // annotate BOM results with type hints
//     bool        colorTableRows    = true;   // alternate row tinting in component lists
// };

// static AppSettings  g_settings;
// static std::string  g_dataDir        = "data";
// static const char*  SETTINGS_FILE    = "ecims_settings.ini";

// // ── Settings persistence ──────────────────────────────────────────────────────
// static void saveSettings() {
//     std::ofstream f(SETTINGS_FILE);
//     if (!f) return;
//     f << "saveMode="        << (int)g_settings.saveMode        << "\n"
//       << "confirmDelete="   << (int)g_settings.confirmDelete   << "\n"
//       << "valueAwareBOM="   << (int)g_settings.valueAwareBOM   << "\n"
//       << "bomTypeWarnings=" << (int)g_settings.bomTypeWarnings << "\n"
//       << "colorTableRows="  << (int)g_settings.colorTableRows  << "\n"
//       << "dataDir="         << g_dataDir                       << "\n";
// }

// static void loadSettings() {
//     std::ifstream f(SETTINGS_FILE);
//     if (!f) return;
//     std::string line;
//     while (std::getline(f, line)) {
//         auto eq = line.find('=');
//         if (eq == std::string::npos) continue;
//         std::string key = line.substr(0, eq);
//         std::string val = line.substr(eq + 1);
//         try {
//             if      (key == "saveMode")        g_settings.saveMode        = (SaveMode)std::stoi(val);
//             else if (key == "confirmDelete")   g_settings.confirmDelete   = (val == "1");
//             else if (key == "valueAwareBOM")   g_settings.valueAwareBOM   = (val == "1");
//             else if (key == "bomTypeWarnings") g_settings.bomTypeWarnings = (val == "1");
//             else if (key == "colorTableRows")  g_settings.colorTableRows  = (val == "1");
//             else if (key == "dataDir" && !val.empty()) g_dataDir          = val;
//         } catch (...) {}
//     }
// }

// // ── Forward declarations ──────────────────────────────────────────────────────
// static void menuInventory(Inventory& inv, ProjectManager& pm);
// static void menuTaxonomy(Inventory& inv, ProjectManager& pm);
// static void menuBOM(Inventory& inv);
// static void menuSearch(Inventory& inv);
// static void menuProjects(Inventory& inv, ProjectManager& pm);
// static void menuDatasheet(Inventory& inv);
// static void menuLowStock(Inventory& inv);
// static void menuSettings();

// // ── I/O helpers ───────────────────────────────────────────────────────────────
// static void clearScreen() {
// #ifdef _WIN32
//     system("cls");
// #else
//     system("clear");
// #endif
// }

// static void enableANSI() {
// #ifdef _WIN32
//     HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//     if (hOut == INVALID_HANDLE_VALUE) return;
//     DWORD dwMode = 0;
//     if (!GetConsoleMode(hOut, &dwMode)) return;
//     dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
//     SetConsoleMode(hOut, dwMode);
// #endif
// }

// static void pause() {
//     std::cout << "\n" << CLR_YELLOW "  [Press ENTER]" CLR_RESET;
//     while (_getch() != 13) {}
// }

// static std::string inputLine(const std::string& prompt) {
//     std::cout << CLR_CYAN "  " << prompt << CLR_RESET;
//     std::string s;
//     std::getline(std::cin, s);
//     return s;
// }

// static int inputInt(const std::string& prompt, int def = 0) {
//     std::cout << CLR_CYAN "  " << prompt << " [" << def << "]: " CLR_RESET;
//     std::string s;
//     std::getline(std::cin, s);
//     if (s.empty()) return def;
//     try { return std::stoi(s); } catch (...) { return def; }
// }

// static double inputDouble(const std::string& prompt, double def = 0.0) {
//     std::cout << CLR_CYAN "  " << prompt << " [" << def << "]: " CLR_RESET;
//     std::string s;
//     std::getline(std::cin, s);
//     if (s.empty()) return def;
//     try { return std::stod(s); } catch (...) { return def; }
// }

// // ── Banner / header ───────────────────────────────────────────────────────────
// static void banner() {
//     std::cout << CLR_BOLD CLR_BLUE
//               << "\n╔══════════════════════════════════════════════════════╗\n"
//                  "║   ECIMS — Electronic Component Inventory Manager     ║\n"
//                  "╚══════════════════════════════════════════════════════╝\n"
//               << CLR_RESET;
// }

// static void hdr(const std::string& title) {
//     std::cout << CLR_BOLD CLR_CYAN
//               << "\n  ── " << title << " ──\n"
//               << CLR_RESET;
// }

// // ── Arrow-navigable menu ──────────────────────────────────────────────────────
// // Returns 0-based index of the selected item.
// static int arrowMenu(
//     const std::string&              title,
//     const std::vector<std::string>& items,
//     int                             indent  = 2,
//     int                             startAt = 0)
// {
//     int selected = startAt;
//     if (selected < 0 || selected >= (int)items.size()) selected = 0;

//     while (true) {
//         clearScreen();
//         banner();
//         hdr(title);
//         std::cout << "\n";

//         for (int i = 0; i < (int)items.size(); ++i) {
//             std::cout << std::string(indent, ' ');
//             if (i == selected)
//                 std::cout << CLR_SELECT "  > " << items[i] << "  " CLR_RESET;
//             else
//                 std::cout << "    " << items[i];
//             std::cout << "\n\n";
//         }

//         std::cout << CLR_DIM "  ↑↓ navigate   Enter select" CLR_RESET "\n";

//         int ch = _getch();
//         if (ch == 224) {
//             ch = _getch();
//             if      (ch == 72) { if (--selected < 0) selected = (int)items.size() - 1; }
//             else if (ch == 80) { if (++selected >= (int)items.size()) selected = 0; }
//         } else if (ch == 13) {
//             return selected;
//         }
//     }
// }

// // ── Auto-save helper ──────────────────────────────────────────────────────────
// static void trySave(Inventory& inv, ProjectManager& pm) {
//     if (g_settings.saveMode == SaveMode::AUTO) {
//         bool ok = Persistence::saveAll(inv, pm, g_dataDir);
//         std::cout << (ok
//             ? CLR_DIM "  ✓ Auto-saved.\n" CLR_RESET
//             : CLR_RED "  Auto-save failed!\n" CLR_RESET);
//     }
// }

// // ── Component list printer ────────────────────────────────────────────────────
// static void printCompList(const std::vector<Component*>& list) {
//     if (list.empty()) { std::cout << "  (none)\n"; return; }

//     std::cout << CLR_BOLD
//               << std::left
//               << "  " << std::setw(10) << "ID"
//               << std::setw(28) << "Name"
//               << std::setw(12) << "Type"
//               << std::setw(8)  << "Qty"
//               << std::setw(8)  << "Min"
//               << std::setw(8)  << "Pins"
//               << std::setw(10) << "Package"
//               << "Price\n"
//               << CLR_RESET
//               << "  " << std::string(90, '-') << "\n";

//     for (int i = 0; i < (int)list.size(); ++i) {
//         auto* c = list[i];
//         bool  low = c->isLowStock();

//         // Optional alternating row tint
//         if (g_settings.colorTableRows && (i % 2 == 1))
//             std::cout << CLR_DIM;

//         std::cout << "  "
//                   << std::left
//                   << std::setw(10) << c->getId()
//                   << std::setw(28) << c->getName().substr(0, 27)
//                   << std::setw(12) << c->getTypeName().substr(0, 11)
//                   << std::setw(8)  << c->getQuantity()
//                   << std::setw(8)  << c->getMinStock()
//                   << std::setw(8)  << c->getPinCount()
//                   << std::setw(10) << c->getPackageType().substr(0, 9)
//                   << "€" << std::fixed << std::setprecision(4) << c->getUnitPrice();

//         if (g_settings.colorTableRows && (i % 2 == 1)) std::cout << CLR_RESET;
//         if (low) std::cout << CLR_RED " [LOW]" CLR_RESET;
//         std::cout << "\n";
//     }
//     std::cout << "  " << list.size() << " component(s)\n";
// }

// // ── ID generator ──────────────────────────────────────────────────────────────
// static std::string genId(Inventory& inv, const std::string& prefix) {
//     static int ctr = 1;
//     std::ostringstream oss;
//     do {
//         oss.str("");
//         oss << prefix << std::setw(4) << std::setfill('0') << ctr++;
//     } while (inv.getComponent(oss.str()));
//     return oss.str();
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 1 – INVENTORY CRUD
// // ══════════════════════════════════════════════════════════════════════════════
// static void moduleAddComponent(Inventory& inv, ProjectManager& pm) {
//     clearScreen(); banner();
//     hdr("Add Component — Select Type");

//     int t = arrowMenu("Component type", {
//         "Resistor",
//         "Capacitor",
//         "Inductor",
//         "MOSFET Transistor",
//         "BJT Transistor",
//         "Diode",
//         "Integrated Circuit (IC)",
//         "Connector",
//         "Arbitrary / Other",
//         "Cancel"
//     }, 4);

//     if (t == 9) return;   // Cancel
//     t += 1;               // 1-based from here

//     clearScreen(); banner();
//     hdr("Add Component");

//     std::string name = inputLine("Name (e.g. 10k, LM2596T-ADJ): ");
//     if (name.empty()) { std::cout << CLR_YELLOW "  Cancelled.\n" CLR_RESET; pause(); return; }

//     int    qty = inputInt   ("Quantity",              10);
//     int    mns = inputInt   ("Min stock threshold",    2);
//     double prc = inputDouble("Unit price (€)",       0.0);

//     std::string fp  = inputLine("Footprint (KiCad format or blank): ");
//     std::string pkg = FootprintParser::extractPackageType(fp);
//     int         pc  = FootprintParser::extractPinCount(fp);
//     if (pkg.empty()) pkg = inputLine("Package type (e.g. 0603, SOIC, THT): ");
//     if (pc  == 0  ) pc  = inputInt ("Pin count", 2);

//     std::string ds  = inputLine("Datasheet path/URL (blank to skip): ");

//     Component* c = nullptr;

//     if (t == 1) {   // RESISTOR
//         std::string cat = inputLine("Category [Passives/Resistors/SMD]: ");
//         if (cat.empty()) cat = "Passives/Resistors/SMD";
//         double res = inputDouble("Resistance (Ω, e.g. 10000 or 10e3)", 1000.0);
//         double tol = inputDouble("Tolerance (%)", 1.0);
//         double pwr = inputDouble("Power rating (W)", 0.1);
//         c = new Resistor(genId(inv, "RES"), name, qty, res, tol, pwr,
//                          pkg, mns, prc, cat, ds, fp, pc);
//     }
//     else if (t == 2) {   // CAPACITOR
//         std::string cat = inputLine("Category [Passives/Capacitors/Ceramic]: ");
//         if (cat.empty()) cat = "Passives/Capacitors/Ceramic";
//         double cap = inputDouble("Capacitance (F, e.g. 100e-9 for 100 nF)", 100e-9);
//         std::string ct = inputLine("Dielectric (ceramic/electrolytic/tantalum/film) [ceramic]: ");
//         if (ct.empty()) ct = "ceramic";
//         double tol = inputDouble("Tolerance (%)", 10.0);
//         double vr  = inputDouble("Voltage rating (V)", 10.0);
//         c = new Capacitor(genId(inv, "CAP"), name, qty, cap, ct, tol, vr,
//                           pkg, mns, prc, cat, ds, fp, pc);
//     }
//     else if (t == 3) {   // INDUCTOR
//         std::string cat = inputLine("Category [Passives/Inductors/THT]: ");
//         if (cat.empty()) cat = "Passives/Inductors/THT";
//         double ind  = inputDouble("Inductance (H, e.g. 33e-6 for 33 µH)", 33e-6);
//         double irat = inputDouble("Current rating (A)", 1.0);
//         double dcr  = inputDouble("DCR / series resistance (Ω)", 0.0);
//         c = new Inductor(genId(inv, "IND"), name, qty, ind, irat, dcr,
//                          pkg, mns, prc, cat, ds, fp, pc);
//     }
//     else if (t == 4) {   // MOSFET
//         std::string cat = inputLine("Category [Semiconductors/Transistors/MOSFET/N-Channel]: ");
//         if (cat.empty()) cat = "Semiconductors/Transistors/MOSFET/N-Channel";
//         std::string tt  = inputLine("Type (N-CH / P-CH) [N-CH]: ");
//         if (tt.empty()) tt = "N-CH";
//         double vgs = inputDouble("Vgs(th) (V)", 2.0);
//         double rds = inputDouble("Rds(on) (Ω)", 0.01);
//         double qg  = inputDouble("Gate charge (nC)", 10.0);
//         double mv  = inputDouble("Max Vds (V)", 30.0);
//         double mc  = inputDouble("Max Id (A)", 5.0);
//         c = new MOSFETTransistor(genId(inv, "MOS"), name, qty, tt, vgs, rds, qg,
//                                  mv, mc, pkg, mns, prc, cat, ds, fp, pkg, pc);
//     }
//     else if (t == 5) {   // BJT
//         std::string cat = inputLine("Category [Semiconductors/Transistors/BJT/NPN]: ");
//         if (cat.empty()) cat = "Semiconductors/Transistors/BJT/NPN";
//         std::string tt  = inputLine("Type (NPN / PNP) [NPN]: ");
//         if (tt.empty()) tt = "NPN";
//         double hfe  = inputDouble("hFE (gain)", 100.0);
//         double vceo = inputDouble("Vceo (V)", 40.0);
//         double icm  = inputDouble("Ic max (A)", 0.1);
//         c = new BJTTransistor(genId(inv, "BJT"), name, qty, tt, hfe, vceo, icm,
//                               pkg, mns, prc, cat, ds, fp, pkg, pc);
//     }
//     else if (t == 6) {   // DIODE
//         std::string cat = inputLine("Category [Semiconductors/Diodes/Rectifier]: ");
//         if (cat.empty()) cat = "Semiconductors/Diodes/Rectifier";
//         double vf  = inputDouble("Forward voltage Vf (V)", 0.7);
//         double vrm = inputDouble("Reverse voltage VRM (V)", 40.0);
//         std::string dt = inputLine("Type (rectifier/schottky/zener/TVS/LED) [rectifier]: ");
//         if (dt.empty()) dt = "rectifier";
//         double imax = inputDouble("If max (A)", 1.0);
//         c = new DiodeComponent(genId(inv, "DIO"), name, qty, vf, vrm, dt, imax,
//                                pkg, mns, prc, cat, ds, fp, pkg, pc);
//     }
//     else if (t == 7) {   // IC
//         std::string cat = inputLine("Category [Semiconductors/ICs/Power/DCDC]: ");
//         if (cat.empty()) cat = "Semiconductors/ICs/Power/DCDC";
//         std::string func = inputLine("Function description: ");
//         double sv = inputDouble("Supply voltage (V)", 3.3);
//         double mv = inputDouble("Max voltage (V)", 40.0);
//         double mc = inputDouble("Max current (A)", 0.01);
//         c = new IntegratedCircuit(genId(inv, "IC_"), name, qty, func, sv, mv, mc,
//                                   pkg, pc, mns, prc, cat, ds, fp, pkg);
//     }
//     else if (t == 8) {   // CONNECTOR
//         std::string cat = inputLine("Category [Connectors/PinHeaders]: ");
//         if (cat.empty()) cat = "Connectors/PinHeaders";
//         std::string ct    = inputLine("Connector type (PinHeader/JST/Molex/USB-C/SMA): ");
//         double      pitch = inputDouble("Pitch (mm)", 2.54);
//         std::string gen   = inputLine("Gender (male/female) [male]: ");
//         if (gen.empty()) gen = "male";
//         c = new ConnectorComponent(genId(inv, "CON"), name, qty, ct, pitch, gen,
//                                    pc, mns, prc, cat, ds, fp, pkg);
//     }
//     else {   // ARBITRARY
//         std::string cat  = inputLine("Category [Other]: ");
//         if (cat.empty()) cat = "Other";
//         std::string desc = inputLine("Description: ");
//         auto* arb = new ArbitraryComponent(genId(inv, "ARB"), name, qty, desc,
//                                            mns, prc, cat, ds, fp, pkg, pc);
//         std::cout << "  Add custom parameters (key=value, blank to stop):\n";
//         while (true) {
//             std::string kv = inputLine("  param (key=value): ");
//             if (kv.empty()) break;
//             auto eq = kv.find('=');
//             if (eq != std::string::npos)
//                 arb->addParam(kv.substr(0, eq), kv.substr(eq + 1));
//         }
//         c = arb;
//     }

//     if (c) {
//         inv.addComponent(c);
//         std::cout << CLR_GREEN "\n  ✓ Added: " << c->getId()
//                   << " — " << c->getName() << "\n" CLR_RESET;
//         trySave(inv, pm);
//     }
//     pause();
// }

// static void moduleEditComponent(Inventory& inv, ProjectManager& pm) {
//     clearScreen(); banner(); hdr("Edit Component");
//     std::string id = inputLine("Component ID to edit: ");
//     Component* c = inv.getComponent(id);
//     if (!c) {
//         std::cout << CLR_RED "  Not found.\n" CLR_RESET; pause(); return;
//     }
//     c->display();

//     int ch = arrowMenu("Field to edit", {
//         "Name",
//         "Quantity",
//         "Min stock threshold",
//         "Unit price",
//         "Category path",
//         "Datasheet path / URL",
//         "Package type",
//         "Pin count",
//         "Cancel"
//     }, 4);

//     clearScreen(); banner(); hdr("Edit: " + c->getName());

//     bool changed = true;
//     switch (ch) {
//         case 0: c->setName      (inputLine  ("New name: "));            break;
//         case 1: c->setQuantity  (inputInt   ("New quantity"));          break;
//         case 2: c->setMinStock  (inputInt   ("New min stock"));         break;
//         case 3: c->setUnitPrice (inputDouble("New unit price (€)"));    break;
//         case 4: c->setCategoryPath(inputLine("New category path: "));   break;
//         case 5: c->setDatasheetPath(inputLine("New datasheet path/URL: ")); break;
//         case 6: c->setPackageType(inputLine ("New package type: "));    break;
//         case 7: c->setPinCount  (inputInt   ("New pin count"));         break;
//         default: std::cout << "  Cancelled.\n"; changed = false; break;
//     }

//     if (changed) {
//         std::cout << CLR_GREEN "  ✓ Updated.\n" CLR_RESET;
//         trySave(inv, pm);
//     }
//     pause();
// }

// static void moduleDeleteComponent(Inventory& inv, ProjectManager& pm) {
//     clearScreen(); banner(); hdr("Delete Component");
//     std::string id = inputLine("Component ID to delete: ");
//     Component* c = inv.getComponent(id);
//     if (!c) {
//         std::cout << CLR_RED "  Not found.\n" CLR_RESET; pause(); return;
//     }
//     std::cout << "\n  Component: " << CLR_BOLD << c->getName() << CLR_RESET
//               << "  (" << c->getId() << ")\n";

//     if (g_settings.confirmDelete) {
//         int ans = arrowMenu("Confirm deletion", {
//             "Yes — delete permanently",
//             "No  — cancel"
//         }, 4);
//         if (ans != 0) { std::cout << "  Cancelled.\n"; pause(); return; }
//     }

//     inv.removeComponent(id);
//     std::cout << CLR_GREEN "  ✓ Deleted.\n" CLR_RESET;
//     trySave(inv, pm);
//     pause();
// }

// // ── Inventory menu ─────────────────────────────────────────────────────────────
// static void menuInventory(Inventory& inv, ProjectManager& pm) {
//     while (true) {
//         int ch = arrowMenu(
//             "Inventory Management"
//             "\n\n  " + std::to_string(inv.size()) + " components"
//             "  |  " + std::to_string(inv.getLowStock().size()) + " low stock",
//             {
//                 "List all components",
//                 "View component detail",
//                 "Add component",
//                 "Edit component",
//                 "Delete component",
//                 "List by category",
//                 "Quick name / ID search",
//                 "List by package type",
//                 "List by pin count",
//                 "Back"
//             }, 4);

//         if (ch == 9) return;

//         if (ch == 0) {
//             clearScreen(); banner(); hdr("All Components");
//             printCompList(inv.getAll());
//             pause();
//         }
//         else if (ch == 1) {
//             clearScreen(); banner(); hdr("View Component");
//             std::string id = inputLine("Component ID: ");
//             Component* c = inv.getComponent(id);
//             if (c) c->display();
//             else   std::cout << CLR_RED "  Not found.\n" CLR_RESET;
//             pause();
//         }
//         else if (ch == 2) { moduleAddComponent(inv, pm); }
//         else if (ch == 3) { moduleEditComponent(inv, pm); }
//         else if (ch == 4) { moduleDeleteComponent(inv, pm); }
//         else if (ch == 5) {
//             clearScreen(); banner(); hdr("List by Category");
//             std::string path = inputLine("Category path (e.g. Passives/Resistors): ");
//             printCompList(inv.getByCategory(path));
//             pause();
//         }
//         else if (ch == 6) {
//             clearScreen(); banner(); hdr("Name / ID Search");
//             std::string q = inputLine("Search query: ");
//             printCompList(inv.searchByName(q));
//             pause();
//         }
//         else if (ch == 7) {
//             clearScreen(); banner(); hdr("List by Package Type");
//             std::string pkg = inputLine("Package type (e.g. 0603, SOIC, THT, QFN): ");
//             printCompList(inv.getByPackageType(pkg));
//             pause();
//         }
//         else if (ch == 8) {
//             clearScreen(); banner(); hdr("List by Pin Count");
//             int pins = inputInt("Pin count", 2);
//             printCompList(inv.getByPinCount(pins));
//             pause();
//         }
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 2 – TAXONOMY BROWSER
// // ══════════════════════════════════════════════════════════════════════════════
// static void printTree(TaxonomyNode* node, int depth = 0) {
//     if (!node) return;
//     if (node->getName() == "__root__") {
//         for (auto* c : node->getChildren()) printTree(c, 0);
//         return;
//     }
//     std::cout << std::string(depth * 2, ' ')
//               << (depth ? "├─ " : "")
//               << CLR_CYAN << node->getName() << CLR_RESET
//               << (node->isLeaf() ? "" : CLR_DIM "/" CLR_RESET) << "\n";
//     for (auto* c : node->getChildren()) printTree(c, depth + 1);
// }

// static void menuTaxonomy(Inventory& inv, ProjectManager& /*pm*/) {
//     while (true) {
//         int ch = arrowMenu("Category Browser (Taxonomy)", {
//             "Browse full tree",
//             "List children of path",
//             "Create new sub-category",
//             "Delete category (leaf only)",
//             "Back"
//         }, 4);

//         if (ch == 4) return;
//         clearScreen(); banner();

//         if (ch == 0) {
//             hdr("Full Category Tree");
//             std::cout << "\n";
//             printTree(inv.getTaxonomy().getRoot());
//             pause();
//         }
//         else if (ch == 1) {
//             hdr("List Children");
//             std::string path = inputLine("Path (leave blank for root): ");
//             for (auto& p : inv.getTaxonomy().listChildren(path))
//                 std::cout << "  " << CLR_CYAN << p << CLR_RESET "\n";
//             pause();
//         }
//         else if (ch == 2) {
//             hdr("Create Sub-Category");
//             std::string parent = inputLine("Parent path (blank = root): ");
//             std::string name   = inputLine("New category name: ");
//             auto* node = inv.getTaxonomy().createCategory(parent, name);
//             std::cout << CLR_GREEN "  ✓ Created: " << node->getFullPath() << CLR_RESET "\n";
//             pause();
//         }
//         else if (ch == 3) {
//             hdr("Delete Category");
//             std::string path = inputLine("Path to delete: ");
//             if (inv.getTaxonomy().deleteCategory(path))
//                 std::cout << CLR_GREEN "  ✓ Deleted.\n" CLR_RESET;
//             else
//                 std::cout << CLR_RED "  Not found or has children.\n" CLR_RESET;
//             pause();
//         }
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 3 – BOM IMPORT & CROSS-REFERENCE
// // ══════════════════════════════════════════════════════════════════════════════
// static void menuBOM(Inventory& inv) {
//     while (true) {
//         int ch = arrowMenu("BOM Import & Cross-Reference", {
//             "Import KiCad BOM CSV and cross-reference",
//             "Back"
//         }, 4);

//         if (ch == 1) return;

//         // ch == 0
//         clearScreen(); banner(); hdr("Import KiCad BOM CSV");

//         std::string fp = inputLine("BOM CSV file path: ");
//         std::string pn = inputLine("Project name (blank = from filename): ");

//         try {
//             BOMImporter importer;
//             BOM bom = importer.importKiCadCSV(fp, pn);
//             std::cout << CLR_GREEN "  ✓ Imported " << bom.size()
//                       << " line(s) for project: " << bom.getProjectName()
//                       << CLR_RESET "\n\n";

//             // Print BOM table
//             std::cout << CLR_BOLD
//                       << std::left
//                       << "  " << std::setw(30) << "Part"
//                       << std::setw(6)  << "Qty"
//                       << std::setw(14) << "Package"
//                       << std::setw(6)  << "Pins"
//                       << "Reference\n" CLR_RESET
//                       << "  " << std::string(72, '-') << "\n";
//             for (auto& e : bom.getEntries()) {
//                 std::cout << "  "
//                           << std::left
//                           << std::setw(30) << e.partNumber.substr(0, 29)
//                           << std::setw(6)  << e.quantity
//                           << std::setw(14) << e.packageType.substr(0, 13)
//                           << std::setw(6)  << e.pinCount
//                           << e.reference << "\n";
//             }

//             std::cout << "\n  Cross-referencing...\n\n";
//             BOMCrossReferencer xref(inv);
//             CrossReferenceResult res = xref.compare(bom);

//             // ── Matched ──────────────────────────────────────────────────
//             std::cout << CLR_GREEN "  ✓ MATCHED (" << res.matched.size() << "):\n" CLR_RESET;
//             for (auto& m : res.matched) {
//                 std::cout << "    " << CLR_BOLD << std::left << std::setw(28)
//                           << m.bomEntry.partNumber.substr(0, 27) << CLR_RESET
//                           << " → " << m.component->getId()
//                           << "  " << m.component->getName()
//                           << "  (have=" << m.component->getQuantity()
//                           << ", need=" << m.bomEntry.quantity << ")\n";
//             }

//             // ── Shortfall ─────────────────────────────────────────────────
//             std::cout << CLR_YELLOW "\n  ⚠ SHORTFALL (" << res.shortfall.size() << "):\n" CLR_RESET;
//             for (auto& m : res.shortfall) {
//                 std::cout << "    " << CLR_BOLD << std::left << std::setw(28)
//                           << m.bomEntry.partNumber.substr(0, 27) << CLR_RESET
//                           << " → " << m.component->getId()
//                           << "  deficit=" << CLR_YELLOW << m.deficit << CLR_RESET
//                           << "  (have=" << m.component->getQuantity()
//                           << ", need=" << m.bomEntry.quantity << ")\n";
//             }

//             // ── Missing ───────────────────────────────────────────────────
//             std::cout << CLR_RED "\n  ✗ MISSING (" << res.missing.size() << "):\n" CLR_RESET;
//             for (auto& e : res.missing) {
//                 std::cout << "    " << CLR_BOLD << std::left << std::setw(28)
//                           << e.partNumber.substr(0, 27) << CLR_RESET
//                           << "  qty=" << e.quantity
//                           << "  ref=" << e.reference << "\n";
//             }

//             // ── Summary ───────────────────────────────────────────────────
//             std::cout << "\n  " << std::string(60, '─') << "\n"
//                       << "  Matched:    " << CLR_GREEN << res.matched.size()   << CLR_RESET
//                       << "   Shortfall: " << CLR_YELLOW << res.shortfall.size() << CLR_RESET
//                       << "   Missing:   " << CLR_RED    << res.missing.size()   << CLR_RESET
//                       << "\n  Estimated cost of available stock: €"
//                       << CLR_BOLD << std::fixed << std::setprecision(2)
//                       << res.totalCost() << CLR_RESET;

//             if (g_settings.valueAwareBOM)
//                 std::cout << CLR_DIM "\n  [Value-aware matching active — unit/type safe]" CLR_RESET;
//             std::cout << "\n";
//         }
//         catch (std::exception& ex) {
//             std::cout << CLR_RED "  Error: " << ex.what() << CLR_RESET "\n";
//         }
//         pause();
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 4 – PARAMETRIC SEARCH
// // ══════════════════════════════════════════════════════════════════════════════
// static void menuSearch(Inventory& inv) {
//     ParametricSearch ps(inv);
//     while (true) {
//         int ch = arrowMenu("Parametric Search", {
//             "Quick name / ID search",
//             "Filter by type",
//             "Filter by package type",
//             "Filter by exact pin count",
//             "Filter by pin count range",
//             "Advanced: numeric parameter range",
//             "Compound search (combine filters)",
//             "Back"
//         }, 4);

//         if (ch == 7) return;

//         clearScreen(); banner();
//         SearchCriteria sc;

//         if (ch == 0) {
//             hdr("Quick Search");
//             sc.setNameFilter(inputLine("Search query: "));
//         }
//         else if (ch == 1) {
//             hdr("Filter by Type");
//             std::cout << "  Types: RESISTOR  CAPACITOR  INDUCTOR  MOSFET  BJT\n"
//                          "         DIODE     IC         CONNECTOR ARBITRARY\n\n";
//             sc.setTypeFilter(inputLine("Type name: "));
//         }
//         else if (ch == 2) {
//             hdr("Filter by Package");
//             std::cout << "  e.g.: 0402  0603  0805  1206  SOIC  MSOP  QFN  DIP  TO-220  SOT-23  THT\n\n";
//             sc.setPackageType(inputLine("Package type: "));
//         }
//         else if (ch == 3) {
//             hdr("Exact Pin Count");
//             sc.setPinCount(inputInt("Exact pin count", 2));
//         }
//         else if (ch == 4) {
//             hdr("Pin Count Range");
//             int lo = inputInt("Min pins",  2);
//             int hi = inputInt("Max pins", 32);
//             sc.setPinCountRange(lo, hi);
//         }
//         else if (ch == 5) {
//             hdr("Numeric Parameter Range");
//             std::cout << "  Param keys: resistance  capacitance  inductance\n"
//                          "              vgsThreshold  rdsOn  vceo  hfe\n"
//                          "              forwardVoltage  supplyVoltage  maxVoltage\n\n";
//             std::string param = inputLine("Parameter name: ");
//             double lo = inputDouble("Min value", 0.0);
//             double hi = inputDouble("Max value", 1e12);
//             sc.addParamRange(param, lo, hi);
//         }
//         else if (ch == 6) {
//             hdr("Compound Search");
//             std::cout << "  Leave any field blank to skip that filter.\n\n";
//             std::string nf = inputLine("Name filter: ");
//             if (!nf.empty()) sc.setNameFilter(nf);
//             std::string tf = inputLine("Type filter: ");
//             if (!tf.empty()) sc.setTypeFilter(tf);
//             std::string pf = inputLine("Package type filter: ");
//             if (!pf.empty()) sc.setPackageType(pf);
//             std::string pc_s = inputLine("Exact pin count (blank=skip): ");
//             if (!pc_s.empty()) { try { sc.setPinCount(std::stoi(pc_s)); } catch(...){} }
//             std::string cat = inputLine("Category filter: ");
//             if (!cat.empty()) sc.setCategory(cat);
//             std::string mp = inputLine("Max unit price € (blank=skip): ");
//             if (!mp.empty()) { try { sc.setMaxUnitPrice(std::stod(mp)); } catch(...){} }
//             std::string ls = inputLine("Low stock only? (y/N): ");
//             if (ls == "y" || ls == "Y") sc.onlyLowStock();
//         }

//         hdr("Results");
//         auto results = ps.execute(sc);
//         printCompList(results);
//         pause();
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 5 – PROJECT MANAGEMENT
// // ══════════════════════════════════════════════════════════════════════════════
// static void menuProjects(Inventory& inv, ProjectManager& pm) {
//     while (true) {
//         int ch = arrowMenu("Project Management", {
//             "List projects",
//             "Create project",
//             "Checkout components for project",
//             "Return components to inventory",
//             "View project transaction log",
//             "View project cost",
//             "Export transaction log to CSV",
//             "Restock component",
//             "Back"
//         }, 4);

//         if (ch == 8) return;
//         clearScreen(); banner();

//         if (ch == 0) {
//             hdr("Projects");
//             auto names = pm.listProjects();
//             if (names.empty()) { std::cout << "  (no projects)\n"; }
//             else {
//                 std::cout << CLR_BOLD "  " << std::left << std::setw(30) << "Name"
//                           << "Cost\n" CLR_RESET "  " << std::string(40, '-') << "\n";
//                 for (auto& n : names)
//                     std::cout << "  " << std::setw(30) << n
//                               << "€" << std::fixed << std::setprecision(2)
//                               << pm.getProjectCost(n) << "\n";
//             }
//             pause();
//         }
//         else if (ch == 1) {
//             hdr("Create Project");
//             std::string name = inputLine("Project name: ");
//             std::string desc = inputLine("Description: ");
//             pm.createProject(name, desc);
//             std::cout << CLR_GREEN "  ✓ Project '" << name << "' created.\n" CLR_RESET;
//             trySave(inv, pm);
//             pause();
//         }
//         else if (ch == 2) {
//             hdr("Checkout Components");
//             std::string proj = inputLine("Project name: ");
//             if (!pm.hasProject(proj)) {
//                 std::cout << CLR_RED "  Project not found.\n" CLR_RESET; pause(); continue;
//             }
//             std::string id  = inputLine("Component ID: ");
//             int         qty = inputInt("Quantity to checkout", 1);
//             if (pm.checkoutForProject(proj, id, qty)) {
//                 std::cout << CLR_GREEN "  ✓ Checked out " << qty << " × " << id << "\n" CLR_RESET;
//                 trySave(inv, pm);
//             } else {
//                 std::cout << CLR_RED "  Failed — insufficient stock or component not found.\n" CLR_RESET;
//             }
//             pause();
//         }
//         else if (ch == 3) {
//             hdr("Return Components");
//             std::string proj = inputLine("Project name: ");
//             std::string id   = inputLine("Component ID: ");
//             int         qty  = inputInt("Quantity to return", 1);
//             pm.returnToInventory(proj, id, qty);
//             std::cout << CLR_GREEN "  ✓ Returned.\n" CLR_RESET;
//             trySave(inv, pm);
//             pause();
//         }
//         else if (ch == 4) {
//             hdr("Transaction Log");
//             std::string proj = inputLine("Project name: ");
//             if (!pm.hasProject(proj)) { pause(); continue; }
//             std::cout << "\n";
//             for (auto& tx : pm.getProject(proj).getLog().getAll())
//                 std::cout << "  " << tx.toString() << "\n";
//             pause();
//         }
//         else if (ch == 5) {
//             hdr("Project Cost");
//             std::string proj = inputLine("Project name: ");
//             std::cout << "\n  Total cost for '" << proj << "': €"
//                       << CLR_BOLD << std::fixed << std::setprecision(4)
//                       << pm.getProjectCost(proj) << CLR_RESET "\n";
//             pause();
//         }
//         else if (ch == 6) {
//             hdr("Export Transaction Log");
//             std::string proj = inputLine("Project name: ");
//             std::string path = inputLine("Output CSV path: ");
//             if (pm.hasProject(proj)) {
//                 pm.getProject(proj).getLog().exportCSV(path);
//                 std::cout << CLR_GREEN "  ✓ Exported to " << path << "\n" CLR_RESET;
//             } else {
//                 std::cout << CLR_RED "  Project not found.\n" CLR_RESET;
//             }
//             pause();
//         }
//         else if (ch == 7) {
//             hdr("Restock Component");
//             std::string id  = inputLine("Component ID: ");
//             int         qty = inputInt("Quantity to add", 10);
//             pm.restockComponent(id, qty);
//             std::cout << CLR_GREEN "  ✓ Restocked " << qty << " × " << id << ".\n" CLR_RESET;
//             trySave(inv, pm);
//             pause();
//         }
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 6 – DATASHEET MANAGER
// // ══════════════════════════════════════════════════════════════════════════════
// static void menuDatasheet(Inventory& inv) {
//     DatasheetManager dm(inv);
//     while (true) {
//         int ch = arrowMenu("Datasheet Manager", {
//             "Open datasheet for component",
//             "Set / update datasheet path or URL",
//             "Show datasheet path",
//             "Back"
//         }, 4);

//         if (ch == 3) return;
//         clearScreen(); banner();

//         std::string id = inputLine("Component ID: ");

//         if (ch == 0) {
//             hdr("Open Datasheet");
//             dm.openDatasheet(id);
//         }
//         else if (ch == 1) {
//             hdr("Set Datasheet");
//             std::string path = inputLine("Datasheet path or URL: ");
//             dm.setDatasheet(id, path);
//             std::cout << CLR_GREEN "  ✓ Set.\n" CLR_RESET;
//         }
//         else if (ch == 2) {
//             hdr("Datasheet Path");
//             std::cout << "  " << dm.getDatasheet(id) << "\n";
//         }
//         pause();
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 7 – LOW STOCK ALERTS
// // ══════════════════════════════════════════════════════════════════════════════
// static void menuLowStock(Inventory& inv) {
//     while (true) {
//         int ch = arrowMenu("Low Stock Alerts", {
//             "Show low stock report",
//             "Export low stock report to CSV",
//             "Back"
//         }, 4);

//         if (ch == 2) return;
//         clearScreen(); banner();

//         if (ch == 0) {
//             hdr("Low Stock Report");
//             std::cout << "\n" << LowStockAlert::generateReport(inv);
//         }
//         else if (ch == 1) {
//             hdr("Export Low Stock CSV");
//             std::string path = inputLine("Output CSV path: ");
//             if (LowStockAlert::exportReport(inv, path))
//                 std::cout << CLR_GREEN "  ✓ Exported to " << path << "\n" CLR_RESET;
//             else
//                 std::cout << CLR_RED "  Failed to write file.\n" CLR_RESET;
//         }
//         pause();
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MODULE 8 – SETTINGS
// // ══════════════════════════════════════════════════════════════════════════════
// static void menuSettings() {
//     while (true) {
//         // Build display strings
//         auto saveModeStr = [&]() -> std::string {
//             switch (g_settings.saveMode) {
//                 case SaveMode::AUTO:     return CLR_SETTINGON  "Auto-save after every change" CLR_RESET;
//                 case SaveMode::PROMPTED: return CLR_SETTINGON  "Prompt to save on exit"       CLR_RESET;
//                 default:                 return CLR_SETTINGOFF "Manual (Save to disk menu)"   CLR_RESET;
//             }
//         };
//         auto yn = [](bool v) -> std::string {
//             return v ? CLR_SETTINGON  "Yes" CLR_RESET
//                      : CLR_SETTINGOFF "No"  CLR_RESET;
//         };

//         int ch = arrowMenu("Settings  [Enter to toggle / change]", {
//             std::string("Save mode           : ") + saveModeStr(),
//             std::string("Confirm before delete: ") + yn(g_settings.confirmDelete),
//             std::string("BOM value-aware match: ") + yn(g_settings.valueAwareBOM),
//             std::string("BOM type warnings    : ") + yn(g_settings.bomTypeWarnings),
//             std::string("Alternating row tint : ") + yn(g_settings.colorTableRows),
//             std::string("Data directory       : ") + CLR_CYAN + g_dataDir + CLR_RESET,
//             "Back"
//         }, 4);

//         if (ch == 6) { saveSettings(); return; }
//         clearScreen(); banner();

//         switch (ch) {
//             case 0:  // Cycle save mode
//                 switch (g_settings.saveMode) {
//                     case SaveMode::MANUAL:   g_settings.saveMode = SaveMode::AUTO;     break;
//                     case SaveMode::AUTO:     g_settings.saveMode = SaveMode::PROMPTED; break;
//                     case SaveMode::PROMPTED: g_settings.saveMode = SaveMode::MANUAL;   break;
//                 }
//                 break;
//             case 1: g_settings.confirmDelete   = !g_settings.confirmDelete;   break;
//             case 2: g_settings.valueAwareBOM   = !g_settings.valueAwareBOM;   break;
//             case 3: g_settings.bomTypeWarnings = !g_settings.bomTypeWarnings; break;
//             case 4: g_settings.colorTableRows  = !g_settings.colorTableRows;  break;
//             case 5: {
//                 hdr("Data Directory");
//                 std::string nd = inputLine("New data directory [" + g_dataDir + "]: ");
//                 if (!nd.empty()) g_dataDir = nd;
//                 break;
//             }
//         }
//         saveSettings();
//     }
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // SAMPLE DATA
// // ══════════════════════════════════════════════════════════════════════════════
// static void loadSampleData(Inventory& inv) {
//     // Resistors
//     inv.addComponent(new Resistor("RES0001","1k Resistor",       100,1000.0,  1.0,0.25,"DIN0207",5,0.02,"Passives/Resistors/THT","","Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",2));
//     inv.addComponent(new Resistor("RES0002","47k Resistor",       50,47000.0, 1.0,0.25,"DIN0207",5,0.02,"Passives/Resistors/THT","","Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",2));
//     inv.addComponent(new Resistor("RES0003","4.7k Resistor",      80,4700.0,  1.0,0.25,"DIN0207",5,0.02,"Passives/Resistors/THT","","Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",2));
//     inv.addComponent(new Resistor("RES0004","10k SMD 0603",      200,10000.0, 1.0,0.10,"0603",  10,0.01,"Passives/Resistors/SMD","","Resistor_SMD:R_0603_1608Metric",2));
//     inv.addComponent(new Resistor("RES0005","100R SMD 0402",      50,100.0,   5.0,0.063,"0402", 10,0.008,"Passives/Resistors/SMD","","Resistor_SMD:R_0402_1005Metric",2));
//     // Capacitors
//     inv.addComponent(new Capacitor("CAP0001","4.7nF Disc",        10,4.7e-9, "ceramic",   10.0,100.0,"Disc", 2,0.05,"Passives/Capacitors/Ceramic","","Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm",2));
//     inv.addComponent(new Capacitor("CAP0002","1000uF Electro",     3,1000e-6,"electrolytic",20.0, 35.0,"Radial",2,0.30,"Passives/Capacitors/Electrolytic","","Capacitor_THT:CP_Radial_D16.0mm_P7.50mm",2));
//     inv.addComponent(new Capacitor("CAP0003","100nF SMD 0603",   500,100e-9, "ceramic",   10.0, 25.0,"0603",10,0.015,"Passives/Capacitors/Ceramic","","Capacitor_SMD:C_0603_1608Metric",2));
//     inv.addComponent(new Capacitor("CAP0004","10uF SMD 0805",    100,10e-6,  "ceramic",   20.0, 10.0,"0805",10,0.05,"Passives/Capacitors/Ceramic","","Capacitor_SMD:C_0805_2012Metric",2));
//     // Inductors
//     inv.addComponent(new Inductor("IND0001","33uH Toroid THT",     5,33e-6, 3.0, 40.0,"Toroid",2,0.80,"Passives/Inductors/THT","","Inductor_THT:L_Toroid_Vertical_L13.0mm_W6.5mm_P5.60mm",2));
//     inv.addComponent(new Inductor("IND0002","10uH SMD",           20,10e-6, 1.5, 0.0, "1210",  5,0.25,"Passives/Inductors/SMD","","",2));
//     // Diodes
//     inv.addComponent(new DiodeComponent("DIO0001","1N5822 Schottky", 8,0.45, 40.0,"schottky",  3.0,"DO-201AD",2,0.35,"Semiconductors/Diodes/Schottky","http://www.vishay.com/docs/88526/1n5820.pdf","Diode_THT:D_DO-201AD_P15.24mm_Horizontal","THT-DO201",2));
//     inv.addComponent(new DiodeComponent("DIO0002","1N4007 Rectifier",50,0.7,1000.0,"rectifier",1.0,"DO-41",   5,0.05,"Semiconductors/Diodes/Rectifier","","","DO-41",2));
//     inv.addComponent(new DiodeComponent("DIO0003","BZX55C5V1 Zener",30,5.1,  5.1,"zener",     0.2,"DO-35",   5,0.08,"Semiconductors/Diodes/Zener","","","DO-35",2));
//     // ICs
//     inv.addComponent(new IntegratedCircuit("IC_0001","LM2596T-ADJ",  2,"Step-down switching regulator",3.0,40.0,3.0,"TO-220-5",5,2,2.50,"Semiconductors/ICs/Power/DCDC","http://www.ti.com/lit/ds/symlink/lm2596.pdf","Package_TO_SOT_THT:TO-220-5_P3.4x3.7mm_StaggerOdd_Lead3.8mm_Vertical","TO-220"));
//     inv.addComponent(new IntegratedCircuit("IC_0002","LT1764AET#PBF",3,"Low-dropout regulator 3A",1.5,20.0,3.0,"TO-263-5",5,2,3.80,"Semiconductors/ICs/Power/LDO","https://www.analog.com/media/en/technical-documentation/data-sheets/1764f.pdf","","TO-263"));
//     inv.addComponent(new IntegratedCircuit("IC_0003","LTC4365",       1,"OV/UV/Reverse Protection",2.5,18.0,0.01,"MSOP-10",10,2,4.20,"Semiconductors/ICs/Protection","https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4365.pdf","","MSOP"));
//     inv.addComponent(new IntegratedCircuit("IC_0004","ESP32-WROOM-32",1,"WiFi+BT SoC",3.3,3.6,0.5,"Module",38,2,5.00,"Semiconductors/ICs/Microcontrollers","","","Module"));
//     // BJTs
//     inv.addComponent(new BJTTransistor("BJT0001","BC547",20,"NPN",200,45,0.1,"TO-92",5,0.08,"Semiconductors/Transistors/BJT/NPN","","","TO-92",3));
//     inv.addComponent(new BJTTransistor("BJT0002","BC557",10,"PNP",200,45,0.1,"TO-92",5,0.08,"Semiconductors/Transistors/BJT/PNP","","","TO-92",3));
//     // MOSFETs
//     inv.addComponent(new MOSFETTransistor("MOS0001","IRLZ44N",5,"N-CH",2.0,0.022,48.0,55.0,47.0,"TO-220",2,0.65,"Semiconductors/Transistors/MOSFET/N-Channel","","Package_TO_SOT_THT:TO-220-3_P3.4mm_StaggerOdd_Vertical","TO-220",3));
//     inv.addComponent(new MOSFETTransistor("MOS0002","AO4407",5,"P-CH",1.8,0.018,30.0,30.0,12.0,"SOP-8",2,0.90,"Semiconductors/Transistors/MOSFET/P-Channel","","","SOP",3));
//     // Connectors
//     inv.addComponent(new ConnectorComponent("CON0001","PinHeader 1x08 2.54mm",4,"PinHeader",2.54,"male",8,2,0.15,"Connectors/PinHeaders","","Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical","PinHeader"));
//     inv.addComponent(new ConnectorComponent("CON0002","PinHeader 1x02 2.54mm",10,"PinHeader",2.54,"male",2,2,0.08,"Connectors/PinHeaders","","Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical","PinHeader"));
//     inv.addComponent(new ConnectorComponent("CON0003","Screw Terminal 1x02",6,"TerminalBlock",5.08,"female",2,2,0.35,"Connectors/ScrewTerminals","","TerminalBlock_MetzConnect:TerminalBlock_MetzConnect_Type101_RT01602HBWC_1x02_P5.08mm_Horizontal","TerminalBlock"));
//     // Arbitrary
//     auto* fuse = new ArbitraryComponent("ARB0001","Inline Fuse 2A",2,"Glass fuse 5x20mm",3,0.40,"Passives/Fuses","","","THT",2);
//     fuse->addParam("rating_A","2"); fuse->addParam("size_mm","5x20"); fuse->addParam("type","fast-blow");
//     inv.addComponent(fuse);
//     auto* pot = new ArbitraryComponent("ARB0002","Potentiometer 10k",1,"Bourns 3296W trim pot",2,0.85,"Passives/Potentiometers","","Potentiometer_THT:Potentiometer_Bourns_3296W_Vertical","THT",3);
//     pot->addParam("resistance_Ohm","10000"); pot->addParam("turns","25"); pot->addParam("power_W","0.5");
//     inv.addComponent(pot);

//     std::cout << CLR_GREEN "  ✓ Sample data loaded (" << inv.size() << " components)\n" CLR_RESET;
// }

// // ══════════════════════════════════════════════════════════════════════════════
// // MAIN
// // ══════════════════════════════════════════════════════════════════════════════
// int main() {
//     SetConsoleOutputCP(CP_UTF8);
//     SetConsoleCP(CP_UTF8);
//     enableANSI();

//     loadSettings();

//     Inventory      inv;
//     ProjectManager pm(inv);

//     // ── Boot menu ────────────────────────────────────────────────────────────
//     int boot = arrowMenu(
//         "Welcome to ECIMS"
//         "\n\n  Data directory: " + g_dataDir,
//         { "Load saved data",
//           "Load sample data (demonstration)",
//           "Start empty" },
//         4);

//     if (boot == 0) {
//         clearScreen(); banner();
//         if (Persistence::loadAll(inv, pm, g_dataDir))
//             std::cout << CLR_GREEN "  ✓ Data loaded. " << inv.size() << " component(s).\n" CLR_RESET;
//         else
//             std::cout << CLR_YELLOW "  No saved data found — starting empty.\n" CLR_RESET;
//         pause();
//     }
//     else if (boot == 1) {
//         clearScreen(); banner();
//         loadSampleData(inv);
//         pause();
//     }

//     // ── Main loop ─────────────────────────────────────────────────────────────
//     while (true) {
//         auto saveModeTag = [&]() -> std::string {
//             switch (g_settings.saveMode) {
//                 case SaveMode::AUTO:     return CLR_GREEN  " [auto]"    CLR_RESET;
//                 case SaveMode::PROMPTED: return CLR_YELLOW " [prompted]" CLR_RESET;
//                 default:                 return CLR_DIM    " [manual]"   CLR_RESET;
//             }
//         };

//         int ch = arrowMenu(
//             "Main Menu"
//             "\n\n  Inventory: " + std::to_string(inv.size()) + " components"
//             "   |   Low stock: " + std::to_string(inv.getLowStock().size()),
//             {
//                 "Inventory (CRUD)",
//                 "Category Browser",
//                 "BOM Import & Cross-Reference",
//                 "Parametric Search",
//                 "Project Management",
//                 "Datasheet Manager",
//                 "Low Stock Alerts",
//                 std::string("Save to disk") + saveModeTag(),
//                 "Load from disk",
//                 "Settings",
//                 "Exit"
//             }, 4);

//         if      (ch == 0)  menuInventory(inv, pm);
//         else if (ch == 1)  menuTaxonomy(inv, pm);
//         else if (ch == 2)  menuBOM(inv);
//         else if (ch == 3)  menuSearch(inv);
//         else if (ch == 4)  menuProjects(inv, pm);
//         else if (ch == 5)  menuDatasheet(inv);
//         else if (ch == 6)  menuLowStock(inv);
//         else if (ch == 7) {
//             clearScreen(); banner(); hdr("Save to Disk");
//             if (Persistence::saveAll(inv, pm, g_dataDir))
//                 std::cout << CLR_GREEN "\n  ✓ Saved to '" << g_dataDir << "/'\n" CLR_RESET;
//             else
//                 std::cout << CLR_RED "\n  Save failed.\n" CLR_RESET;
//             pause();
//         }
//         else if (ch == 8) {
//             clearScreen(); banner(); hdr("Load from Disk");
//             if (Persistence::loadAll(inv, pm, g_dataDir))
//                 std::cout << CLR_GREEN "\n  ✓ Loaded. " << inv.size() << " component(s).\n" CLR_RESET;
//             else
//                 std::cout << CLR_RED "\n  Load failed.\n" CLR_RESET;
//             pause();
//         }
//         else if (ch == 9) { menuSettings(); }
//         else if (ch == 10) {
//             // Exit — handle prompted-save mode
//             if (g_settings.saveMode == SaveMode::PROMPTED) {
//                 int ans = arrowMenu("Save before exiting?", {
//                     "Yes — save and exit",
//                     "No  — exit without saving",
//                     "Cancel — go back"
//                 }, 4);
//                 if (ans == 0) {
//                     Persistence::saveAll(inv, pm, g_dataDir);
//                     break;
//                 }
//                 else if (ans == 1) { break; }
//                 // ans == 2 → go back to main menu
//             }
//             else { break; }
//         }
//     }

//     std::cout << CLR_BOLD CLR_BLUE "\n  Goodbye.\n\n" CLR_RESET;
//     return 0;
// }


























































































/*  ECIMS — Electronic Component Inventory Management System
    Console UI  */

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
#include <fstream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>

// ── ANSI colours ──────────────────────────────────────────────────────────────
#ifdef _WIN32
#  define CLR_RESET   "\x1b[0m"
#  define CLR_BOLD    "\x1b[1m"
#  define CLR_DIM     "\x1b[2m"
#  define CLR_CYAN    "\x1b[36m"
#  define CLR_GREEN   "\x1b[32m"
#  define CLR_YELLOW  "\x1b[33m"
#  define CLR_RED     "\x1b[31m"
#  define CLR_BLUE    "\x1b[34m"
#  define CLR_MAG     "\x1b[35m"
#  define CLR_WHITE   "\x1b[37m"
#  define CLR_SELECT  "\x1b[30;47m"
#  define CLR_SETTINGON  "\x1b[32m"   // green  = active / enabled
#  define CLR_SETTINGOFF "\x1b[90m"   // dark   = inactive / disabled
#else
#  define CLR_RESET   "\033[0m"
#  define CLR_BOLD    "\033[1m"
#  define CLR_DIM     "\033[2m"
#  define CLR_CYAN    "\033[36m"
#  define CLR_GREEN   "\033[32m"
#  define CLR_YELLOW  "\033[33m"
#  define CLR_RED     "\033[31m"
#  define CLR_BLUE    "\033[34m"
#  define CLR_MAG     "\033[35m"
#  define CLR_WHITE   "\033[37m"
#  define CLR_SELECT  "\033[30;47m"
#  define CLR_SETTINGON  "\033[32m"
#  define CLR_SETTINGOFF "\033[90m"
#endif

// ── Global settings ───────────────────────────────────────────────────────────
enum class SaveMode { MANUAL, AUTO, PROMPTED };

struct AppSettings {
    SaveMode    saveMode          = SaveMode::MANUAL;
    bool        confirmDelete     = true;
    bool        valueAwareBOM     = true;   // use type/unit-aware BOM matching
    bool        bomTypeWarnings   = true;   // annotate BOM results with type hints
    bool        colorTableRows    = true;   // alternate row tinting in component lists
};

static AppSettings  g_settings;
static std::string  g_dataDir        = "data";
static const char*  SETTINGS_FILE    = "ecims_settings.ini";

// ── Settings persistence ──────────────────────────────────────────────────────
static void saveSettings() {
    std::ofstream f(SETTINGS_FILE);
    if (!f) return;
    f << "saveMode="        << (int)g_settings.saveMode        << "\n"
      << "confirmDelete="   << (int)g_settings.confirmDelete   << "\n"
      << "valueAwareBOM="   << (int)g_settings.valueAwareBOM   << "\n"
      << "bomTypeWarnings=" << (int)g_settings.bomTypeWarnings << "\n"
      << "colorTableRows="  << (int)g_settings.colorTableRows  << "\n"
      << "dataDir="         << g_dataDir                       << "\n";
}

static void loadSettings() {
    std::ifstream f(SETTINGS_FILE);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        try {
            if      (key == "saveMode")        g_settings.saveMode        = (SaveMode)std::stoi(val);
            else if (key == "confirmDelete")   g_settings.confirmDelete   = (val == "1");
            else if (key == "valueAwareBOM")   g_settings.valueAwareBOM   = (val == "1");
            else if (key == "bomTypeWarnings") g_settings.bomTypeWarnings = (val == "1");
            else if (key == "colorTableRows")  g_settings.colorTableRows  = (val == "1");
            else if (key == "dataDir" && !val.empty()) g_dataDir          = val;
        } catch (...) {}
    }
}

// ── Forward declarations ──────────────────────────────────────────────────────
static void menuInventory(Inventory& inv, ProjectManager& pm);
static void menuTaxonomy(Inventory& inv, ProjectManager& pm);
static void menuBOM(Inventory& inv);
static void menuSearch(Inventory& inv);
static void menuProjects(Inventory& inv, ProjectManager& pm);
static void menuDatasheet(Inventory& inv);
static void menuLowStock(Inventory& inv);
static void menuSettings();

// ── I/O helpers ───────────────────────────────────────────────────────────────
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void enableANSI() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

static void pause() {
    std::cout << "\n" << CLR_YELLOW "  [Press ENTER]" CLR_RESET;
    while (_getch() != 13) {}
}

static std::string inputLine(const std::string& prompt) {
    std::cout << CLR_CYAN "  " << prompt << CLR_RESET;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static int inputInt(const std::string& prompt, int def = 0) {
    std::cout << CLR_CYAN "  " << prompt << " [" << def << "]: " CLR_RESET;
    std::string s;
    std::getline(std::cin, s);
    if (s.empty()) return def;
    try { return std::stoi(s); } catch (...) { return def; }
}

static double inputDouble(const std::string& prompt, double def = 0.0) {
    std::cout << CLR_CYAN "  " << prompt << " [" << def << "]: " CLR_RESET;
    std::string s;
    std::getline(std::cin, s);
    if (s.empty()) return def;
    try { return std::stod(s); } catch (...) { return def; }
}

// ── Banner / header ───────────────────────────────────────────────────────────
static void banner() {
    std::cout << CLR_BOLD CLR_BLUE
              << "\n╔══════════════════════════════════════════════════════╗\n"
                 "║   ECIMS — Electronic Component Inventory Manager     ║\n"
                 "╚══════════════════════════════════════════════════════╝\n"
              << CLR_RESET;
}

static void hdr(const std::string& title) {
    std::cout << CLR_BOLD CLR_CYAN
              << "\n  ── " << title << " ──\n"
              << CLR_RESET;
}

// ── Arrow-navigable menu ──────────────────────────────────────────────────────
// Returns 0-based index of the selected item.
static int arrowMenu(
    const std::string&              title,
    const std::vector<std::string>& items,
    int                             indent  = 2,
    int                             startAt = 0)
{
    int selected = startAt;
    if (selected < 0 || selected >= (int)items.size()) selected = 0;

    while (true) {
        clearScreen();
        banner();
        hdr(title);
        std::cout << "\n";

        for (int i = 0; i < (int)items.size(); ++i) {
            std::cout << std::string(indent, ' ');
            if (i == selected)
                std::cout << CLR_SELECT "  > " << items[i] << "  " CLR_RESET;
            else
                std::cout << "    " << items[i];
            std::cout << "\n\n";
        }

        std::cout << CLR_DIM "  ↑↓ navigate   Enter select" CLR_RESET "\n";

        int ch = _getch();
        if (ch == 224) {
            ch = _getch();
            if      (ch == 72) { if (--selected < 0) selected = (int)items.size() - 1; }
            else if (ch == 80) { if (++selected >= (int)items.size()) selected = 0; }
        } else if (ch == 13) {
            return selected;
        }
    }
}

// ── Auto-save helper ──────────────────────────────────────────────────────────
static void trySave(Inventory& inv, ProjectManager& pm) {
    if (g_settings.saveMode == SaveMode::AUTO) {
        bool ok = Persistence::saveAll(inv, pm, g_dataDir);
        std::cout << (ok
            ? CLR_DIM "  ✓ Auto-saved.\n" CLR_RESET
            : CLR_RED "  Auto-save failed!\n" CLR_RESET);
    }
}

// ── Component list printer ────────────────────────────────────────────────────
static void printCompList(const std::vector<Component*>& list) {
    if (list.empty()) { std::cout << "  (none)\n"; return; }

    std::cout << CLR_BOLD
              << std::left
              << "  " << std::setw(10) << "ID"
              << std::setw(28) << "Name"
              << std::setw(12) << "Type"
              << std::setw(8)  << "Qty"
              << std::setw(8)  << "Min"
              << std::setw(8)  << "Pins"
              << std::setw(10) << "Package"
              << "Price\n"
              << CLR_RESET
              << "  " << std::string(90, '-') << "\n";

    for (int i = 0; i < (int)list.size(); ++i) {
        auto* c = list[i];
        bool  low = c->isLowStock();

        // Optional alternating row tint
        if (g_settings.colorTableRows && (i % 2 == 1))
            std::cout << CLR_DIM;

        std::cout << "  "
                  << std::left
                  << std::setw(10) << c->getId()
                  << std::setw(28) << c->getName().substr(0, 27)
                  << std::setw(12) << c->getTypeName().substr(0, 11)
                  << std::setw(8)  << c->getQuantity()
                  << std::setw(8)  << c->getMinStock()
                  << std::setw(8)  << c->getPinCount()
                  << std::setw(10) << c->getPackageType().substr(0, 9)
                  << "€" << std::fixed << std::setprecision(4) << c->getUnitPrice();

        if (g_settings.colorTableRows && (i % 2 == 1)) std::cout << CLR_RESET;
        if (low) std::cout << CLR_RED " [LOW]" CLR_RESET;
        std::cout << "\n";
    }
    std::cout << "  " << list.size() << " component(s)\n";
}

// ── ID generator ──────────────────────────────────────────────────────────────
static std::string genId(Inventory& inv, const std::string& prefix) {
    static int ctr = 1;
    std::ostringstream oss;
    do {
        oss.str("");
        oss << prefix << std::setw(4) << std::setfill('0') << ctr++;
    } while (inv.getComponent(oss.str()));
    return oss.str();
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 1 – INVENTORY CRUD
// ══════════════════════════════════════════════════════════════════════════════
static void moduleAddComponent(Inventory& inv, ProjectManager& pm) {
    clearScreen(); banner();
    hdr("Add Component — Select Type");

    int t = arrowMenu("Component type", {
        "Resistor",
        "Capacitor",
        "Inductor",
        "MOSFET Transistor",
        "BJT Transistor",
        "Diode",
        "Integrated Circuit (IC)",
        "Connector",
        "Arbitrary / Other",
        "Cancel"
    }, 4);

    if (t == 9) return;   // Cancel
    t += 1;               // 1-based from here

    clearScreen(); banner();
    hdr("Add Component");

    std::string name = inputLine("Name (e.g. 10k, LM2596T-ADJ): ");
    if (name.empty()) { std::cout << CLR_YELLOW "  Cancelled.\n" CLR_RESET; pause(); return; }

    int    qty = inputInt   ("Quantity",              10);
    int    mns = inputInt   ("Min stock threshold",    2);
    double prc = inputDouble("Unit price (€)",       0.0);

    std::string fp  = inputLine("Footprint (KiCad format or blank): ");
    std::string pkg = FootprintParser::extractPackageType(fp);
    int         pc  = FootprintParser::extractPinCount(fp);
    if (pkg.empty()) pkg = inputLine("Package type (e.g. 0603, SOIC, THT): ");
    if (pc  == 0  ) pc  = inputInt ("Pin count", 2);

    std::string ds  = inputLine("Datasheet path/URL (blank to skip): ");

    Component* c = nullptr;

    if (t == 1) {   // RESISTOR
        std::string cat = inputLine("Category [Passives/Resistors/SMD]: ");
        if (cat.empty()) cat = "Passives/Resistors/SMD";
        double res = inputDouble("Resistance (Ω, e.g. 10000 or 10e3)", 1000.0);
        double tol = inputDouble("Tolerance (%)", 1.0);
        double pwr = inputDouble("Power rating (W)", 0.1);
        c = new Resistor(genId(inv, "RES"), name, qty, res, tol, pwr,
                         pkg, mns, prc, cat, ds, fp, pc);
    }
    else if (t == 2) {   // CAPACITOR
        std::string cat = inputLine("Category [Passives/Capacitors/Ceramic]: ");
        if (cat.empty()) cat = "Passives/Capacitors/Ceramic";
        double cap = inputDouble("Capacitance (F, e.g. 100e-9 for 100 nF)", 100e-9);
        std::string ct = inputLine("Dielectric (ceramic/electrolytic/tantalum/film) [ceramic]: ");
        if (ct.empty()) ct = "ceramic";
        double tol = inputDouble("Tolerance (%)", 10.0);
        double vr  = inputDouble("Voltage rating (V)", 10.0);
        c = new Capacitor(genId(inv, "CAP"), name, qty, cap, ct, tol, vr,
                          pkg, mns, prc, cat, ds, fp, pc);
    }
    else if (t == 3) {   // INDUCTOR
        std::string cat = inputLine("Category [Passives/Inductors/THT]: ");
        if (cat.empty()) cat = "Passives/Inductors/THT";
        double ind  = inputDouble("Inductance (H, e.g. 33e-6 for 33 µH)", 33e-6);
        double irat = inputDouble("Current rating (A)", 1.0);
        double dcr  = inputDouble("DCR / series resistance (Ω)", 0.0);
        c = new Inductor(genId(inv, "IND"), name, qty, ind, irat, dcr,
                         pkg, mns, prc, cat, ds, fp, pc);
    }
    else if (t == 4) {   // MOSFET
        std::string cat = inputLine("Category [Semiconductors/Transistors/MOSFET/N-Channel]: ");
        if (cat.empty()) cat = "Semiconductors/Transistors/MOSFET/N-Channel";
        std::string tt  = inputLine("Type (N-CH / P-CH) [N-CH]: ");
        if (tt.empty()) tt = "N-CH";
        double vgs = inputDouble("Vgs(th) (V)", 2.0);
        double rds = inputDouble("Rds(on) (Ω)", 0.01);
        double qg  = inputDouble("Gate charge (nC)", 10.0);
        double mv  = inputDouble("Max Vds (V)", 30.0);
        double mc  = inputDouble("Max Id (A)", 5.0);
        c = new MOSFETTransistor(genId(inv, "MOS"), name, qty, tt, vgs, rds, qg,
                                 mv, mc, pkg, mns, prc, cat, ds, fp, pkg, pc);
    }
    else if (t == 5) {   // BJT
        std::string cat = inputLine("Category [Semiconductors/Transistors/BJT/NPN]: ");
        if (cat.empty()) cat = "Semiconductors/Transistors/BJT/NPN";
        std::string tt  = inputLine("Type (NPN / PNP) [NPN]: ");
        if (tt.empty()) tt = "NPN";
        double hfe  = inputDouble("hFE (gain)", 100.0);
        double vceo = inputDouble("Vceo (V)", 40.0);
        double icm  = inputDouble("Ic max (A)", 0.1);
        c = new BJTTransistor(genId(inv, "BJT"), name, qty, tt, hfe, vceo, icm,
                              pkg, mns, prc, cat, ds, fp, pkg, pc);
    }
    else if (t == 6) {   // DIODE
        std::string cat = inputLine("Category [Semiconductors/Diodes/Rectifier]: ");
        if (cat.empty()) cat = "Semiconductors/Diodes/Rectifier";
        double vf  = inputDouble("Forward voltage Vf (V)", 0.7);
        double vrm = inputDouble("Reverse voltage VRM (V)", 40.0);
        std::string dt = inputLine("Type (rectifier/schottky/zener/TVS/LED) [rectifier]: ");
        if (dt.empty()) dt = "rectifier";
        double imax = inputDouble("If max (A)", 1.0);
        c = new DiodeComponent(genId(inv, "DIO"), name, qty, vf, vrm, dt, imax,
                               pkg, mns, prc, cat, ds, fp, pkg, pc);
    }
    else if (t == 7) {   // IC
        std::string cat = inputLine("Category [Semiconductors/ICs/Power/DCDC]: ");
        if (cat.empty()) cat = "Semiconductors/ICs/Power/DCDC";
        std::string func = inputLine("Function description: ");
        double sv = inputDouble("Supply voltage (V)", 3.3);
        double mv = inputDouble("Max voltage (V)", 40.0);
        double mc = inputDouble("Max current (A)", 0.01);
        c = new IntegratedCircuit(genId(inv, "IC_"), name, qty, func, sv, mv, mc,
                                  pkg, pc, mns, prc, cat, ds, fp, pkg);
    }
    else if (t == 8) {   // CONNECTOR
        std::string cat = inputLine("Category [Connectors/PinHeaders]: ");
        if (cat.empty()) cat = "Connectors/PinHeaders";
        std::string ct    = inputLine("Connector type (PinHeader/JST/Molex/USB-C/SMA): ");
        double      pitch = inputDouble("Pitch (mm)", 2.54);
        std::string gen   = inputLine("Gender (male/female) [male]: ");
        if (gen.empty()) gen = "male";
        c = new ConnectorComponent(genId(inv, "CON"), name, qty, ct, pitch, gen,
                                   pc, mns, prc, cat, ds, fp, pkg);
    }
    else {   // ARBITRARY
        std::string cat  = inputLine("Category [Other]: ");
        if (cat.empty()) cat = "Other";
        std::string desc = inputLine("Description: ");
        auto* arb = new ArbitraryComponent(genId(inv, "ARB"), name, qty, desc,
                                           mns, prc, cat, ds, fp, pkg, pc);
        std::cout << "  Add custom parameters (key=value, blank to stop):\n";
        while (true) {
            std::string kv = inputLine("  param (key=value): ");
            if (kv.empty()) break;
            auto eq = kv.find('=');
            if (eq != std::string::npos)
                arb->addParam(kv.substr(0, eq), kv.substr(eq + 1));
        }
        c = arb;
    }

    if (c) {
        inv.addComponent(c);
        std::cout << CLR_GREEN "\n  ✓ Added: " << c->getId()
                  << " — " << c->getName() << "\n" CLR_RESET;
        trySave(inv, pm);
    }
    pause();
}

static void moduleEditComponent(Inventory& inv, ProjectManager& pm) {
    clearScreen(); banner(); hdr("Edit Component");
    std::string id = inputLine("Component ID to edit: ");
    Component* c = inv.getComponent(id);
    if (!c) {
        std::cout << CLR_RED "  Not found.\n" CLR_RESET; pause(); return;
    }
    c->display();

    int ch = arrowMenu("Field to edit", {
        "Name",
        "Quantity",
        "Min stock threshold",
        "Unit price",
        "Category path",
        "Datasheet path / URL",
        "Package type",
        "Pin count",
        "Cancel"
    }, 4);

    clearScreen(); banner(); hdr("Edit: " + c->getName());

    bool changed = true;
    switch (ch) {
        case 0: c->setName      (inputLine  ("New name: "));            break;
        case 1: c->setQuantity  (inputInt   ("New quantity"));          break;
        case 2: c->setMinStock  (inputInt   ("New min stock"));         break;
        case 3: c->setUnitPrice (inputDouble("New unit price (€)"));    break;
        case 4: c->setCategoryPath(inputLine("New category path: "));   break;
        case 5: c->setDatasheetPath(inputLine("New datasheet path/URL: ")); break;
        case 6: c->setPackageType(inputLine ("New package type: "));    break;
        case 7: c->setPinCount  (inputInt   ("New pin count"));         break;
        default: std::cout << "  Cancelled.\n"; changed = false; break;
    }

    if (changed) {
        std::cout << CLR_GREEN "  ✓ Updated.\n" CLR_RESET;
        trySave(inv, pm);
    }
    pause();
}

static void moduleDeleteComponent(Inventory& inv, ProjectManager& pm) {
    clearScreen(); banner(); hdr("Delete Component");
    std::string id = inputLine("Component ID to delete: ");
    Component* c = inv.getComponent(id);
    if (!c) {
        std::cout << CLR_RED "  Not found.\n" CLR_RESET; pause(); return;
    }
    std::cout << "\n  Component: " << CLR_BOLD << c->getName() << CLR_RESET
              << "  (" << c->getId() << ")\n";

    if (g_settings.confirmDelete) {
        int ans = arrowMenu("Confirm deletion", {
            "Yes — delete permanently",
            "No  — cancel"
        }, 4);
        if (ans != 0) { std::cout << "  Cancelled.\n"; pause(); return; }
    }

    inv.removeComponent(id);
    std::cout << CLR_GREEN "  ✓ Deleted.\n" CLR_RESET;
    trySave(inv, pm);
    pause();
}

// ── Inventory menu ─────────────────────────────────────────────────────────────
static void menuInventory(Inventory& inv, ProjectManager& pm) {
    while (true) {
        int ch = arrowMenu(
            "Inventory Management"
            "\n\n  " + std::to_string(inv.size()) + " components"
            "  |  " + std::to_string(inv.getLowStock().size()) + " low stock",
            {
                "List all components",
                "View component detail",
                "Add component",
                "Edit component",
                "Delete component",
                "List by category",
                "Quick name / ID search",
                "List by package type",
                "List by pin count",
                "Back"
            }, 4);

        if (ch == 9) return;

        if (ch == 0) {
            clearScreen(); banner(); hdr("All Components");
            printCompList(inv.getAll());
            pause();
        }
        else if (ch == 1) {
            clearScreen(); banner(); hdr("View Component");
            std::string id = inputLine("Component ID: ");
            Component* c = inv.getComponent(id);
            if (c) c->display();
            else   std::cout << CLR_RED "  Not found.\n" CLR_RESET;
            pause();
        }
        else if (ch == 2) { moduleAddComponent(inv, pm); }
        else if (ch == 3) { moduleEditComponent(inv, pm); }
        else if (ch == 4) { moduleDeleteComponent(inv, pm); }
        else if (ch == 5) {
            clearScreen(); banner(); hdr("List by Category");
            std::string path = inputLine("Category path (e.g. Passives/Resistors): ");
            printCompList(inv.getByCategory(path));
            pause();
        }
        else if (ch == 6) {
            clearScreen(); banner(); hdr("Name / ID Search");
            std::string q = inputLine("Search query: ");
            printCompList(inv.searchByName(q));
            pause();
        }
        else if (ch == 7) {
            clearScreen(); banner(); hdr("List by Package Type");
            std::string pkg = inputLine("Package type (e.g. 0603, SOIC, THT, QFN): ");
            printCompList(inv.getByPackageType(pkg));
            pause();
        }
        else if (ch == 8) {
            clearScreen(); banner(); hdr("List by Pin Count");
            int pins = inputInt("Pin count", 2);
            printCompList(inv.getByPinCount(pins));
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 2 – TAXONOMY BROWSER
// ══════════════════════════════════════════════════════════════════════════════
static void printTree(TaxonomyNode* node, int depth = 0) {
    if (!node) return;
    if (node->getName() == "__root__") {
        for (auto* c : node->getChildren()) printTree(c, 0);
        return;
    }
    std::cout << std::string(depth * 2, ' ')
              << (depth ? "├─ " : "")
              << CLR_CYAN << node->getName() << CLR_RESET
              << (node->isLeaf() ? "" : CLR_DIM "/" CLR_RESET) << "\n";
    for (auto* c : node->getChildren()) printTree(c, depth + 1);
}

static void menuTaxonomy(Inventory& inv, ProjectManager& /*pm*/) {
    while (true) {
        int ch = arrowMenu("Category Browser (Taxonomy)", {
            "Browse full tree",
            "List children of path",
            "Create new sub-category",
            "Delete category (leaf only)",
            "Back"
        }, 4);

        if (ch == 4) return;
        clearScreen(); banner();

        if (ch == 0) {
            hdr("Full Category Tree");
            std::cout << "\n";
            printTree(inv.getTaxonomy().getRoot());
            pause();
        }
        else if (ch == 1) {
            hdr("List Children");
            std::string path = inputLine("Path (leave blank for root): ");
            for (auto& p : inv.getTaxonomy().listChildren(path))
                std::cout << "  " << CLR_CYAN << p << CLR_RESET "\n";
            pause();
        }
        else if (ch == 2) {
            hdr("Create Sub-Category");
            std::string parent = inputLine("Parent path (blank = root): ");
            std::string name   = inputLine("New category name: ");
            auto* node = inv.getTaxonomy().createCategory(parent, name);
            std::cout << CLR_GREEN "  ✓ Created: " << node->getFullPath() << CLR_RESET "\n";
            pause();
        }
        else if (ch == 3) {
            hdr("Delete Category");
            std::string path = inputLine("Path to delete: ");
            if (inv.getTaxonomy().deleteCategory(path))
                std::cout << CLR_GREEN "  ✓ Deleted.\n" CLR_RESET;
            else
                std::cout << CLR_RED "  Not found or has children.\n" CLR_RESET;
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 3 – BOM IMPORT & CROSS-REFERENCE
// ══════════════════════════════════════════════════════════════════════════════
static void menuBOM(Inventory& inv) {
    while (true) {
        int ch = arrowMenu("BOM Import & Cross-Reference", {
            "Import KiCad BOM CSV and cross-reference",
            "Back"
        }, 4);

        if (ch == 1) return;

        // ch == 0
        clearScreen(); banner(); hdr("Import KiCad BOM CSV");

        std::string fp = inputLine("BOM CSV file path: ");
        std::string pn = inputLine("Project name (blank = from filename): ");

        try {
            BOMImporter importer;
            BOM bom = importer.importKiCadCSV(fp, pn);
            std::cout << CLR_GREEN "  ✓ Imported " << bom.size()
                      << " line(s) for project: " << bom.getProjectName()
                      << CLR_RESET "\n\n";

            // Print BOM table
            std::cout << CLR_BOLD
                      << std::left
                      << "  " << std::setw(30) << "Part"
                      << std::setw(6)  << "Qty"
                      << std::setw(14) << "Package"
                      << std::setw(6)  << "Pins"
                      << "Reference\n" CLR_RESET
                      << "  " << std::string(72, '-') << "\n";
            for (auto& e : bom.getEntries()) {
                std::cout << "  "
                          << std::left
                          << std::setw(30) << e.partNumber.substr(0, 29)
                          << std::setw(6)  << e.quantity
                          << std::setw(14) << e.packageType.substr(0, 13)
                          << std::setw(6)  << e.pinCount
                          << e.reference << "\n";
            }

            std::cout << "\n  Cross-referencing...\n\n";
            BOMCrossReferencer xref(inv);
            CrossReferenceResult res = xref.compare(bom);

            // ── Matched ──────────────────────────────────────────────────
            std::cout << CLR_GREEN "  ✓ MATCHED (" << res.matched.size() << "):\n" CLR_RESET;
            for (auto& m : res.matched) {
                std::cout << "    " << CLR_BOLD << std::left << std::setw(28)
                          << m.bomEntry.partNumber.substr(0, 27) << CLR_RESET
                          << " → " << m.component->getId()
                          << "  " << m.component->getName()
                          << "  (have=" << m.component->getQuantity()
                          << ", need=" << m.bomEntry.quantity << ")\n";
            }

            // ── Shortfall ─────────────────────────────────────────────────
            std::cout << CLR_YELLOW "\n  ⚠ SHORTFALL (" << res.shortfall.size() << "):\n" CLR_RESET;
            for (auto& m : res.shortfall) {
                std::cout << "    " << CLR_BOLD << std::left << std::setw(28)
                          << m.bomEntry.partNumber.substr(0, 27) << CLR_RESET
                          << " → " << m.component->getId()
                          << "  deficit=" << CLR_YELLOW << m.deficit << CLR_RESET
                          << "  (have=" << m.component->getQuantity()
                          << ", need=" << m.bomEntry.quantity << ")\n";
            }

            // ── Missing ───────────────────────────────────────────────────
            std::cout << CLR_RED "\n  ✗ MISSING (" << res.missing.size() << "):\n" CLR_RESET;
            for (auto& e : res.missing) {
                std::cout << "    " << CLR_BOLD << std::left << std::setw(28)
                          << e.partNumber.substr(0, 27) << CLR_RESET
                          << "  qty=" << e.quantity
                          << "  ref=" << e.reference << "\n";
            }

            // ── Summary ───────────────────────────────────────────────────
            std::cout << "\n  " << std::string(60, '─') << "\n"
                      << "  Matched:    " << CLR_GREEN << res.matched.size()   << CLR_RESET
                      << "   Shortfall: " << CLR_YELLOW << res.shortfall.size() << CLR_RESET
                      << "   Missing:   " << CLR_RED    << res.missing.size()   << CLR_RESET
                      << "\n  Estimated cost of available stock: €"
                      << CLR_BOLD << std::fixed << std::setprecision(2)
                      << res.totalCost() << CLR_RESET;

            if (g_settings.valueAwareBOM)
                std::cout << CLR_DIM "\n  [Value-aware matching active — unit/type safe]" CLR_RESET;
            std::cout << "\n";
        }
        catch (std::exception& ex) {
            std::cout << CLR_RED "  Error: " << ex.what() << CLR_RESET "\n";
        }
        pause();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 4 – PARAMETRIC SEARCH
// ══════════════════════════════════════════════════════════════════════════════
static void menuSearch(Inventory& inv) {
    ParametricSearch ps(inv);
    while (true) {
        int ch = arrowMenu("Parametric Search", {
            "Quick name / ID search",
            "Filter by type",
            "Filter by package type",
            "Filter by exact pin count",
            "Filter by pin count range",
            "Advanced: numeric parameter range",
            "Compound search (combine filters)",
            "Back"
        }, 4);

        if (ch == 7) return;

        clearScreen(); banner();
        SearchCriteria sc;

        if (ch == 0) {
            hdr("Quick Search");
            sc.setNameFilter(inputLine("Search query: "));
        }
        else if (ch == 1) {
            hdr("Filter by Type");
            std::cout << "  Types: RESISTOR  CAPACITOR  INDUCTOR  MOSFET  BJT\n"
                         "         DIODE     IC         CONNECTOR ARBITRARY\n\n";
            sc.setTypeFilter(inputLine("Type name: "));
        }
        else if (ch == 2) {
            hdr("Filter by Package");
            std::cout << "  e.g.: 0402  0603  0805  1206  SOIC  MSOP  QFN  DIP  TO-220  SOT-23  THT\n\n";
            sc.setPackageType(inputLine("Package type: "));
        }
        else if (ch == 3) {
            hdr("Exact Pin Count");
            sc.setPinCount(inputInt("Exact pin count", 2));
        }
        else if (ch == 4) {
            hdr("Pin Count Range");
            int lo = inputInt("Min pins",  2);
            int hi = inputInt("Max pins", 32);
            sc.setPinCountRange(lo, hi);
        }
        else if (ch == 5) {
            hdr("Numeric Parameter Range");
            std::cout << "  Param keys: resistance  capacitance  inductance\n"
                         "              vgsThreshold  rdsOn  vceo  hfe\n"
                         "              forwardVoltage  supplyVoltage  maxVoltage\n\n";
            std::string param = inputLine("Parameter name: ");
            double lo = inputDouble("Min value", 0.0);
            double hi = inputDouble("Max value", 1e12);
            sc.addParamRange(param, lo, hi);
        }
        else if (ch == 6) {
            hdr("Compound Search");
            std::cout << "  Leave any field blank to skip that filter.\n\n";
            std::string nf = inputLine("Name filter: ");
            if (!nf.empty()) sc.setNameFilter(nf);
            std::string tf = inputLine("Type filter: ");
            if (!tf.empty()) sc.setTypeFilter(tf);
            std::string pf = inputLine("Package type filter: ");
            if (!pf.empty()) sc.setPackageType(pf);
            std::string pc_s = inputLine("Exact pin count (blank=skip): ");
            if (!pc_s.empty()) { try { sc.setPinCount(std::stoi(pc_s)); } catch(...){} }
            std::string cat = inputLine("Category filter: ");
            if (!cat.empty()) sc.setCategory(cat);
            std::string mp = inputLine("Max unit price € (blank=skip): ");
            if (!mp.empty()) { try { sc.setMaxUnitPrice(std::stod(mp)); } catch(...){} }
            std::string ls = inputLine("Low stock only? (y/N): ");
            if (ls == "y" || ls == "Y") sc.onlyLowStock();
        }

        hdr("Results");
        auto results = ps.execute(sc);
        printCompList(results);
        pause();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 5 – PROJECT MANAGEMENT
// ══════════════════════════════════════════════════════════════════════════════
static void menuProjects(Inventory& inv, ProjectManager& pm) {
    while (true) {
        int ch = arrowMenu("Project Management", {
            "List projects",
            "Create project",
            "Checkout components for project",
            "Return components to inventory",
            "View project transaction log",
            "View project cost",
            "Export transaction log to CSV",
            "Restock component",
            "Back"
        }, 4);

        if (ch == 8) return;
        clearScreen(); banner();

        if (ch == 0) {
            hdr("Projects");
            auto names = pm.listProjects();
            if (names.empty()) { std::cout << "  (no projects)\n"; }
            else {
                std::cout << CLR_BOLD "  " << std::left << std::setw(30) << "Name"
                          << "Cost\n" CLR_RESET "  " << std::string(40, '-') << "\n";
                for (auto& n : names)
                    std::cout << "  " << std::setw(30) << n
                              << "€" << std::fixed << std::setprecision(2)
                              << pm.getProjectCost(n) << "\n";
            }
            pause();
        }
        else if (ch == 1) {
            hdr("Create Project");
            std::string name = inputLine("Project name: ");
            std::string desc = inputLine("Description: ");
            pm.createProject(name, desc);
            std::cout << CLR_GREEN "  ✓ Project '" << name << "' created.\n" CLR_RESET;
            trySave(inv, pm);
            pause();
        }
        else if (ch == 2) {
            hdr("Checkout Components");
            std::string proj = inputLine("Project name: ");
            if (!pm.hasProject(proj)) {
                std::cout << CLR_RED "  Project not found.\n" CLR_RESET; pause(); continue;
            }
            std::string id  = inputLine("Component ID: ");
            int         qty = inputInt("Quantity to checkout", 1);
            if (pm.checkoutForProject(proj, id, qty)) {
                std::cout << CLR_GREEN "  ✓ Checked out " << qty << " × " << id << "\n" CLR_RESET;
                trySave(inv, pm);
            } else {
                std::cout << CLR_RED "  Failed — insufficient stock or component not found.\n" CLR_RESET;
            }
            pause();
        }
        else if (ch == 3) {
            hdr("Return Components");
            std::string proj = inputLine("Project name: ");
            std::string id   = inputLine("Component ID: ");
            int         qty  = inputInt("Quantity to return", 1);
            pm.returnToInventory(proj, id, qty);
            std::cout << CLR_GREEN "  ✓ Returned.\n" CLR_RESET;
            trySave(inv, pm);
            pause();
        }
        else if (ch == 4) {
            hdr("Transaction Log");
            std::string proj = inputLine("Project name: ");
            if (!pm.hasProject(proj)) { pause(); continue; }
            std::cout << "\n";
            for (auto& tx : pm.getProject(proj).getLog().getAll())
                std::cout << "  " << tx.toString() << "\n";
            pause();
        }
        else if (ch == 5) {
            hdr("Project Cost");
            std::string proj = inputLine("Project name: ");
            std::cout << "\n  Total cost for '" << proj << "': €"
                      << CLR_BOLD << std::fixed << std::setprecision(4)
                      << pm.getProjectCost(proj) << CLR_RESET "\n";
            pause();
        }
        else if (ch == 6) {
            hdr("Export Transaction Log");
            std::string proj = inputLine("Project name: ");
            std::string path = inputLine("Output CSV path: ");
            if (pm.hasProject(proj)) {
                pm.getProject(proj).getLog().exportCSV(path);
                std::cout << CLR_GREEN "  ✓ Exported to " << path << "\n" CLR_RESET;
            } else {
                std::cout << CLR_RED "  Project not found.\n" CLR_RESET;
            }
            pause();
        }
        else if (ch == 7) {
            hdr("Restock Component");
            std::string id  = inputLine("Component ID: ");
            int         qty = inputInt("Quantity to add", 10);
            pm.restockComponent(id, qty);
            std::cout << CLR_GREEN "  ✓ Restocked " << qty << " × " << id << ".\n" CLR_RESET;
            trySave(inv, pm);
            pause();
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 6 – DATASHEET MANAGER
// ══════════════════════════════════════════════════════════════════════════════
static void menuDatasheet(Inventory& inv) {
    DatasheetManager dm(inv);
    while (true) {
        int ch = arrowMenu("Datasheet Manager", {
            "Open datasheet for component",
            "Set / update datasheet path or URL",
            "Show datasheet path",
            "Back"
        }, 4);

        if (ch == 3) return;
        clearScreen(); banner();

        std::string id = inputLine("Component ID: ");

        if (ch == 0) {
            hdr("Open Datasheet");
            dm.openDatasheet(id);
        }
        else if (ch == 1) {
            hdr("Set Datasheet");
            std::string path = inputLine("Datasheet path or URL: ");
            dm.setDatasheet(id, path);
            std::cout << CLR_GREEN "  ✓ Set.\n" CLR_RESET;
        }
        else if (ch == 2) {
            hdr("Datasheet Path");
            std::cout << "  " << dm.getDatasheet(id) << "\n";
        }
        pause();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 7 – LOW STOCK ALERTS
// ══════════════════════════════════════════════════════════════════════════════
static void menuLowStock(Inventory& inv) {
    while (true) {
        int ch = arrowMenu("Low Stock Alerts", {
            "Show low stock report",
            "Export low stock report to CSV",
            "Back"
        }, 4);

        if (ch == 2) return;
        clearScreen(); banner();

        if (ch == 0) {
            hdr("Low Stock Report");
            std::cout << "\n" << LowStockAlert::generateReport(inv);
        }
        else if (ch == 1) {
            hdr("Export Low Stock CSV");
            std::string path = inputLine("Output CSV path: ");
            if (LowStockAlert::exportReport(inv, path))
                std::cout << CLR_GREEN "  ✓ Exported to " << path << "\n" CLR_RESET;
            else
                std::cout << CLR_RED "  Failed to write file.\n" CLR_RESET;
        }
        pause();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// MODULE 8 – SETTINGS
// ══════════════════════════════════════════════════════════════════════════════
static void menuSettings() {
    while (true) {
        // Build display strings
        auto saveModeStr = [&]() -> std::string {
            switch (g_settings.saveMode) {
                case SaveMode::AUTO:     return CLR_SETTINGON  "Auto-save after every change" CLR_RESET;
                case SaveMode::PROMPTED: return CLR_SETTINGON  "Prompt to save on exit"       CLR_RESET;
                default:                 return CLR_SETTINGOFF "Manual (Save to disk menu)"   CLR_RESET;
            }
        };
        auto yn = [](bool v) -> std::string {
            return v ? CLR_SETTINGON  "Yes" CLR_RESET
                     : CLR_SETTINGOFF "No"  CLR_RESET;
        };

        int ch = arrowMenu("Settings  [Enter to toggle / change]", {
            std::string("Save mode           : ") + saveModeStr(),
            std::string("Confirm before delete: ") + yn(g_settings.confirmDelete),
            std::string("BOM value-aware match: ") + yn(g_settings.valueAwareBOM),
            std::string("BOM type warnings    : ") + yn(g_settings.bomTypeWarnings),
            std::string("Alternating row tint : ") + yn(g_settings.colorTableRows),
            std::string("Data directory       : ") + CLR_CYAN + g_dataDir + CLR_RESET,
            "Back"
        }, 4);

        if (ch == 6) { saveSettings(); return; }
        clearScreen(); banner();

        switch (ch) {
            case 0:  // Cycle save mode
                switch (g_settings.saveMode) {
                    case SaveMode::MANUAL:   g_settings.saveMode = SaveMode::AUTO;     break;
                    case SaveMode::AUTO:     g_settings.saveMode = SaveMode::PROMPTED; break;
                    case SaveMode::PROMPTED: g_settings.saveMode = SaveMode::MANUAL;   break;
                }
                break;
            case 1: g_settings.confirmDelete   = !g_settings.confirmDelete;   break;
            case 2: g_settings.valueAwareBOM   = !g_settings.valueAwareBOM;   break;
            case 3: g_settings.bomTypeWarnings = !g_settings.bomTypeWarnings; break;
            case 4: g_settings.colorTableRows  = !g_settings.colorTableRows;  break;
            case 5: {
                hdr("Data Directory");
                std::string nd = inputLine("New data directory [" + g_dataDir + "]: ");
                if (!nd.empty()) g_dataDir = nd;
                break;
            }
        }
        saveSettings();
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// SAMPLE DATA
// ══════════════════════════════════════════════════════════════════════════════
static void loadSampleData(Inventory& inv) {
    // Resistors
    inv.addComponent(new Resistor("RES0001","1k Resistor",       100,1000.0,  1.0,0.25,"DIN0207",5,0.02,"Passives/Resistors/THT","","Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",2));
    inv.addComponent(new Resistor("RES0002","47k Resistor",       50,47000.0, 1.0,0.25,"DIN0207",5,0.02,"Passives/Resistors/THT","","Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",2));
    inv.addComponent(new Resistor("RES0003","4.7k Resistor",      80,4700.0,  1.0,0.25,"DIN0207",5,0.02,"Passives/Resistors/THT","","Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal",2));
    inv.addComponent(new Resistor("RES0004","10k SMD 0603",      200,10000.0, 1.0,0.10,"0603",  10,0.01,"Passives/Resistors/SMD","","Resistor_SMD:R_0603_1608Metric",2));
    inv.addComponent(new Resistor("RES0005","100R SMD 0402",      50,100.0,   5.0,0.063,"0402", 10,0.008,"Passives/Resistors/SMD","","Resistor_SMD:R_0402_1005Metric",2));
    // Capacitors
    inv.addComponent(new Capacitor("CAP0001","4.7nF Disc",        10,4.7e-9, "ceramic",   10.0,100.0,"Disc", 2,0.05,"Passives/Capacitors/Ceramic","","Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm",2));
    inv.addComponent(new Capacitor("CAP0002","1000uF Electro",     3,1000e-6,"electrolytic",20.0, 35.0,"Radial",2,0.30,"Passives/Capacitors/Electrolytic","","Capacitor_THT:CP_Radial_D16.0mm_P7.50mm",2));
    inv.addComponent(new Capacitor("CAP0003","100nF SMD 0603",   500,100e-9, "ceramic",   10.0, 25.0,"0603",10,0.015,"Passives/Capacitors/Ceramic","","Capacitor_SMD:C_0603_1608Metric",2));
    inv.addComponent(new Capacitor("CAP0004","10uF SMD 0805",    100,10e-6,  "ceramic",   20.0, 10.0,"0805",10,0.05,"Passives/Capacitors/Ceramic","","Capacitor_SMD:C_0805_2012Metric",2));
    // Inductors
    inv.addComponent(new Inductor("IND0001","33uH Toroid THT",     5,33e-6, 3.0, 40.0,"Toroid",2,0.80,"Passives/Inductors/THT","","Inductor_THT:L_Toroid_Vertical_L13.0mm_W6.5mm_P5.60mm",2));
    inv.addComponent(new Inductor("IND0002","10uH SMD",           20,10e-6, 1.5, 0.0, "1210",  5,0.25,"Passives/Inductors/SMD","","",2));
    // Diodes
    inv.addComponent(new DiodeComponent("DIO0001","1N5822 Schottky", 8,0.45, 40.0,"schottky",  3.0,"DO-201AD",2,0.35,"Semiconductors/Diodes/Schottky","http://www.vishay.com/docs/88526/1n5820.pdf","Diode_THT:D_DO-201AD_P15.24mm_Horizontal","THT-DO201",2));
    inv.addComponent(new DiodeComponent("DIO0002","1N4007 Rectifier",50,0.7,1000.0,"rectifier",1.0,"DO-41",   5,0.05,"Semiconductors/Diodes/Rectifier","","","DO-41",2));
    inv.addComponent(new DiodeComponent("DIO0003","BZX55C5V1 Zener",30,5.1,  5.1,"zener",     0.2,"DO-35",   5,0.08,"Semiconductors/Diodes/Zener","","","DO-35",2));
    // ICs
    inv.addComponent(new IntegratedCircuit("IC_0001","LM2596T-ADJ",  2,"Step-down switching regulator",3.0,40.0,3.0,"TO-220-5",5,2,2.50,"Semiconductors/ICs/Power/DCDC","http://www.ti.com/lit/ds/symlink/lm2596.pdf","Package_TO_SOT_THT:TO-220-5_P3.4x3.7mm_StaggerOdd_Lead3.8mm_Vertical","TO-220"));
    inv.addComponent(new IntegratedCircuit("IC_0002","LT1764AET#PBF",3,"Low-dropout regulator 3A",1.5,20.0,3.0,"TO-263-5",5,2,3.80,"Semiconductors/ICs/Power/LDO","https://www.analog.com/media/en/technical-documentation/data-sheets/1764f.pdf","","TO-263"));
    inv.addComponent(new IntegratedCircuit("IC_0003","LTC4365",       1,"OV/UV/Reverse Protection",2.5,18.0,0.01,"MSOP-10",10,2,4.20,"Semiconductors/ICs/Protection","https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4365.pdf","","MSOP"));
    inv.addComponent(new IntegratedCircuit("IC_0004","ESP32-WROOM-32",1,"WiFi+BT SoC",3.3,3.6,0.5,"Module",38,2,5.00,"Semiconductors/ICs/Microcontrollers","","","Module"));
    // BJTs
    inv.addComponent(new BJTTransistor("BJT0001","BC547",20,"NPN",200,45,0.1,"TO-92",5,0.08,"Semiconductors/Transistors/BJT/NPN","","","TO-92",3));
    inv.addComponent(new BJTTransistor("BJT0002","BC557",10,"PNP",200,45,0.1,"TO-92",5,0.08,"Semiconductors/Transistors/BJT/PNP","","","TO-92",3));
    // MOSFETs
    inv.addComponent(new MOSFETTransistor("MOS0001","IRLZ44N",5,"N-CH",2.0,0.022,48.0,55.0,47.0,"TO-220",2,0.65,"Semiconductors/Transistors/MOSFET/N-Channel","","Package_TO_SOT_THT:TO-220-3_P3.4mm_StaggerOdd_Vertical","TO-220",3));
    inv.addComponent(new MOSFETTransistor("MOS0002","AO4407",5,"P-CH",1.8,0.018,30.0,30.0,12.0,"SOP-8",2,0.90,"Semiconductors/Transistors/MOSFET/P-Channel","","","SOP",3));
    // Connectors
    inv.addComponent(new ConnectorComponent("CON0001","PinHeader 1x08 2.54mm",4,"PinHeader",2.54,"male",8,2,0.15,"Connectors/PinHeaders","","Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical","PinHeader"));
    inv.addComponent(new ConnectorComponent("CON0002","PinHeader 1x02 2.54mm",10,"PinHeader",2.54,"male",2,2,0.08,"Connectors/PinHeaders","","Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical","PinHeader"));
    inv.addComponent(new ConnectorComponent("CON0003","Screw Terminal 1x02",6,"TerminalBlock",5.08,"female",2,2,0.35,"Connectors/ScrewTerminals","","TerminalBlock_MetzConnect:TerminalBlock_MetzConnect_Type101_RT01602HBWC_1x02_P5.08mm_Horizontal","TerminalBlock"));
    // Arbitrary
    auto* fuse = new ArbitraryComponent("ARB0001","Inline Fuse 2A",2,"Glass fuse 5x20mm",3,0.40,"Passives/Fuses","","","THT",2);
    fuse->addParam("rating_A","2"); fuse->addParam("size_mm","5x20"); fuse->addParam("type","fast-blow");
    inv.addComponent(fuse);
    auto* pot = new ArbitraryComponent("ARB0002","Potentiometer 10k",1,"Bourns 3296W trim pot",2,0.85,"Passives/Potentiometers","","Potentiometer_THT:Potentiometer_Bourns_3296W_Vertical","THT",3);
    pot->addParam("resistance_Ohm","10000"); pot->addParam("turns","25"); pot->addParam("power_W","0.5");
    inv.addComponent(pot);

    std::cout << CLR_GREEN "  ✓ Sample data loaded (" << inv.size() << " components)\n" CLR_RESET;
}

// ══════════════════════════════════════════════════════════════════════════════
// MAIN
// ══════════════════════════════════════════════════════════════════════════════
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    enableANSI();

    loadSettings();

    Inventory      inv;
    ProjectManager pm(inv);

    // ── Boot menu ────────────────────────────────────────────────────────────
    int boot = arrowMenu(
        "Welcome to ECIMS"
        "\n\n  Data directory: " + g_dataDir,
        { "Load saved data",
          "Load sample data (demonstration)",
          "Start empty" },
        4);

    if (boot == 0) {
        clearScreen(); banner();
        if (Persistence::loadAll(inv, pm, g_dataDir))
            std::cout << CLR_GREEN "  ✓ Data loaded. " << inv.size() << " component(s).\n" CLR_RESET;
        else
            std::cout << CLR_YELLOW "  No saved data found — starting empty.\n" CLR_RESET;
        pause();
    }
    else if (boot == 1) {
        clearScreen(); banner();
        loadSampleData(inv);
        pause();
    }

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (true) {
        auto saveModeTag = [&]() -> std::string {
            switch (g_settings.saveMode) {
                case SaveMode::AUTO:     return CLR_GREEN  " [auto]"    CLR_RESET;
                case SaveMode::PROMPTED: return CLR_YELLOW " [prompted]" CLR_RESET;
                default:                 return CLR_DIM    " [manual]"   CLR_RESET;
            }
        };

        int ch = arrowMenu(
            "Main Menu"
            "\n\n  Inventory: " + std::to_string(inv.size()) + " components"
            "   |   Low stock: " + std::to_string(inv.getLowStock().size()),
            {
                "Inventory (CRUD)",
                "Category Browser",
                "BOM Import & Cross-Reference",
                "Parametric Search",
                "Project Management",
                "Datasheet Manager",
                "Low Stock Alerts",
                std::string("Save to disk") + saveModeTag(),
                "Load from disk",
                "Settings",
                "Exit"
            }, 4);

        if      (ch == 0)  menuInventory(inv, pm);
        else if (ch == 1)  menuTaxonomy(inv, pm);
        else if (ch == 2)  menuBOM(inv);
        else if (ch == 3)  menuSearch(inv);
        else if (ch == 4)  menuProjects(inv, pm);
        else if (ch == 5)  menuDatasheet(inv);
        else if (ch == 6)  menuLowStock(inv);
        else if (ch == 7) {
            clearScreen(); banner(); hdr("Save to Disk");
            if (Persistence::saveAll(inv, pm, g_dataDir))
                std::cout << CLR_GREEN "\n  ✓ Saved to '" << g_dataDir << "/'\n" CLR_RESET;
            else
                std::cout << CLR_RED "\n  Save failed.\n" CLR_RESET;
            pause();
        }
        else if (ch == 8) {
            clearScreen(); banner(); hdr("Load from Disk");
            if (Persistence::loadAll(inv, pm, g_dataDir))
                std::cout << CLR_GREEN "\n  ✓ Loaded. " << inv.size() << " component(s).\n" CLR_RESET;
            else
                std::cout << CLR_RED "\n  Load failed.\n" CLR_RESET;
            pause();
        }
        else if (ch == 9) { menuSettings(); }
        else if (ch == 10) {
            // Exit — handle prompted-save mode
            if (g_settings.saveMode == SaveMode::PROMPTED) {
                int ans = arrowMenu("Save before exiting?", {
                    "Yes — save and exit",
                    "No  — exit without saving",
                    "Cancel — go back"
                }, 4);
                if (ans == 0) {
                    Persistence::saveAll(inv, pm, g_dataDir);
                    break;
                }
                else if (ans == 1) { break; }
                // ans == 2 → go back to main menu
            }
            else { break; }
        }
    }

    std::cout << CLR_BOLD CLR_BLUE "\n  Goodbye.\n\n" CLR_RESET;
    return 0;
}