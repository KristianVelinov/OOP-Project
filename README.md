
# ECIMS — Electronic Component Inventory Management System

## Notice!

CMake is required to build the project

## Build

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
./ecims
```

## Features
1. **CRUD** — add/edit/delete all 9 component types
2. **Taxonomy** — dynamic category tree browser
3. **BOM Import** — KiCad CSV cross-reference
4. **Smart Matcher** — prefix matching (LT1764 → LT1764AET#PBF)
5. **Parametric Search** — filter by type/package/pins/parameters
6. **Project Checkout** — track component usage per project
7. **Project Costing** — calculate BOM cost
8. **Low Stock Alerts** — threshold-based reorder list
9. **Datasheet Launcher** — open local PDFs or URLs
10. **Persistence** — save/load all data

## Data files (auto-created in `data/`)
- `inventory.dat`     — all components
- `taxonomy.dat`      — category tree
- `projects.dat`      — project names/descriptions
- `transactions.dat`  — checkout/return log
