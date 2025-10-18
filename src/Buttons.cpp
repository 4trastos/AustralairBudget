#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"

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

void MainWindow::onDeleteFields() {
    // --- Campos cliente ---
    leClientName->clear();
    leCompany->clear();
    leContact->clear();
    leAddress->clear();
    lePhone->clear();
    leEmail->clear();

    // --- Campos proyecto ---
    sbMetros->setValue(0);
    sbKM->setValue(0);
    sbLitros->setValue(0);
    sbHoras->setValue(0);
    spDietas->setValue(0);
    spDiasDieta->setValue(0);
    spDias->setValue(0);
    leLocalidadObra->clear();

    // --- ComboBoxes ---
    cbTipoLocal->setCurrentIndex(0);
    cbTipoCubierta->setCurrentIndex(0);
    cbZona->setCurrentIndex(0);

    // --- RadioButtons dietas ---
    //if (rbDietasNo) rbDietasNo->setChecked(true);  // resetea a "No"

    // --- Tabla materiales ---
    if (twMaterials) {
        twMaterials->clearContents();
        twMaterials->setRowCount(0);
    }

    // --- Labels de totales ---
    if (lblTotalNoIVA) lblTotalNoIVA->setText("0.00 €");
    if (lblTotalConIVA) lblTotalConIVA->setText("0.00 €");

    // --- Estado interno ---
    currentBudgetId = 0;
    currentStatus = "Abierta";

    QMessageBox::information(this, "Campos borrados", "Todos los campos se han limpiado correctamente.");
}
