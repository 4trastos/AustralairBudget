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

/* void MainWindow::onToggleStatus()
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
 */

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

    // ⬇️ BLOQUE AÑADIDO: Aplicar el bloqueo/desbloqueo inmediato si es el presupuesto actual
    if (id == currentBudgetId) {
        currentBudgetStatus = nuevoEstado;
        currentStatus = nuevoEstado;
        toggleInputFields(nuevoEstado == "abierta");
    }
    
    // Actualizar visualmente el texto del botón (se aplica al que está en el formulario)
    QPushButton *btnCloseProject = findChild<QPushButton*>("btnCloseProject");
    if (btnCloseProject) {
        btnCloseProject->setText((nuevoEstado == "abierta") ? "  Cerrar obra  " : "  Reabrir obra  ");
    }

    // Actualizar el texto en la lista (Sin cambios)
    QString currentText = item->text();
    QStringList parts = currentText.split("  |  ");
    if (parts.size() >= 1) {
        QString primeraParte = parts[0];
        QString segundaParte = (parts.size() > 1) ? parts[1].split("(")[0].trimmed() : "";
        
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

/* void MainWindow::onCloseProject()
{
    if (currentBudgetId <= 0) {
        QMessageBox::warning(this, "Error", "Debes cargar un presupuesto antes de duplicarlo para cierre de obra.");
        return;
    }

    int originalBudgetId = currentBudgetId;
    int originalClientId;
    QSqlDatabase d = Database::instance();
    
    // Obtener Client ID y verificar estado del presupuesto original
    QSqlQuery qOriginal(d);
    qOriginal.prepare("SELECT client_id, status FROM budgets WHERE id = ?");
    qOriginal.addBindValue(originalBudgetId);
    if (!qOriginal.exec() || !qOriginal.next()) {
        QMessageBox::critical(this, "Error DB", "No se pudo encontrar el presupuesto original.");
        return;
    }
    originalClientId = qOriginal.value(0).toInt();
    QString statusOriginal = qOriginal.value(1).toString().toLower();

    if (statusOriginal == "cerrada") {
        QMessageBox::information(this, "Obra ya cerrada", "La obra ya ha sido marcada como cerrada. Debes usar el botón 'Reabrir obra' en la lista de presupuestos si deseas re-editar el original.");
        return;
    }

    if (!d.transaction()) {
        QMessageBox::critical(this, "DB Error", "No se pudo iniciar la transacción.");
        return;
    }

    // 1. Duplicar Cliente (para que el nuevo presupuesto apunte a un registro independiente)
    // Usaremos los datos cargados en la interfaz (que son los del cliente original)
    QSqlQuery qClient(d);
    qClient.prepare(R"(
        INSERT INTO clients(budget,name,company,cif,contact,address,phone,email,fecha)
        VALUES(?,?,?,?,?,?,?,?,?)
    )");
    qClient.addBindValue(leNumPresu->text());
    qClient.addBindValue(leClientName->text());
    qClient.addBindValue(leCompany->text());
    qClient.addBindValue(leCIF->text());
    qClient.addBindValue(leContact->text());
    qClient.addBindValue(leAddress->text());
    qClient.addBindValue(lePhone->text());
    qClient.addBindValue(leEmail->text());
    qClient.addBindValue(leFecha->text());

    if (!qClient.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al duplicar cliente: " + qClient.lastError().text());
        return;
    }
    int newClientId = qClient.lastInsertId().toInt();
    
    // 2. Duplicar Presupuesto (copiando todos los campos del original)
    QSqlQuery qBudget(d);
    // Seleccionar todos los campos excepto 'id', 'client_id', y 'status' del original
    qBudget.prepare("SELECT tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, dias_elevador, precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, operarios, dias_trabajo, horas, horas_viaje, base_price, total_no_iva, total_con_iva FROM budgets WHERE id = ?");
    qBudget.addBindValue(originalBudgetId);

    if (!qBudget.exec() || !qBudget.next()) {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "No se pudo obtener datos del presupuesto original para duplicar.");
        return;
    }

    QSqlQuery qNewBudget(d);
    // Sentencia INSERT con los 24 campos de la tabla budgets (client_id + 22 campos + status)
    qNewBudget.prepare(R"(
        INSERT INTO budgets(
            client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, dias_elevador, 
            precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, operarios, dias_trabajo, 
            horas, horas_viaje, base_price, total_no_iva, total_con_iva, status
        ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
    )");
    
    qNewBudget.addBindValue(newClientId); // ⬅️ Nuevo Client ID
    
    // Copiar los 22 campos restantes del presupuesto
    for (int i = 0; i < 22; ++i) { 
        qNewBudget.addBindValue(qBudget.value(i));
    }
    
    qNewBudget.addBindValue("abierta"); // ⬅️ Nuevo Presupuesto ABIERTO por defecto

    if (!qNewBudget.exec()) {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "Error al insertar presupuesto duplicado: " + qNewBudget.lastError().text());
        return;
    }
    int newBudgetId = qNewBudget.lastInsertId().toInt();

    // 3. Duplicar Materiales
    QSqlQuery qMat(d);
    qMat.prepare("SELECT name, quantity, unit_price, cost_price FROM materials WHERE budget_id = ?");
    qMat.addBindValue(originalBudgetId);

    if (qMat.exec()) {
        QSqlQuery qNewMat(d);
        qNewMat.prepare("INSERT INTO materials(budget_id, name, quantity, unit_price, cost_price) VALUES (?, ?, ?, ?, ?)");

        while (qMat.next()) {
            qNewMat.addBindValue(newBudgetId); // ⬅️ Nuevo Budget ID
            qNewMat.addBindValue(qMat.value(0));
            qNewMat.addBindValue(qMat.value(1));
            qNewMat.addBindValue(qMat.value(2));
            qNewMat.addBindValue(qMat.value(3));
            if (!qNewMat.exec()) {
                d.rollback();
                qDebug() << "Error duplicando material:" << qNewMat.lastError().text();
                return;
            }
        }
    } else {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "Error al leer materiales originales.");
        return;
    }

    // 4. Marcar el Presupuesto Original como CERRADO
    QSqlQuery qCloseOriginal(d);
    qCloseOriginal.prepare("UPDATE budgets SET status = 'cerrada' WHERE id = ?");
    qCloseOriginal.addBindValue(originalBudgetId);
    if (!qCloseOriginal.exec()) {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "Error al cerrar el presupuesto original.");
        return;
    }

    // 5. Finalizar transacción
    if (!d.commit()) {
        QMessageBox::critical(this, "DB Error", "Error al confirmar la transacción de cierre de obra.");
        return;
    }

    // 6. Cargar el nuevo presupuesto duplicado en la interfaz y actualizar la lista
    QMessageBox::information(this, "Obra Abierta", QString("Presupuesto original #%1 cerrado. Se ha creado la copia de trabajo #%2 para registrar la desviación.").arg(originalBudgetId).arg(newBudgetId));
    refreshBudgetsList();
    
    // Seleccionar y cargar el nuevo presupuesto
    for (int i = 0; i < lwBudgets->count(); ++i) {
        QListWidgetItem *item = lwBudgets->item(i);
        if (item->data(Qt::UserRole).toInt() == newBudgetId) {
            lwBudgets->setCurrentItem(item);
            onLoadSelectedBudget(); // Carga la copia abierta para su edición
            break;
        }
    }

    // Necesitamos guardar el ID del original para la desviación
    // Usaremos un nuevo miembro privado para esto: 'originalBudgetIdForDesviacion'
    originalBudgetIdForDesviacion = originalBudgetId;
} */

void MainWindow::onCloseProject()
{
    if (currentBudgetId <= 0) {
        QMessageBox::warning(this, "Error", "Debes cargar un presupuesto antes de duplicarlo para cierre de obra.");
        return;
    }

    int originalBudgetId = currentBudgetId;
    
    QSqlDatabase d = Database::instance();
    
    // Obtener y verificar estado
    QSqlQuery qOriginal(d);
    qOriginal.prepare("SELECT client_id, status FROM budgets WHERE id = ?");
    qOriginal.addBindValue(originalBudgetId);
    if (!qOriginal.exec() || !qOriginal.next()) {
        QMessageBox::critical(this, "Error DB", "No se pudo encontrar el presupuesto original.");
        return;
    }
    int originalClientId = qOriginal.value(0).toInt();
    (void)originalClientId; // ⬅️ SOLUCIÓN PARA LA ADVERTENCIA UNUSED VARIABLE
    QString statusOriginal = qOriginal.value(1).toString().toLower();

    if (statusOriginal == "cerrada") {
        QMessageBox::information(this, "Obra ya cerrada", "La obra ya ha sido marcada como cerrada. Debes usar el botón 'Reabrir obra' en la lista de presupuestos si deseas re-editar el original.");
        return;
    }

    if (!d.transaction()) {
        QMessageBox::critical(this, "DB Error", "No se pudo iniciar la transacción.");
        return;
    }

    // 1. Duplicar Cliente (usando los datos de la interfaz, que son los del cliente original)
    QSqlQuery qClient(d);
    qClient.prepare(R"(
        INSERT INTO clients(budget,name,company,cif,contact,address,phone,email,fecha)
        VALUES(?,?,?,?,?,?,?,?,?)
    )");
    qClient.addBindValue(leNumPresu->text()); // Usamos los datos actuales de la interfaz
    qClient.addBindValue(leClientName->text());
    qClient.addBindValue(leCompany->text());
    qClient.addBindValue(leCIF->text());
    qClient.addBindValue(leContact->text());
    qClient.addBindValue(leAddress->text());
    qClient.addBindValue(lePhone->text());
    qClient.addBindValue(leEmail->text());
    qClient.addBindValue(leFecha->text());

    if (!qClient.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al duplicar cliente: " + qClient.lastError().text());
        return;
    }
    int newClientId = qClient.lastInsertId().toInt();
    
    // 2. Duplicar Presupuesto (copiando todos los campos del original)
    QSqlQuery qBudget(d);
    // Campos: tipo_local, metros... status (23 campos después del ID)
    qBudget.prepare("SELECT tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, dias_elevador, precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, operarios, dias_trabajo, horas, horas_viaje, base_price, total_no_iva, total_con_iva FROM budgets WHERE id = ?");
    qBudget.addBindValue(originalBudgetId);

    if (!qBudget.exec() || !qBudget.next()) {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "No se pudo obtener datos del presupuesto original para duplicar.");
        return;
    }

    QSqlQuery qNewBudget(d);
    // 24 campos a insertar
    qNewBudget.prepare(R"(
        INSERT INTO budgets(
            client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, dias_elevador, 
            precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, operarios, dias_trabajo, 
            horas, horas_viaje, base_price, total_no_iva, total_con_iva, status
        ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
    )");
    
    qNewBudget.addBindValue(newClientId); // ⬅️ Nuevo Client ID (1)
    
    // Copiar los 22 campos restantes del presupuesto
    for (int i = 0; i < 22; ++i) { 
        qNewBudget.addBindValue(qBudget.value(i)); // (2 a 23)
    }
    
    qNewBudget.addBindValue("abierta"); // ⬅️ Sobreescribir el estado a ABIERTA (24)

    if (!qNewBudget.exec()) {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "Error al insertar presupuesto duplicado: " + qNewBudget.lastError().text());
        return;
    }
    int newBudgetId = qNewBudget.lastInsertId().toInt();

    // 3. Duplicar Materiales
    QSqlQuery qMat(d);
    qMat.prepare("SELECT name, quantity, unit_price, cost_price FROM materials WHERE budget_id = ?");
    qMat.addBindValue(originalBudgetId);

    if (qMat.exec()) {
        QSqlQuery qNewMat(d);
        qNewMat.prepare("INSERT INTO materials(budget_id, name, quantity, unit_price, cost_price) VALUES (?, ?, ?, ?, ?)");

        while (qMat.next()) {
            qNewMat.addBindValue(newBudgetId); // ⬅️ Nuevo Budget ID
            qNewMat.addBindValue(qMat.value(0));
            qNewMat.addBindValue(qMat.value(1));
            qNewMat.addBindValue(qMat.value(2));
            qNewMat.addBindValue(qMat.value(3));
            if (!qNewMat.exec()) {
                d.rollback();
                qDebug() << "Error duplicando material:" << qNewMat.lastError().text();
                return;
            }
        }
    } else {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "Error al leer materiales originales.");
        return;
    }

    // 4. Marcar el Presupuesto Original como CERRADO
    QSqlQuery qCloseOriginal(d);
    qCloseOriginal.prepare("UPDATE budgets SET status = 'cerrada' WHERE id = ?");
    qCloseOriginal.addBindValue(originalBudgetId);
    if (!qCloseOriginal.exec()) {
        d.rollback();
        QMessageBox::critical(this, "Error DB", "Error al cerrar el presupuesto original.");
        return;
    }

    // 5. Finalizar transacción
    if (!d.commit()) {
        QMessageBox::critical(this, "DB Error", "Error al confirmar la transacción de cierre de obra.");
        return;
    }

    // 6. Cargar el nuevo presupuesto duplicado en la interfaz y actualizar la lista
    QMessageBox::information(this, "Obra Abierta", QString("Presupuesto original #%1 cerrado. Se ha creado la copia de trabajo #%2 para registrar la desviación.").arg(originalBudgetId).arg(newBudgetId));
    refreshBudgetsList();
    
    // Seleccionar y cargar el nuevo presupuesto
    for (int i = 0; i < lwBudgets->count(); ++i) {
        QListWidgetItem *item = lwBudgets->item(i);
        if (item->data(Qt::UserRole).toInt() == newBudgetId) {
            lwBudgets->setCurrentItem(item);
            onLoadSelectedBudget(); // Carga la copia abierta para su edición
            break;
        }
    }

    // Guardar el ID del original para la desviación
    originalBudgetIdForDesviacion = originalBudgetId;
}