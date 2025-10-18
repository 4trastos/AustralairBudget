#include "AustralairBudget.hpp"
#include "MainWindow.hpp"
#include "Database.hpp"

void MainWindow::onToggleStatus()
{
    auto *item = lwBudgets->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Sin selección", "Selecciona un presupuesto antes de cerrar o abrir una obra.");
        return;
    }

    int id = item->data(Qt::UserRole).toInt();
    QSqlQuery q(Database::instance());

    // Obtener el estado actual
    q.prepare("SELECT status FROM budgets WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) {
        QMessageBox::warning(this, "Error", "No se pudo leer el estado actual del presupuesto.");
        return;
    }

    QString estadoActual = q.value(0).toString();
    QString nuevoEstado = (estadoActual == "abierta") ? "cerrada" : "abierta";

    // Actualizar en la DB
    QSqlQuery update(Database::instance());
    update.prepare("UPDATE budgets SET status = ? WHERE id = ?");
    update.addBindValue(nuevoEstado);
    update.addBindValue(id);

    if (!update.exec()) {
        QMessageBox::critical(this, "Error", "No se pudo actualizar el estado: " + update.lastError().text());
        return;
    }

    // Actualizar visualmente el texto del botón
    QPushButton *btnCloseProject = findChild<QPushButton*>("btnCloseProject");
    if (btnCloseProject)
        btnCloseProject->setText((nuevoEstado == "abierta") ? "Cerrar obra" : "Reabrir obra");

    // Mostrar mensaje
    QMessageBox::information(this, "Estado actualizado",
        QString("El presupuesto %1 ahora está %2.")
        .arg(id)
        .arg(nuevoEstado == "abierta" ? "ABIERTO" : "CERRADO"));

    // Si quieres refrescar la lista (para marcar visualmente el estado):
    refreshBudgetsList();
}

void MainWindow::refreshBudgetsList()
{
    if (!lwBudgets)
        return;

    lwBudgets->clear();

    QSqlQuery q(Database::instance());
    if (!q.exec("SELECT id, created_at, status FROM budgets ORDER BY created_at DESC")) {
        qDebug() << "Error al recargar presupuestos:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        int id = q.value(0).toInt();
        QString ts = q.value(1).toString();
        QString st = q.value(2).toString();

        QString label = QString("%1 - %2 (%3)")
                            .arg(id)
                            .arg(ts)
                            .arg(st.toUpper());

        QListWidgetItem *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, id);

        // Colorear según estado
        if (st == "cerrada") {
            item->setBackground(QColor("#d9d9d9"));  // gris claro
            item->setForeground(QColor("#666666"));
        } else {
            item->setBackground(QColor("#e8f5e9"));  // verde suave
            item->setForeground(QColor("#2e7d32"));
        }

        lwBudgets->addItem(item);
    }

    if (lwBudgets->count() == 0)
        lwBudgets->addItem("No hay presupuestos guardados.");
}
