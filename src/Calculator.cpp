#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"

void MainWindow::onCalculate()
{
    // --- Cargar precios desde archivo ---
    QMap<QString, double> prices = loadPricesFromFile(QCoreApplication::applicationDirPath() + "/prices.txt");
    double iva = getSettingDouble("iva_pct", 21.0);

    double total = 0.0;

    // --- Sumar materiales ---
    for (int r = 0; r < twMaterials->rowCount(); ++r)
    {
        QTableWidgetItem *qtyItem = twMaterials->item(r, 1);
        QTableWidgetItem *priceItem = twMaterials->item(r, 2);
        QTableWidgetItem *totalItem = twMaterials->item(r, 3);

        double qty = qtyItem ? qtyItem->text().toDouble() : 0.0;
        double up = priceItem ? priceItem->text().toDouble() : 0.0;
        double line = qty * up;

        if (totalItem)
            totalItem->setText(QString::number(line, 'f', 2));
        total += line;
    }

    // --- Coste combustible ---
    double fuelPrice = prices.value("fuel", 1.4);  // €/litro
    double litrosConsumidos = sbKM->value() * 0.1; // 10 L/100 km
    total += litrosConsumidos * fuelPrice;

    // --- Dietas ---
    double dietaPrice = prices.value("dieta", 300);
    total += spDietas->value() * spDiasDieta->value() * dietaPrice;

    // --- Horas de trabajo CON INCREMENTOS POR DISTANCIA ---
    double horaPrice = prices.value("hora_trabajo", 80);
    double horasBase = sbHoras->value();
    double horasIncremento = 0.0;

    // Aplicar incrementos solo si está en "Zona Centro" y hay una distancia seleccionada
    if (cbZona->currentText() == "Zona Centro") {
        if (rbCorta->isChecked()) {
            horasIncremento = 0.30 * 2; // 0.30 horas x 2 (ida y vuelta)
        } else if (rbMedia->isChecked()) {
            horasIncremento = 1.0 * 2;  // 1.0 hora x 2 (ida y vuelta)
        } else if (rbLarga->isChecked()) {
            horasIncremento = 1.5 * 2;  // 1.5 horas x 2 (ida y vuelta)
        }
    }

    double horasTotales = horasBase + horasIncremento;
    total += horasTotales * horaPrice;

    // --- Total con IVA ---
    double totalConIva = total * (1.0 + iva / 100.0);
    lblTotalNoIVA->setText(QString::number(total, 'f', 2) + " €");
    lblTotalConIVA->setText(QString::number(totalConIva, 'f', 2) + " €");
    
    // Mensaje de confirmación
    QMessageBox::information(this, "Cálculo completado", 
        QString("Presupuesto calculado:\n- Horas base: %1\n- Incremento desplazamiento: %2\n- Horas totales: %3")
            .arg(horasBase)
            .arg(horasIncremento)
            .arg(horasTotales));
}