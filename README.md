# BiteKing - Restaurant Billing & Ordering System

BiteKing is a modern, cross-platform C++/Qt 6 restaurant point-of-sale (POS) and ordering application. It supports table management, menu editing, invoice generation, discount/tax calculations, light/dark theme switching, and automatic text-file menu persistence.

---

## Prerequisites

To compile and run this application, you must have **CMake** and **Qt 6 SDK (Core, Gui, Widgets)** installed.

---

## How to Build & Run

### 🐧 Option 1: In WSL / Linux Terminal (Recommended for WSL Users)

1. **Install Dependencies:**
   On Ubuntu/Debian (inside WSL):
   ```bash
   sudo apt-get update
   sudo apt-get install -y cmake build-essential qt6-base-dev qt6-base-private-dev
   ```
   *(Note: Ensure your WSL has GUI support enabled, i.e., WSLg on Windows 11/10, or an X Server running on your host machine.)*

2. **Configure & Build:**
   ```bash
   cmake -B build -S .
   cmake --build build
   ```

3. **Launch the Application:**
   ```bash
   ./build/restaurant_billing_qt
   ```

---

### 💻 Option 2: In Visual Studio (Windows Native)

Visual Studio natively supports opening and building CMake projects.

1. **Install Qt 6:**
   - Download the Qt Online Installer from [qt.io](https://www.qt.io/download-open-source).
   - Install **Qt 6.x** for MSVC (e.g., *Qt 6.5 MSVC 2019 64-bit*).

2. **Open the Project in Visual Studio:**
   - Open Visual Studio.
   - Go to **File** -> **Open** -> **Folder...** and select the `Restaurant-Billing-Project` root directory.
   - Visual Studio will automatically detect `CMakeLists.txt` and start configuring the project.

3. **Configure the Qt Path:**
   - Visual Studio's CMake generation will likely fail initially with a "Qt6 package not found" error because it doesn't know where Qt is installed.
   - To fix this, create or open `CMakePresets.json` or configure CMake variables by going to **Project** -> **CMake Settings for Project**.
   - Add a CMake variable named `CMAKE_PREFIX_PATH` and set its value to point to your Qt MSVC directory, for example:
     `C:/Qt/6.5.3/msvc2019_64`
   - Save and let CMake regenerate.

4. **Build & Run:**
   - Set the startup item/target to `restaurant_billing_qt.exe` in the toolbar.
   - Press **F5** or click the **Start** button to compile and launch.

---

## Project Directory Structure

```
├── CMakeLists.txt         # CMake compilation rules
├── data/
│   └── menu.txt           # Persistent food and beverage items database
├── src/                   # C++ source and header files
│   ├── main_qt.cpp        # Application entry point
│   ├── MainWindow.h/cpp   # BiteKing UI logic and styles
│   ├── CustomerOrder.h/cpp# Order management model
│   ├── DiningTable.h/cpp  # Table layout and bill calculation
│   ├── FoodItem.h/cpp     # Food menu items model
│   ├── BeverageItem.h/cpp # Beverage menu items model
│   └── ...
├── test_plan.xlsx         # Styled ONLYOFFICE/Excel testing spreadsheet
├── test_plan.md           # Step-by-step test cases
└── README.md              # This build guide
```
