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