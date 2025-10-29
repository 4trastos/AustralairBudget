#include "AustralairBudget.hpp"
#include "MainWindow.hpp"
#include "Database.hpp"
#include "MaterialsWindow.hpp"

void MainWindow::refreshBudgetsList()
{
    if (!lwBudgets)
        return;

    lwBudgets->clear();

    QSqlQuery q(Database::instance());
    if (!q.exec(R"(
        SELECT b.id, c.name, b.created_at, b.status
        FROM budgets b
        LEFT JOIN clients c ON b.client_id = c.id
        ORDER BY b.created_at DESC
    )")) {
        qDebug() << "Error al recargar presupuestos:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        int id = q.value(0).toInt();
        QString clientName = q.value(1).toString();
        QString ts = q.value(2).toString();
        QString st = q.value(3).toString();

        QString estadoTexto = (st == "cerrada") ? "CERRADA" : "ABIERTA";
        QString emoji = (st == "cerrada") ? "●" : "○";
        
        QString label = QString("%1 - %2  |  %3  (%4) %5")
                            .arg(id)
                            .arg(clientName.isEmpty() ? "Sin cliente" : clientName)
                            .arg(ts)
                            .arg(estadoTexto)
                            .arg(emoji);

        QListWidgetItem *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, id);

        lwBudgets->addItem(item);
    }

    if (lwBudgets->count() == 0)
        lwBudgets->addItem("No hay presupuestos guardados.");
}

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
    QString textoEstado = (nuevoEstado == "abierta") ? "ABIERTA" : "CERRADA";
    QString emoji = (nuevoEstado == "cerrada") ? "●" : "○";

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
    if (btnCloseProject) {
        btnCloseProject->setText((nuevoEstado == "abierta") ? "Cerrar obra" : "Reabrir obra");
    }

    // Actualizar el texto en la lista
    QString currentText = item->text();
    
    // Extraer las partes fijas (ID, nombre cliente y fecha)
    QStringList parts = currentText.split("  |  ");
    if (parts.size() >= 1) {
        QString primeraParte = parts[0]; // "ID - Nombre Cliente"
        QString segundaParte = (parts.size() > 1) ? parts[1].split("(")[0].trimmed() : ""; // Fecha sin el estado
        
        // Reconstruir el texto con el nuevo estado y emoji
        QString newText = QString("%1  |  %2  (%3) %4")
                            .arg(primeraParte)
                            .arg(segundaParte)
                            .arg(textoEstado)
                            .arg(emoji);
        
        item->setText(newText);
    }

    // Mostrar mensaje
    QMessageBox::information(this, "Estado actualizado",
        QString("El presupuesto %1 ahora está %2.")
        .arg(id)
        .arg(textoEstado));
}
