#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"
#include "MaterialsWindow.hpp"

void MainWindow::onCalculate()
{
    if (!lblTotalNoIVA || !lblTotalConIVA || !lblCostoEstimado || !lblBeneficioEstimado) {
        return;
    }

    // 1. Cargar precios desde la Base de Datos (settings)
    double ivaPct = getSettingDouble("iva_pct", 21.0);
    double fuelPriceLiter = getSettingDouble("fuel_liter_price", 1.80);
    double horaPrice = getSettingDouble("price_base", 80.0); // Precio de VENTA de la hora
    double dietaPrice = getSettingDouble("dieta_price", 150.0); // Precio de VENTA de la dieta

    // 2. Inicializar totales de venta y costo (basado en onCalculate/calculateCostsAndBenefits)
    double totalNoImprevistos = 0.0; // Total de venta sin el 10%
    double totalCost = 0.0;          // Costo total real (calculado a continuación)

    // --- Sumar materiales (La venta y el costo se calculan aquí directamente para simplificar el flujo) ---
    for (int r = 0; r < twMaterials->rowCount(); ++r)
    {
        QTableWidgetItem *qtyItem = twMaterials->item(r, 1);
        QTableWidgetItem *priceSellItem = twMaterials->item(r, 2); // Precio Venta
        QTableWidgetItem *priceCostItem = twMaterials->item(r, 3); // Precio Compra/Costo
        QTableWidgetItem *totalSellItem = twMaterials->item(r, 4); // Total Venta

        double qty = qtyItem ? qtyItem->text().toDouble() : 0.0;
        double priceSell = priceSellItem ? priceSellItem->text().toDouble() : 0.0;
        double priceCost = priceCostItem ? priceCostItem->text().toDouble() : 0.0;

        double lineSell = qty * priceSell;
        
        if (totalSellItem)
            totalSellItem->setText(QString::number(lineSell, 'f', 2));
        
        // Sumar a la VENTA SIN imprevistos
        totalNoImprevistos += lineSell;
        
        // Sumar al COSTO total
        totalCost += qty * priceCost;
    }

    // --- Elevación (Venta y Costo) ---
    if (cbElevador->currentText() == "Si") {
        // Venta (Cobramos al cliente: Portes + (Días * Precio x Día))
        double elevPortes = sbElevPortes->value();
        int elevDias = spElevDia->value();
        int elevPrecioDia = spElevPrecDia->value();
        
        double ventaElevacion = elevPortes + (elevDias * elevPrecioDia);
        totalNoImprevistos += ventaElevacion;

        // Costo (Asumimos que el costo real es solo el coste administrativo de los portes 
        // más un costo fijo por día, ya que el precio/día es VENTA)
        double costElevacionPortes = getSettingDouble("cost_elevacion_portes", elevPortes); // Asumimos que el costo es similar al porte
        double costElevacionDia = getSettingDouble("cost_elevacion_dia", 5.0); // Nuevo setting: Coste interno de gestionar la elevación por día

        totalCost += costElevacionPortes;
        totalCost += elevDias * costElevacionDia; 
    }

    // --- Costos Operacionales (Venta y Costo) ---
    
    // Combustible
    double litrosConsumidos = sbLitros->value();
    totalNoImprevistos += litrosConsumidos * fuelPriceLiter; // Venta: basado en precio/litro
    totalCost += litrosConsumidos * fuelPriceLiter;          // Costo: asumimos que el coste es igual al precio de venta del litro

    // Dietas
    // Obtener el precio de venta por dieta (de los settings O del spinbox sbPrecioDiet)
    double precioVentaDieta = dietaPrice; // Valor por defecto de settings (antiguo)
    if (sbPrecioDiet->value() > 0) {
        precioVentaDieta = sbPrecioDiet->value(); // Usar el valor del spinbox si se ha establecido
    }
    
    // El cálculo de dietas solo aplica si cbDietasYes está en "Si"
    if (cbDietasYes->currentText() == "Si") {
        int numOperariosDieta = spDietas->value();
        int numDiasDieta = spDiasDieta->value();
        int numDietasTotales = numOperariosDieta * numDiasDieta;

        double costPerDieta = getSettingDouble("cost_per_dieta", 150.0); // Costo estimado real
        
        // Venta: Total de dietas cobradas al cliente
        totalNoImprevistos += numDietasTotales * precioVentaDieta; 
        
        // Costo: Costo real incurrido
        totalCost += numDietasTotales * costPerDieta;        
    }

    // --- Horas de Trabajo Y Viaje (Bloque CORREGIDO Y AMPLIADO) ---
    
    // Venta: Horas de obra
    double horasBase = sbHoras->value();
    double horasViajeInput = sbHorasViaje->value(); // <--- Nuevo campo
    double horasIncremento = 0.0; // Incremento por zona (Zona Centro)
    
    if (cbZona->currentText() == "Zona Centro") {
        if (rbCorta->isChecked()) horasIncremento = 0.30 * 2;
        else if (rbMedia->isChecked()) horasIncremento = 1.0 * 2;
        else if (rbLarga->isChecked()) horasIncremento = 1.5 * 2;
    }
    
    // Total de horas cobradas al cliente (Obra + Incremento + Viaje)
    double horasTotalesVenta = horasBase + horasIncremento + horasViajeInput; 
    totalNoImprevistos += horasTotalesVenta * horaPrice; // Venta: basado en precio/hora (se cobra igual)

    // Costo: Horas de obra + Horas de viaje (sin el incremento por zona)
    double costPerHourObra = getSettingDouble("cost_per_hour", 65.0); // Costo hora de obra
    double costPerHourViaje = getSettingDouble("cost_hour_viaje", 65.0); // Costo hora de viaje
    
    double costoHorasObra = horasBase * costPerHourObra;
    double costoHorasViaje = horasViajeInput * costPerHourViaje;
    
    totalCost += costoHorasObra;
    totalCost += costoHorasViaje;

    // Furgonetas
    int numFurgonetas = spFurgonetas->value();
    double costVanDay = getSettingDouble("cost_van_day", 95.0);
    
    if (numFurgonetas > 0 && spDias->value() > 0) {
        // Venta: SE ELIMINA LA LÍNEA DE VENTA. La furgoneta ya no se cobra al cliente.
        // totalNoImprevistos += numFurgonetas * precioVentaFurgo * spDias->value(); // ¡ELIMINADA!

        // Costo: Se calcula el costo real por día de furgoneta * días de obra.
        totalCost += numFurgonetas * costVanDay * spDias->value();
    }

    // ====================================================
    // 3. APLICACIÓN DE IMPREVISTOS
    double totalFinal = totalNoImprevistos;
    if (isImprevistosApplied) {
        totalFinal *= 1.10;
    }
    // ====================================================

    // 4. Calcular el Beneficio (Precio Final - Costo Total)
    double estimatedBenefit = totalFinal - totalCost;

    // 5. Total con IVA
    lblIVAPct->setText(QString("(%1%)").arg(QString::number(ivaPct, 'f', 2)));
    double totalConIva = totalFinal * (1.0 + ivaPct / 100.0);
    
    // 6. Actualizar Etiquetas
    lblTotalNoIVA->setText(QString::number(totalFinal, 'f', 2) + " €");
    lblTotalConIVA->setText(QString::number(totalConIva, 'f', 2) + " €");
    lblCostoEstimado->setText(QString::number(totalCost, 'f', 2) + " €");
    lblBeneficioEstimado->setText(QString::number(estimatedBenefit, 'f', 2) + " €");

    // Colorear beneficio
    if (estimatedBenefit >= 0) {
        lblBeneficioEstimado->setStyleSheet("color: green; font-weight: bold;");
    } else {
        lblBeneficioEstimado->setStyleSheet("color: red; font-weight: bold;");
    }
    
    // 7. Actualizar la visualización del IVA
    updateTotalsDisplay();
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
    double fuelCost = sbLitros->value() * 1.7; // Costo combustible real
    double dietCost = spDietas->value() * spDiasDieta->value() * 150.0; // Costo dietas estimado
    double hoursCost = sbHoras->value() * 65.0; // Costo mano de obra estimado
    
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

void MainWindow::onCalculateDesviation()
{
    // NO ES NECESARIO RECALCULAR. onCalculate() ya se ejecutó y actualizó el Beneficio.

    if (currentBudgetId <= 0) {
        QMessageBox::warning(this, "Error", "No hay un presupuesto cargado.");
        return;
    }
    
    // ... (Validaciones de estado y originalBudgetIdForDesviacion) ...

    QSqlDatabase d = Database::instance();
    
    // 1. Obtener PVP Original (Sin IVA) para la comparación
    QSqlQuery qOriginal(d);
    qOriginal.prepare("SELECT total_no_iva FROM budgets WHERE id = ?");
    qOriginal.addBindValue(originalBudgetIdForDesviacion);

    if (qOriginal.exec() && qOriginal.next()) {
        double totalOriginalNoIVA = qOriginal.value(0).toDouble(); 
        
        // El total final ejecutado se toma de la etiqueta actualizada por onCalculate()
        double totalFinalEjecutadoNoIVA = lblTotalNoIVA->text().replace(" €", "").toDouble();

        double desviacionPVP = totalFinalEjecutadoNoIVA - totalOriginalNoIVA;
        
        // 2. Actualizar el QLabel de Desviación (Resultado final del proceso)
        if (lblDesviacionPVP) {
            lblDesviacionPVP->setText(QString::number(desviacionPVP, 'f', 2) + " €");
            lblDesviacionPVP->setToolTip(QString("Inicial (Sin IVA): %1 €").arg(QString::number(totalOriginalNoIVA, 'f', 2)));
            lblDesviacionPVP->setStyleSheet(desviacionPVP >= 0 ? "color: green; font-weight: bold;" : "color: red; font-weight: bold;");
        }
    } else {
        QMessageBox::critical(this, "Error DB", "No se pudieron obtener los totales del presupuesto original.");
        return;
    }
    
    // 3. Marcar la copia de desviación como CERRADA
    QSqlQuery qCloseCopy(d);
    qCloseCopy.prepare("UPDATE budgets SET status = 'cerrada' WHERE id = ?");
    qCloseCopy.addBindValue(currentBudgetId);
    
    if (!qCloseCopy.exec()) {
        QMessageBox::critical(this, "Error DB", "Error al marcar la copia de obra como CERRADA.");
        return;
    }

    // 4. Aplicar el bloqueo, actualizar la lista y limpiar la referencia
    currentBudgetStatus = "cerrada";
    toggleInputFields(false);
    refreshBudgetsList();
    originalBudgetIdForDesviacion = 0;
    
    QMessageBox::information(this, "Obra Cerrada", "Desviación calculada y visible en el resumen. Obra ejecutada marcada como CERRADA.");
}

//*ho0sJ8!o)@*H3Oqb!8P7mwI