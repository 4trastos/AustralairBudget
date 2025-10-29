#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

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

QMap<QString, QPair<double, double>> MainWindow::loadMaterialsFromFile(const QString &filename)
{
    QMap<QString, QPair<double, double>> materials;
    QFile file(filename);
    qDebug() << "Intentando abrir:" << filename;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo de materiales.";
        // Crear archivo de ejemplo si no existe
        createSampleMaterialsFile(filename);
        return materials;
    }

    QTextStream in(&file);
    int lineCount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineCount++;
        if (line.isEmpty() || line.startsWith("#")) continue;
        
        QStringList parts = line.split(';');
        if (parts.size() == 2) {
            // Formato antiguo: Nombre;PrecioVenta
            QString name = parts[0].trimmed();
            double sellPrice = parts[1].toDouble();
            materials[name] = qMakePair(sellPrice, sellPrice * 0.7); // Precio compra = 70% de venta
        } else if (parts.size() == 3) {
            // Formato nuevo: Nombre;PrecioVenta;PrecioCompra
            QString name = parts[0].trimmed();
            double sellPrice = parts[1].toDouble();
            double costPrice = parts[2].toDouble();
            materials[name] = qMakePair(sellPrice, costPrice);
        } else {
            qWarning() << "Formato inválido en línea" << lineCount << ":" << line;
        }
    }
    
    qDebug() << "Materiales cargados:" << materials.size() << "elementos";
    file.close();
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


void MainWindow::setNextBudgetNumberAndDate()
{
    QSqlQuery q(Database::instance());
    q.exec("SELECT MAX(id) FROM budgets");
    int nextId = 1;
    if (q.next())
        nextId = q.value(0).toInt() + 1;

    QString numPresu = QString("P%1").arg(nextId, 4, 10, QChar('0')); // ejemplo: P0001, P0002, ...
    leNumPresu->setText(numPresu);

    QString fechaHoy = QDate::currentDate().toString("dd/MM/yyyy");
    leFecha->setText(fechaHoy);
}


void MainWindow::createSampleMaterialsFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "# Archivo de materiales - Formato: Nombre;PrecioVenta;PrecioCompra\n";
        out << "# También soporta formato antiguo: Nombre;PrecioVenta\n";
        out << "Tornillo M8;0.15;0.08\n";
        out << "Panel sandwich;45.00;32.50\n";
        out << "Poliuretano;12.50;8.75\n";
        out << "Silicona;4.50;3.20\n";
        out << "Lana mineral;8.00;5.60\n";
        out << "Pintura;25.00;18.00\n";
        out << "Cinta aluminio;3.50;2.45\n";
        out << "Brida plástico;0.80;0.56\n";
        out << "Tubo PVC;2.30;1.61\n";
        out << "Fijación techo;1.20;0.84\n";
        file.close();
        qDebug() << "Archivo de materiales de ejemplo creado:" << filename;
    }
}