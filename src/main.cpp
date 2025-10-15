#include "MainWindow.hpp"
#include "Database.hpp"

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    // init DB (in constructor also)
    MainWindow w;
    w.show();
    return a.exec();
}
