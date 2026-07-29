cmake_minimum_required(VERSION 3.16)
project(RestaurantBillingProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable automatic compilation of Qt Meta-Object Compiler (MOC), User Interface Compiler (UIC), and Resource Compiler (RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt 6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Source and header files
set(SOURCES
    src/main_qt.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/MenuItem.h
    src/MenuItem.cpp
    src/FoodItem.h
    src/FoodItem.cpp
    src/BeverageItem.h
    src/BeverageItem.cpp
    src/CustomerOrder.h
    src/CustomerOrder.cpp
    src/DiningTable.h
    src/DiningTable.cpp
    src/RestaurantManager.h
    src/RestaurantManager.cpp
    src/MenuFileIO.h
    src/MenuFileIO.cpp
)

# Create the executable
add_executable(restaurant_billing_qt ${SOURCES})

# Link Qt 6 libraries
target_link_libraries(restaurant_billing_qt PRIVATE Qt6::Widgets)
