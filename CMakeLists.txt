cmake_minimum_required(VERSION 3.16)
project(AustralairBudget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

# Fuentes y headers
set(SRC
    src/main.cpp
    src/MainWindow.cpp
    src/Database.cpp
    src/AustralairBudget.cpp
    src/Buttons.cpp
    src/Printer.cpp
    src/ExportPdf.cpp
    src/StatusWork.cpp
    src/Fuel.cpp
)

set(HDR
    incl/MainWindow.hpp
    incl/Database.hpp
    incl/AustralairBudget.hpp
)

# Ejecutable
add_executable(australair ${SRC} ${HDR})

# MOC automatico
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Recursos
qt6_add_resources(RESOURCES_RCC resources.qrc)
target_sources(australair PRIVATE ${RESOURCES_RCC})

# Librerías
target_link_libraries(australair PRIVATE Qt6::Widgets Qt6::Sql)
