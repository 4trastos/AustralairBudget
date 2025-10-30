QT += core widgets sql printsupport

TARGET = australair
TEMPLATE = app

CONFIG += c++17

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/Database.cpp \
    src/AustralairBudget.cpp \
    src/Buttons.cpp \
    src/Printer.cpp \
    src/ExportPdf.cpp \
    src/StatusWork.cpp \
    src/Fuel.cpp \
    src/Load.cpp \
    src/DataSaveLoad.cpp \
    src/Calculator.cpp \
    src/MaterialsWindow.cpp

HEADERS += \
    incl/MainWindow.hpp \
    incl/AustralairBudget.hpp \
    incl/MaterialsWindow.hpp \
    incl/Database.hpp

RESOURCES += resources.qrc

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android