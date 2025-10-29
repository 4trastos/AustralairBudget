#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"
#include "MaterialsWindow.hpp"

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    // init DB (in constructor also)
    AustralairBudget inicio;
    inicio.show();
    return a.exec();
}
