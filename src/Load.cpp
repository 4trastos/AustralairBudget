#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"

QMap<QString, double> MainWindow::loadPricesFromFile(const QString &filename)
{
    QMap<QString, double> prices;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return prices;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(';');
        if (parts.size() != 2) continue;
        QString name = parts[0].trimmed().toLower();
        double price = parts[1].toDouble();
        prices[name] = price;
    }
    return prices;
}

QMap<QString, double> MainWindow::loadMaterialsFromFile(const QString &filename)
{
    QMap<QString, double> materials;
    QFile file(filename);
    qDebug() << "Intentando abrir:" << filename;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo de materiales.";
        return materials;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(';');
        if (parts.size() != 2) continue;
        QString name = parts[0].trimmed();
        double price = parts[1].toDouble();
        materials[name] = price;
    }
    qDebug() << "Materiales cargados:" << materials;
    return materials;
}

void MainWindow::onMaterialChanged(QTableWidgetItem *item) {
    int row = item->row();
    QTableWidgetItem *qtyItem = twMaterials->item(row, 1);
    QTableWidgetItem *priceItem = twMaterials->item(row, 2);
    QTableWidgetItem *totalItem = twMaterials->item(row, 3);

    if (!qtyItem || !priceItem || !totalItem) return;

    double qty = qtyItem->text().toDouble();
    double price = priceItem->text().toDouble();
    totalItem->setText(QString::number(qty * price, 'f', 2));

    // Opcional: actualizar totales generales aquí si quieres
    onCalculate(); // recalcula totales
}
