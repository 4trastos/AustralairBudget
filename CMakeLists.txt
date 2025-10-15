cmake_minimum_required(VERSION 3.16)
project(AustralairBudget LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

add_executable(australair
    src/AustralairBudget.hpp
    lib/MainWindow.h
    src/MainWindow.cpp
    lib/Database.h
    src/Database.cpp
    src/resources.qrc
)

qt6_add_resources(AU_RES src/resources.qrc)

target_link_libraries(australair PRIVATE Qt6::Widgets Qt6::Sql)