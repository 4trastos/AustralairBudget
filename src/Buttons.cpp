#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

void MainWindow::onBackToStart() {
    AustralairBudget *start = new AustralairBudget;
    start->show();
    this->close();
}

void MainWindow::onDeleteSelectedBudget() {
    auto *item = lwBudgets->currentItem();
    if (!item) return;

    int id = item->data(Qt::UserRole).toInt();
    if (QMessageBox::question(this, "Confirmar", "¿Deseas borrar el presupuesto seleccionado?") 
        == QMessageBox::Yes) 
    {
        QSqlQuery q(Database::instance());
        q.prepare("DELETE FROM budgets WHERE id = ?");
        q.addBindValue(id);
        if (!q.exec()) {
            QMessageBox::warning(this, "Error", "No se pudo borrar el presupuesto: " + q.lastError().text());
            return;
        }

        // Borrar materiales asociados
        QSqlQuery qm(Database::instance());
        qm.prepare("DELETE FROM materials WHERE budget_id = ?");
        qm.addBindValue(id);
        qm.exec();

        delete item; // quitar de la lista
        QMessageBox::information(this, "Borrado", "Presupuesto eliminado correctamente.");
    }

    setNextBudgetNumberAndDate();

}

void MainWindow::onNewMaterial() {
    int r = twMaterials->rowCount();
    twMaterials->insertRow(r);
    twMaterials->setItem(r,0, new QTableWidgetItem("Nombre"));
    twMaterials->setItem(r,1, new QTableWidgetItem("1"));
    twMaterials->setItem(r,2, new QTableWidgetItem("10.00"));
    twMaterials->setItem(r,3, new QTableWidgetItem("10.00"));
}

void MainWindow::onRemoveMaterial() {
    auto items = twMaterials->selectedItems();
    if (items.isEmpty()) return;
    int row = items.first()->row();
    twMaterials->removeRow(row);
}

// Desktop
/* void MainWindow::onDeleteFields() {
    // // --- Campos cliente ---
    // leClientName->clear();
    // leCompany->clear();
    // leContact->clear();
    // leAddress->clear();
    // lePhone->clear();
    // leEmail->clear();
    // leFecha->clear();
    // leCIF->clear();
    // leNumPresu->clear();

    // // --- Campos proyecto ---
    // sbMetros->setValue(0);
    // sbKM->setValue(0);
    // sbLitros->setValue(0);
    // sbHoras->setValue(0);
    // sbElevPortes->setValue(0);
    // spDietas->setValue(0);
    // spDiasDieta->setValue(0);
    // spDias->setValue(0);
    // spElevDia->setValue(0);
    // spOperarios->setValue(0);
    // leLocalidadObra->clear();
    // spElevPrecDia->setValue(0);
    // sbPrecioDiet->setValue(0);
    // spFurgonetas->setValue(0);
    // sbCosteFurgo->setValue(0);

    // // --- ComboBoxes ---
    // cbTipoLocal->setCurrentIndex(0);
    // cbTipoCubierta->setCurrentIndex(0);
    // cbZona->setCurrentIndex(0);
    // cbElevador->setCurrentIndex(0);
    // cbDietasYes->setCurrentIndex(0);
    // cbExtractor->setCurrentIndex(0);

    // // --- RadioButtons Zonas ---
    // rbCorta->setChecked(false);
    // rbMedia->setChecked(false);
    // rbLarga->setChecked(false);
    // --- Campos cliente (con verificaciones) ---
    if (leClientName) leClientName->clear();
    if (leCompany) leCompany->clear();
    if (leContact) leContact->clear();
    if (leAddress) leAddress->clear();
    if (lePhone) lePhone->clear();
    if (leEmail) leEmail->clear();
    if (leFecha) leFecha->clear();
    if (leCIF) leCIF->clear();
    if (leNumPresu) leNumPresu->clear();

    // --- Campos proyecto ---
    if (sbMetros) sbMetros->setValue(0);
    if (sbKM) sbKM->setValue(0);
    if (sbLitros) sbLitros->setValue(0);
    if (sbHoras) sbHoras->setValue(0);
    if (sbHorasViaje) sbHorasViaje->setValue(0);
    if (sbElevPortes) sbElevPortes->setValue(0);
    if (spDietas) spDietas->setValue(0);
    if (spDiasDieta) spDiasDieta->setValue(0);
    if (spDias) spDias->setValue(0);
    if (spElevDia) spElevDia->setValue(0);
    if (spOperarios) spOperarios->setValue(0);
    if (leLocalidadObra) leLocalidadObra->clear();
    if (spElevPrecDia) spElevPrecDia->setValue(0);
    if (sbPrecioDiet) sbPrecioDiet->setValue(0);
    if (spFurgonetas) spFurgonetas->setValue(0);
    if (sbCosteFurgo) sbCosteFurgo->setValue(0);

    // --- ComboBoxes ---
    if (cbTipoLocal) cbTipoLocal->setCurrentIndex(0);
    if (cbTipoCubierta) cbTipoCubierta->setCurrentIndex(0);
    if (cbZona) cbZona->setCurrentIndex(0);
    if (cbElevador) cbElevador->setCurrentIndex(0);
    if (cbDietasYes) cbDietasYes->setCurrentIndex(0);
    if (cbExtractor) cbExtractor->setCurrentIndex(0);

    // --- RadioButtons Zonas ---
    if (rbCorta) rbCorta->setChecked(false);
    if (rbMedia) rbMedia->setChecked(false);
    if (rbLarga) rbLarga->setChecked(false);

    // --- Tabla materiales ---
    if (twMaterials) {
        twMaterials->clearContents();
        twMaterials->setRowCount(0);
    }

    // --- Labels de totales ---
    if (lblTotalNoIVA) lblTotalNoIVA->setText("0.00 €");
    if (lblTotalConIVA) lblTotalConIVA->setText("0.00 €");
    if (lblCostoEstimado) lblCostoEstimado->setText("0.00 €");
    if (lblBeneficioEstimado) lblBeneficioEstimado->setText("0.00 €");
    if (lblDesviacionPVP) lblDesviacionPVP->setText("N/A €");

    // --- Estado interno ---
    currentBudgetId = 0;
    currentStatus = "Abierta";

    setNextBudgetNumberAndDate();

    QMessageBox::information(this, "Campos borrados", "Todos los campos se han limpiado correctamente.");
} */


