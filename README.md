# BiteKing - Restaurant Billing & Ordering System

BiteKing is a modern, cross-platform C++/Qt 6 restaurant point-of-sale (POS) and ordering application. It supports table management, menu editing, invoice generation, discount/tax calculations, light/dark theme switching, and automatic text-file menu persistence.

---

## Prerequisites

To compile and run this application, you must have CMake and Qt 6 SDK (Core, Gui, Widgets) installed.

## How to Build & Run

### Option 1: In Linux / macOS / WSL Terminal

1. **Install Dependencies:**
   On Ubuntu/Debian:
   ```bash
   sudo apt-get update
   sudo apt-get install -y cmake build-essential qt6-base-dev qt6-base-private-dev
   ```
   *(Note: Ensure your environment has GUI support enabled, i.e., WSLg on Windows 11/10, or an active X Server.)*

2. **Configure & Build (from the parent directory):**
   ```bash
   cmake -B build -S Restaurant-Billing-Project
   cmake --build build
   ```

3. **Launch the Application:**
   ```bash
   ./build/restaurant_billing_qt
   ```

*Note: If you are running commands from inside the `Restaurant-Billing-Project` directory itself, use `-S .` instead:*
```bash
cmake -B build -S .
cmake --build build
./build/restaurant_billing_qt
```

---

### Option 2: In Visual Studio (Windows Native)

Visual Studio natively supports opening and building CMake projects.

1. **Install Qt 6:**
   - Download the Qt Online Installer from qt.io.
   - Install Qt 6.x for MSVC (e.g., *Qt 6.5 MSVC 2019 64-bit*).

2. **Open the Project in Visual Studio:**
   - Open Visual Studio.
   - Go to **File** -> **Open** -> **Folder...** and select the `Restaurant-Billing-Project` subdirectory.
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

## Features Added

* **Cash Change Calculation**: Toggling to **Cash** enables manual payment amount entry; handles validation (insufficient cash checks) and computes/displays change in the confirmation dialog.
* **Card Exact-Amount Lock**: Toggling to **Card** automatically disables the payment input field and locks the payment amount to the exact bill total.
* **UI Auto-Clearing**: Processing a payment automatically resets and clears the Billing tab receipt text, the grand total label, and the Payment tab status panels for the next table.
* **Dated Logs & Reports**: Auto-saves a dated receipt `receipt_<PaymentID>_YYYY-MM-DD.txt`, a dated database payments summary `payments_YYYY-MM-DD.txt`, and a daily financial sales and statistics report `analytics_YYYY-MM-DD.txt` under the `data/` directory.

---

## Project Directory Structure

```
├── CMakeLists.txt              # CMake compilation rules
├── include/                    # C++ Header files (.h)
│   ├── AnalyticsEngine.h       # Sales stats calculator class
│   ├── MainWindow.h            # Main UI window declarations
│   ├── MenuItem.h / FoodItem.h # Menu item classes
│   ├── ...
├── src/                        # C++ Source files (.cpp)
│   ├── main_qt.cpp             # Application entry point
│   ├── MainWindow.cpp          # UI controller implementation
│   ├── Payment.cpp / Bill.cpp  # Billing and payment logic
│   └── ...
├── data/                       # Files database directory
│   ├── menu.txt                # Persistent menu database
│   ├── payments.txt            # Persistent payments database
│   └── ...
├── docs/                       # Project documentation and test matrices
│   ├── todo.md                 # Task tracker
│   ├── test_plan.md / .csv     # Step-by-step test cases
│   ├── test_plan.xlsx          # Interactive validation spreadsheet
│   └── responsibilities.md     # Team task assignments split
└── README.md              # This build guide
```
