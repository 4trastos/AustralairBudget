cmake_minimum_required(VERSION 3.21)
project(AustralairBudget VERSION 1.0.0 LANGUAGES CXX)

if(ANDROID)
    set(ANDROID_MIN_SDK 28)
    set(ANDROID_TARGET_SDK 34)
    set(ANDROID_PACKAGE_NAME "com.australair.budget")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql PrintSupport Gui)
qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Database.cpp
    src/AustralairBudget.cpp
    src/Buttons.cpp
    src/Printer.cpp
    src/ExportPdf.cpp
    src/StatusWork.cpp
    src/Fuel.cpp
    src/Load.cpp
    src/DataSaveLoad.cpp
    src/Calculator.cpp
    src/MaterialsWindow.cpp
    incl/MainWindow.hpp
    incl/AustralairBudget.hpp
    incl/MaterialsWindow.hpp
)

qt_add_executable(australair
    MANUAL_FINALIZATION
    ${SOURCES}
)

if(ANDROID)
    set_target_properties(australair PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
        QT_ANDROID_APP_NAME "Australair Budget"
        QT_ANDROID_PACKAGE_NAME "com.australair.budget"
        QT_ANDROID_VERSION_CODE "1"
        QT_ANDROID_VERSION_NAME "1.0.0"
    )
    find_library(LOG_LIB log)
    target_link_libraries(australair PRIVATE ${LOG_LIB})
endif()

qt_add_resources(australair resources.qrc)
target_include_directories(australair PRIVATE incl)
target_link_libraries(australair PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
    Qt6::Gui
)

qt_finalize_executable(australair)