// APP

void MainWindow::onDeleteFields() {
    // --- Campos cliente (con verificaciones) ---
    if (leClientName) leClientName->clear();
    if (leCompany) leCompany->clear();
    if (leContact) leContact->clear();
    if (leAddress) leAddress->clear();
    if (lePhone) lePhone->clear();
    if (leEmail) leEmail->clear();
    if (leFecha) leFecha->clear();
    if (leCIF) leCIF->clear();
    if (leNumPresu) leNumPresu->clear();

    // --- Campos proyecto ---
    if (sbMetros) sbMetros->setValue(0);
    if (sbKM) sbKM->setValue(0);
    if (sbLitros) sbLitros->setValue(0);
    if (sbHoras) sbHoras->setValue(0);
    if (sbHorasViaje) sbHorasViaje->setValue(0);
    if (sbElevPortes) sbElevPortes->setValue(0);
    if (spDietas) spDietas->setValue(0);
    if (spDiasDieta) spDiasDieta->setValue(0);
    if (spDias) spDias->setValue(0);
    if (spElevDia) spElevDia->setValue(0);
    if (spOperarios) spOperarios->setValue(0);
    if (leLocalidadObra) leLocalidadObra->clear();
    if (spElevPrecDia) spElevPrecDia->setValue(0);
    if (sbPrecioDiet) sbPrecioDiet->setValue(0);
    if (spFurgonetas) spFurgonetas->setValue(0);
    if (sbCosteFurgo) sbCosteFurgo->setValue(0);

    // --- ComboBoxes ---
    if (cbTipoLocal) cbTipoLocal->setCurrentIndex(0);
    if (cbTipoCubierta) cbTipoCubierta->setCurrentIndex(0);
    if (cbZona) cbZona->setCurrentIndex(0);
    if (cbElevador) cbElevador->setCurrentIndex(0);
    if (cbDietasYes) cbDietasYes->setCurrentIndex(0);
    if (cbExtractor) cbExtractor->setCurrentIndex(0);

    // --- RadioButtons Zonas ---
    if (rbCorta) rbCorta->setChecked(false);
    if (rbMedia) rbMedia->setChecked(false);
    if (rbLarga) rbLarga->setChecked(false);

    // --- Tabla materiales ---
    if (twMaterials) {
        twMaterials->clearContents();
        twMaterials->setRowCount(0);
    }

    // --- Labels de totales ---
    if (lblTotalNoIVA) lblTotalNoIVA->setText("0.00 €");
    if (lblTotalConIVA) lblTotalConIVA->setText("0.00 €");
    if (lblCostoEstimado) lblCostoEstimado->setText("0.00 €");
    if (lblBeneficioEstimado) lblBeneficioEstimado->setText("0.00 €");
    if (lblDesviacionPVP) lblDesviacionPVP->setText("N/A €");

    // --- Estado interno ---
    currentBudgetId = 0;
    currentStatus = "Abierta";

    setNextBudgetNumberAndDate();

    QMessageBox::information(this, "Campos borrados", "Todos los campos se han limpiado correctamente.");
}