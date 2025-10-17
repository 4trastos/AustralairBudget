#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    // init DB (in constructor also)
    // MainWindow w;
    // w.show();
    AustralairBudget inicio;
    inicio.show();
    return a.exec();
}
