# Restaurant Billing and Ordering System - Project Todo List
**Last Updated:** 2026-07-28 08:44 AM (EDT)

## Task Division & Progress Tracker

### 1. Alhan Mahfouz (Core Menu Classes) — **STATUS: COMPLETE** 
- [x] Build core `MenuItem` base class
- [x] Build derived `FoodItem` and `BeverageItem` classes
- [x] Implement dynamic price calculations (discounts, taxes, special offers)
- [x] Implement dynamic string printing functions
- [x] Integrate classes into CMake compilation structure

### 2. Syed Razi (Orders, Data Analytics & File I/O) — **STATUS: PENDING** 
- [ ] Implement classes to handle customer orders and tables
- [ ] Implement main manager class to organize data and perform data analytics
- [ ] Implement File I/O functions to safely load and save menu items from/to a text file
- [ ] Utilize smart pointers to handle memory automatically without leaks
- [ ] Push codebase changes to the repository for UI integration

### 3. Arshdeep Sahota (UI & Testing) — **STATUS: IN PROGRESS** 
- [x] Design Qt MainWindow layout (tables, receipt preview, spinners, totals)
- [x] Implement multiple dining table views and state switches (Tables 1-8)
- [x] Integrate Alhan's `MenuItem`, `FoodItem`, and `BeverageItem` classes into UI and build configuration
- [ ] Integrate Syed's customer orders and dining tables classes once uploaded
- [ ] Connect Syed's File I/O load/save menu functions to the UI
- [ ] Set up test cases and perform thorough system verification and leak checks

### 4. Shared Deliverables — **STATUS: TO DO** 
- [ ] Create project PowerPoint presentation slides
- [ ] Write final project report document
