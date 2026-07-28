# Team Responsibilities & Task Division
**Course:** CST8219 – C++ Programming  
**Project Title:** Restaurant Billing and Ordering System  
**Last Updated:** 2026-07-28  

This document outlines the final division of responsibilities for each team member, reconciled from the signed **Group Project Acknowledgement & Task Division** agreement (`Responsibilities.jpg`) and the current project `todo.md`.

---

## 👥 Team Members
1. **Alhan Mahfouz** (Lab Section: 300)
2. **Syed Razi** (Lab Section: 300)
3. **Arshdeep Sahota** (Lab Section: 300)

---

##  Detailed Responsibilities Split

### 1. Alhan Mahfouz (Core Menu Classes)
*   **Role:** Backend Core Development
*   **Planned Responsibilities (from agreement):**
    *   Build core menu classes:
        *   Base class: `MenuItem`
        *   Derived classes: `FoodItem` and `BeverageItem`
    *   Set up dynamic price calculations (handling discounts, taxes, and special offers).
    *   Create a custom shortcut/override to print item details using `std::cout` (dynamic string formatting).
*   **Integration Annotations:**
    *   Responsible for integrating these core classes into the general C++ structure (annotated as **"INTEGRATION"**).
*   **Current Status:** **COMPLETE** 

---

### 2. Syed Razi (Orders, Data Analytics & File I/O)
*   **Role:** Business Logic, File Persistence & Data Management
*   **Planned Responsibilities (from agreement + updates):**
    *   Write classes to handle customer orders and tables (managing order lists).
    *   Utilize `std::vector` to manage collections of items.
    *   Use **smart pointers** (`std::unique_ptr` / `std::shared_ptr`) to handle dynamic memory allocation safely without memory leaks.
*   **Transferred Tasks (Annotated in agreement):**
    *   *From Arshdeep:* Build the main manager class to organize the system's data.
    *   *From Arshdeep:* Perform data analytics on order history and restaurant performance (annotated as **"Data Analytics"**).
    *   *From Arshdeep:* Implement safe File I/O functions to save and load the menu items to/from a text file.
*   **Current Status:** **PENDING**  (Awaiting codebase push for UI integration)

---

### 3. Arshdeep Sahota (UI & Testing)
*   **Role:** Frontend Design, Integration & Quality Assurance
*   **Planned Responsibilities (from agreement + updates):**
    *   Design the graphical user interface (GUI) using Qt:
        *   Visual window, buttons, tables, receipt preview, spinners, and totals.
        *   Utilize Qt **signals and slots** for event-driven programming.
    *   Implement multiple dining table views and state switches (Tables 1-8).
*   **Integration & Testing Tasks (Annotated in agreement):**
    *   Integrate Alhan's core classes (`MenuItem`, `FoodItem`, `BeverageItem`) into the build configuration and UI.
    *   Integrate Syed's order/table classes and File I/O load/save menu features once uploaded.
    *   Set up comprehensive test cases and perform thorough system verification and leak checks (annotated as **"TESTING"**).
*   **Current Status:** **IN PROGRESS** 🔄 (UI components/layouts completed; waiting on Syed's backend files to complete integration)

---

##  Shared Deliverables
The following tasks are collaboratively shared by all team members:
1. **Academic Integrity & Collaboration:** Ensure all contributions are original, correctly formatted, and comply with the Algonquin College Academic Integrity Policy.
2. **Project Presentation:** Design PowerPoint slides outlining the project structure, design patterns used, and demonstrating the working application.
3. **Final Project Report:** Author the final project documentation explaining the class hierarchy, Qt GUI implementation details, data storage mechanism, and division of labor.
