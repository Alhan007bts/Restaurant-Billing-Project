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
    include/MainWindow.h
    src/MainWindow.cpp
    include/MenuItem.h
    src/MenuItem.cpp
    include/FoodItem.h
    src/FoodItem.cpp
    include/BeverageItem.h
    src/BeverageItem.cpp
    include/CustomerOrder.h
    src/CustomerOrder.cpp
    include/DiningTable.h
    src/DiningTable.cpp
    include/RestaurantManager.h
    src/RestaurantManager.cpp
    include/MenuFileIO.h
    src/MenuFileIO.cpp
    include/Order.h
    src/Order.cpp
    include/DineInOrder.h
    src/DineInOrder.cpp
    include/Bill.h
    src/Bill.cpp
    include/Payment.h
    src/Payment.cpp
    include/AnalyticsEngine.h
    include/DataManager.h
    src/DataManager.cpp
    resources.qrc
)

# Create the executable
add_executable(restaurant_billing_qt ${SOURCES})

# Target include directories (compiler will search the include/ directory for header files)
target_include_directories(restaurant_billing_qt PRIVATE include)

# Link Qt 6 libraries
target_link_libraries(restaurant_billing_qt PRIVATE Qt6::Widgets)
