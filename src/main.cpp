#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"
#include "MaterialsWindow.hpp"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/icons/app_icon.png"));

    AustralairBudget inicio;
    inicio.show();

    return a.exec();
}

