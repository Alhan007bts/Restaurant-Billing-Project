cmake_minimum_required(VERSION 3.16)
project(RestaurantBillingProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic compilation of Qt Meta-Object Compiler (MOC), User Interface Compiler (UIC), and Resource Compiler (RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt 5 components
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

# Source and header files
set(SOURCES
    main_qt.cpp
    MainWindow.h
    MainWindow.cpp
    MenuItem.h
    MenuItem.cpp
)

# Create the executable
add_executable(restaurant_billing_qt ${SOURCES})

# Link Qt 5 libraries
target_link_libraries(restaurant_billing_qt PRIVATE Qt5::Widgets)
