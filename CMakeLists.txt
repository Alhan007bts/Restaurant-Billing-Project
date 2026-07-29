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
    main_qt.cpp
    MainWindow.h
    MainWindow.cpp
    MenuItem.h
    MenuItem.cpp
    FoodItem.h
    FoodItem.cpp
    BeverageItem.h
    BeverageItem.cpp
    CustomerOrder.h
    CustomerOrder.cpp
    DiningTable.h
    DiningTable.cpp
    RestaurantManager.h
    RestaurantManager.cpp
    MenuFileIO.h
    MenuFileIO.cpp
)

# Create the executable
add_executable(restaurant_billing_qt ${SOURCES})

# Link Qt 6 libraries
target_link_libraries(restaurant_billing_qt PRIVATE Qt6::Widgets)
