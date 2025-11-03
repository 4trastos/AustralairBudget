#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"
#include "MaterialsWindow.hpp"

int main(int argc, char **argv)
{
    // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    // QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    //QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication a(argc, argv);

    // init DB (in constructor also)
    AustralairBudget inicio;
    inicio.show();
    return a.exec();
}



/* int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    // Configurar estilo para mejor apariencia
    a.setStyle("Fusion");

    // init DB (in constructor also)
    AustralairBudget inicio;
    //inicio.show();
    inicio.showMaximized();
    return a.exec();
}
 */
