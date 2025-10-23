#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"

/* void MainWindow::onCalculate()
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
} */

void MainWindow::onCalculate()
{
    // --- Cargar precios desde archivo ---
    QMap<QString, double> prices = loadPricesFromFile(QCoreApplication::applicationDirPath() + "/prices.txt");
    double iva = getSettingDouble("iva_pct", 21.0);

    double total = 0.0;

    // --- Sumar materiales (COLUMNAS MODIFICADAS) ---
    for (int r = 0; r < twMaterials->rowCount(); ++r)
    {
        QTableWidgetItem *qtyItem = twMaterials->item(r, 1);    // Columna 1: Cantidad
        QTableWidgetItem *priceItem = twMaterials->item(r, 2);  // Columna 2: Precio Venta
        QTableWidgetItem *totalItem = twMaterials->item(r, 4);  // Columna 4: Total Venta

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
    
    // --- CALCULAR Y MOSTRAR BENEFICIOS ---
    double totalSelling, totalCost, estimatedBenefit;
    calculateCostsAndBenefits(totalSelling, totalCost, estimatedBenefit);

    lblCostoEstimado->setText(QString::number(totalCost, 'f', 2) + " €");
    lblBeneficioEstimado->setText(QString::number(estimatedBenefit, 'f', 2) + " €");

    // Colorear beneficio
    if (estimatedBenefit >= 0) {
        lblBeneficioEstimado->setStyleSheet("color: green; font-weight: bold;");
    } else {
        lblBeneficioEstimado->setStyleSheet("color: red; font-weight: bold;");
    }
}

void MainWindow::calculateCostsAndBenefits(double &totalSelling, double &totalCost, double &estimatedBenefit)
{
    totalSelling = 0.0;
    totalCost = 0.0;
    
    // --- Costo materiales (COLUMNAS MODIFICADAS) ---
    for (int r = 0; r < twMaterials->rowCount(); ++r) {
        QString materialName = twMaterials->item(r, 0)->text();  // Columna 0: Nombre
        double qty = twMaterials->item(r, 1)->text().toDouble(); // Columna 1: Cantidad
        
        // Precio de venta (para total de venta) - Columna 2
        double sellPrice = twMaterials->item(r, 2)->text().toDouble();
        totalSelling += qty * sellPrice;
        
        // Precio de compra (para costo) - Columna 3
        double costPrice = twMaterials->item(r, 3)->text().toDouble();
        totalCost += qty * costPrice;
    }
    
    // --- Otros costos estimados ---
    double fuelCost = sbLitros->value() * 1.4; // Costo combustible real
    double dietCost = spDietas->value() * spDiasDieta->value() * 25.0; // Costo dietas estimado
    double hoursCost = sbHoras->value() * 25.0; // Costo mano de obra estimado
    
    totalCost += fuelCost + dietCost + hoursCost;
    estimatedBenefit = totalSelling - totalCost;
}

void MainWindow::updateMaterialTotal(int row) {
    if (row < 0 || row >= twMaterials->rowCount()) return;
    
    QTableWidgetItem *qtyItem = twMaterials->item(row, 1);
    QTableWidgetItem *sellPriceItem = twMaterials->item(row, 2);
    QTableWidgetItem *totalItem = twMaterials->item(row, 4);
    
    if (!qtyItem || !sellPriceItem || !totalItem) return;
    
    double qty = qtyItem->text().toDouble();
    double sellPrice = sellPriceItem->text().toDouble();
    double total = qty * sellPrice;
    
    totalItem->setText(QString::number(total, 'f', 2));
    
    // Recalcular totales
    onCalculate();
}