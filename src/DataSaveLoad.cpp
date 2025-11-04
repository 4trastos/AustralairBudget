#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

bool MainWindow::updateExistingBudget(int budgetId)
{
    QSqlDatabase d = Database::instance();
    
    // Necesitamos el Client ID original para actualizar el cliente
    QSqlQuery clientQuery(d);
    clientQuery.prepare("SELECT client_id FROM budgets WHERE id = ?");
    clientQuery.addBindValue(budgetId);
    if (!clientQuery.exec() || !clientQuery.next()) {
        QMessageBox::warning(this, "DB", "No se pudo encontrar el Client ID para actualizar.");
        return false;
    }
    int clientId = clientQuery.value(0).toInt();

    if (!d.transaction()) return false;

    // 1. Actualizar cliente (UPDATE)
    QSqlQuery qc(d);
    qc.prepare(R"(
        UPDATE clients SET budget=?, name=?, company=?, cif=?, contact=?, address=?, phone=?, email=?, fecha=?
        WHERE id=?
    )");
    qc.addBindValue(leNumPresu->text());
    qc.addBindValue(leClientName->text());
    qc.addBindValue(leCompany->text());
    qc.addBindValue(leCIF->text());
    qc.addBindValue(leContact->text());
    qc.addBindValue(leAddress->text());
    qc.addBindValue(lePhone->text());
    qc.addBindValue(leEmail->text());
    qc.addBindValue(leFecha->text());
    qc.addBindValue(clientId);

    if (!qc.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al actualizar cliente: " + qc.lastError().text());
        return false;
    }

    // 2. Actualizar presupuesto (UPDATE) - ¡Aquí se PRESERVA currentBudgetStatus!
    double base = getSettingDouble("price_base", 10.0);
    double totalNoIva = lblTotalNoIVA->text().replace(" €", "").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €", "").toDouble();
    
    QSqlQuery qb(d);
    qb.prepare(R"(
        UPDATE budgets SET
            tipo_local=?, metros=?, tipo_cubierta=?, extraccion=?, elevador=?, portes=?, dias_elevador=?, 
            precio_dia=?, zona=?, dietas=?, dias_dieta=?, precio_dieta=?, furgonetas=?, coste_furgonetas=?, km=?, combustible=?, operarios=?, dias_trabajo=?, 
            horas=?, horas_viaje=?, base_price=?, total_no_iva=?, total_con_iva=?, status=?
        WHERE id=?
    )");
    
    // ... (BindValues del presupuesto, omitidos por espacio, pero deben ser 24) ...
    // Asegúrate de que los BindValues coincidan con el orden de las columnas en el UPDATE
    qb.addBindValue(cbTipoLocal->currentText());    
    qb.addBindValue(sbMetros->value());
    qb.addBindValue(cbTipoCubierta->currentText()); 
    qb.addBindValue(cbExtractor->currentText());    
    qb.addBindValue(cbElevador->currentText());     
    qb.addBindValue(sbElevPortes->value());         
    qb.addBindValue(spElevDia->value());            
    qb.addBindValue(spElevPrecDia->value());        
    qb.addBindValue(cbZona->currentText());         
    qb.addBindValue(spDietas->value());             
    qb.addBindValue(spDiasDieta->value());         
    qb.addBindValue(sbPrecioDiet->value());         
    qb.addBindValue(spFurgonetas->value());         
    qb.addBindValue(sbCosteFurgo->value());         
    qb.addBindValue(sbKM->value());                 
    qb.addBindValue(sbLitros->value());             
    qb.addBindValue(spOperarios->value());         
    qb.addBindValue(spDias->value());               
    qb.addBindValue(sbHoras->value());              
    qb.addBindValue(sbHorasViaje->value());
    qb.addBindValue(base);                          
    qb.addBindValue(totalNoIva);                    
    qb.addBindValue(totalConIva);
    qb.addBindValue(currentBudgetStatus); // ⬅️ PRESERVAMOS EL ESTADO ACTUAL
    qb.addBindValue(budgetId); 

    if (!qb.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al actualizar presupuesto: " + qb.lastError().text());
        return false;
    }

    // 3. Borrar y Guardar materiales
    // Borrar materiales viejos
    QSqlQuery qd(d);
    qd.prepare("DELETE FROM materials WHERE budget_id=?");
    qd.addBindValue(budgetId);
    if (!qd.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al borrar materiales antiguos: " + qd.lastError().text());
        return false;
    }

    // Insertar materiales nuevos (mismo código que INSERT)
    QSqlQuery qm(d);
    qm.prepare("INSERT INTO materials(budget_id,name,quantity,unit_price,cost_price) VALUES(?,?,?,?,?)");
    
    for (int r = 0; r < twMaterials->rowCount(); ++r) {
        // ... (Tu código para addBindValue del material) ...
        QString name = twMaterials->item(r, 0)->text();
        double qty = twMaterials->item(r, 1)->text().toDouble();
        double up  = twMaterials->item(r, 2)->text().toDouble();
        double cost = twMaterials->item(r, 3)->text().toDouble();

        qm.addBindValue(budgetId);
        qm.addBindValue(name);
        qm.addBindValue(qty);
        qm.addBindValue(up);
        qm.addBindValue(cost);
        
        if (!qm.exec())
            qDebug() << "Error re-saving material:" << qm.lastError().text();
    }
    
    // --- Finalizar transacción y aplicar bloqueo ---
    if (d.commit()) {
        
        // ⬇️ LÓGICA DE BLOQUEO (AÑADIDA AQUÍ) 
        // 4. Aplicar el bloqueo/desbloqueo a la interfaz
        bool isEnabled = (currentBudgetStatus == "abierta");
        toggleInputFields(isEnabled); 
        
        // 5. Actualizar el texto del botón "Cerrar/Reabrir obra"
        QPushButton *btnCloseProject = findChild<QPushButton*>("btnCloseProject");
        if (btnCloseProject) {
            btnCloseProject->setText(isEnabled ? "  Cerrar obra  " : "  Reabrir obra  ");
        }
        refreshBudgetsList();
        // ⬆️ FIN LÓGICA DE BLOQUEO
        
        return true; 
    } else {
        QMessageBox::critical(this, "DB Error", "Error al confirmar la transacción de actualización: " + d.lastError().text());
        return false;
    } 
}

void MainWindow::onSaveBudget()
{
    // Si currentBudgetId es > 0, estamos editando un presupuesto existente.
    if (currentBudgetId > 0) {
        // Llama a la nueva función de actualización
        if (updateExistingBudget(currentBudgetId)) {
            QMessageBox::information(this, "Guardado", "Presupuesto ID " + QString::number(currentBudgetId) + " actualizado correctamente.");
        }
        return;
    }

    // --- Si currentBudgetId es 0, es un presupuesto nuevo (INSERT) ---

    QSqlDatabase d = Database::instance();
    QSqlQuery q(d);
    
    // --- Comienzo de la transacción para asegurar atomicidad ---
    if (!d.transaction()) {
        QMessageBox::critical(this, "DB Error", "No se pudo iniciar la transacción.");
        return;
    }

    // 1. Guardar cliente (INSERT)
    q.prepare(R"(
        INSERT INTO clients(budget,name,company,cif,contact,address,phone,email,fecha)
        VALUES(?,?,?,?,?,?,?,?,?)
    )");
    // ... (Tu código original para addBindValue del cliente) ...
    q.addBindValue(leNumPresu->text());
    q.addBindValue(leClientName->text());
    q.addBindValue(leCompany->text());
    q.addBindValue(leCIF->text());
    q.addBindValue(leContact->text());
    q.addBindValue(leAddress->text());
    q.addBindValue(lePhone->text());
    q.addBindValue(leEmail->text());
    q.addBindValue(leFecha->text());
    
    if (!q.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al guardar cliente: " + q.lastError().text());
        return;
    }

    int clientId = q.lastInsertId().toInt();

    // 2. Guardar presupuesto (INSERT) - ¡Añadir el estado 'abierta'!
    double base = getSettingDouble("price_base", 10.0);
    double totalNoIva = lblTotalNoIVA->text().replace(" €", "").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €", "").toDouble();

    QSqlQuery qb(d);
    qb.prepare(R"(
        INSERT INTO budgets(
            client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, dias_elevador, 
            precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, operarios, dias_trabajo, 
            horas, horas_viaje, base_price, total_no_iva, total_con_iva, status 
        ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
    )");

    // ... (Tu código original para addBindValue del presupuesto) ...
    qb.addBindValue(clientId);                       
    qb.addBindValue(cbTipoLocal->currentText());    
    qb.addBindValue(sbMetros->value());             
    qb.addBindValue(cbTipoCubierta->currentText()); 
    qb.addBindValue(cbExtractor->currentText());    
    qb.addBindValue(cbElevador->currentText());     
    qb.addBindValue(sbElevPortes->value());         
    qb.addBindValue(spElevDia->value());            
    qb.addBindValue(spElevPrecDia->value());        
    qb.addBindValue(cbZona->currentText());         
    qb.addBindValue(spDietas->value());             
    qb.addBindValue(spDiasDieta->value());         
    qb.addBindValue(sbPrecioDiet->value());         
    qb.addBindValue(spFurgonetas->value());         
    qb.addBindValue(sbCosteFurgo->value());         
    qb.addBindValue(sbKM->value());                 
    qb.addBindValue(sbLitros->value());             
    qb.addBindValue(spOperarios->value());         
    qb.addBindValue(spDias->value());               
    qb.addBindValue(sbHoras->value());              
    qb.addBindValue(sbHorasViaje->value());         
    qb.addBindValue(base);                          
    qb.addBindValue(totalNoIva);                    
    qb.addBindValue(totalConIva);                  
    qb.addBindValue("abierta");                      // ⬅️ Nuevo estado por defecto
    
    if (!qb.exec()) {
        d.rollback();
        QMessageBox::warning(this, "DB", "Error al guardar presupuesto: " + qb.lastError().text());
        return;
    }

    int budgetId = qb.lastInsertId().toInt();

    // 3. Guardar materiales (INSERT)
    QSqlQuery qm(d);
    qm.prepare("INSERT INTO materials(budget_id,name,quantity,unit_price,cost_price) VALUES(?,?,?,?,?)");
    
    // ... (Tu código original para guardar materiales) ...
    for (int r = 0; r < twMaterials->rowCount(); ++r) {
        QString name = twMaterials->item(r, 0)->text();
        double qty = twMaterials->item(r, 1)->text().toDouble();
        double up  = twMaterials->item(r, 2)->text().toDouble();
        double cost = twMaterials->item(r, 3)->text().toDouble();

        qm.addBindValue(budgetId);
        qm.addBindValue(name);
        qm.addBindValue(qty);
        qm.addBindValue(up);
        qm.addBindValue(cost);

        if (!qm.exec())
            qDebug() << "Error saving material:" << qm.lastError().text();
    }
    
    // --- Finalizar transacción ---
    if (d.commit()) {
        // 4. Actualizar lista de presupuestos (solo visual)
        QString clientName = leClientName->text();
        QString fecha = QDateTime::currentDateTime().toString("ddd MMM dd HH:mm:ss yyyy");
        QString estado = "ABIERTA"; // Estado por defecto
        
        QString label = QString("%1 - %2  |  %3  (%4) %5")
                            .arg(budgetId)
                            .arg(clientName.isEmpty() ? "Sin cliente" : clientName)
                            .arg(fecha)
                            .arg(estado)
                            .arg("○"); // Emoji de abierta

        auto *it = new QListWidgetItem(label);
        it->setData(Qt::UserRole, budgetId);
        lwBudgets->insertItem(0, it);

        currentBudgetId = budgetId; // ¡Establece el ID recién creado!
        currentBudgetStatus = "abierta";

        QMessageBox::information(this, "Guardado", "Presupuesto guardado con id " + QString::number(budgetId));
    } else {
        QMessageBox::critical(this, "DB Error", "Error al confirmar la transacción: " + d.lastError().text());
    }
}

/* void MainWindow::onSaveBudget()
{
    QSqlDatabase d = Database::instance();
    QSqlQuery q(d);

    // --- Guardar cliente ---
    q.prepare(R"(
        INSERT INTO clients(budget,name,company,cif,contact,address,phone,email,fecha)
        VALUES(?,?,?,?,?,?,?,?,?)
    )");
    q.addBindValue(leNumPresu->text());
    q.addBindValue(leClientName->text());
    q.addBindValue(leCompany->text());
    q.addBindValue(leCIF->text());
    q.addBindValue(leContact->text());
    q.addBindValue(leAddress->text());
    q.addBindValue(lePhone->text());
    q.addBindValue(leEmail->text());
    q.addBindValue(leFecha->text());

    if (!q.exec()) {
        QMessageBox::warning(this, "DB", "Error al guardar cliente: " + q.lastError().text());
        return;
    }

    int clientId = q.lastInsertId().toInt();

    // --- Guardar presupuesto ---
    double base = getSettingDouble("price_base", 10.0);
    double totalNoIva = lblTotalNoIVA->text().replace(" €", "").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €", "").toDouble();

    QSqlQuery qb(d);
    qb.prepare(R"(
        INSERT INTO budgets(
            client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, dias_elevador, 
            precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, operarios, dias_trabajo, 
            horas, horas_viaje, base_price, total_no_iva, total_con_iva
        ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
    )");

    qb.addBindValue(clientId);                        // client_id
    qb.addBindValue(cbTipoLocal->currentText());     // tipo_local
    qb.addBindValue(sbMetros->value());              // metros
    qb.addBindValue(cbTipoCubierta->currentText());  // tipo_cubierta
    qb.addBindValue(cbExtractor->currentText());     // Extracción
    qb.addBindValue(cbElevador->currentText());      // elevador
    qb.addBindValue(sbElevPortes->value());          // portes
    qb.addBindValue(spElevDia->value());             // dias_elevador
    qb.addBindValue(spElevPrecDia->value());         // Precio por día
    qb.addBindValue(cbZona->currentText());          // zona
    qb.addBindValue(spDietas->value());             // dietas
    qb.addBindValue(spDiasDieta->value());          // dias_dieta
    qb.addBindValue(sbPrecioDiet->value());         // Precio dieta X día
    //qb.addBindValue(leLocalidadObra ? leLocalidadObra->text() : QString()); // localidad
    qb.addBindValue(spFurgonetas->value());         // Furgonetas
    qb.addBindValue(sbCosteFurgo->value());         // Coste Furgonetas
    qb.addBindValue(sbKM->value());                  // km
    qb.addBindValue(sbLitros->value());             // combustible
    qb.addBindValue(spOperarios->value());          // operarios
    qb.addBindValue(spDias->value());               // dias_trabajo
    qb.addBindValue(sbHoras->value());              // horas
    qb.addBindValue(sbHorasViaje->value());         // Horas viaje
    qb.addBindValue(base);                           // base_price
    qb.addBindValue(totalNoIva);                     // total_no_iva
    qb.addBindValue(totalConIva);                   // total_con_iva

    if (!qb.exec()) {
        QMessageBox::warning(this, "DB", "Error al guardar presupuesto: " + qb.lastError().text());
        return;
    }

    int budgetId = qb.lastInsertId().toInt();

    // --- Guardar materiales ---
    QSqlQuery qm(d);
    qm.prepare("INSERT INTO materials(budget_id,name,quantity,unit_price,cost_price) VALUES(?,?,?,?,?)");

    for (int r = 0; r < twMaterials->rowCount(); ++r) {
        QString name = twMaterials->item(r, 0)->text();
        double qty = twMaterials->item(r, 1)->text().toDouble();
        double up  = twMaterials->item(r, 2)->text().toDouble();  // Precio venta
        double cost = twMaterials->item(r, 3)->text().toDouble(); // Precio compra (columna 3)

        qm.addBindValue(budgetId);
        qm.addBindValue(name);
        qm.addBindValue(qty);
        qm.addBindValue(up);
        qm.addBindValue(cost);

        if (!qm.exec())
            qDebug() << "Error saving material:" << qm.lastError().text();
    }

    // --- Añadir a la lista de presupuestos ---
    QString clientName = leClientName->text();
    QString fecha = QDateTime::currentDateTime().toString("ddd MMM dd HH:mm:ss yyyy");
    QString estado = "ABIERTA";

    QString label = QString("%1 - %2  |  %3  (%4)")
                        .arg(budgetId)
                        .arg(clientName.isEmpty() ? "Sin cliente" : clientName)
                        .arg(fecha)
                        .arg(estado);

    auto *it = new QListWidgetItem(label);
    it->setData(Qt::UserRole, budgetId);
    lwBudgets->insertItem(0, it);

    QMessageBox::information(this, "Guardado", "Presupuesto guardado con id " + QString::number(budgetId));
} */

/* void MainWindow::onLoadSelectedBudget()
{
    // 1️⃣ Verificar que haya un ítem seleccionado
    QListWidgetItem *currentItem = lwBudgets->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Abrir presupuesto", "No se ha seleccionado ningún presupuesto.");
        return;
    }

    int budgetId = currentItem->data(Qt::UserRole).toInt();
    if (budgetId <= 0) {
        qWarning() << "ID de presupuesto inválido";
        return;
    }
    qDebug() << "Cargando presupuesto ID:" << budgetId;

    // 2️⃣ Cargar datos del presupuesto - CONSULTA CORREGIDA
    QSqlQuery query(Database::instance());
    query.prepare("SELECT client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, "
                  "dias_elevador, precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, "
                  "operarios, dias_trabajo, horas, horas_viaje, base_price, total_no_iva, total_con_iva, status "
                  "FROM budgets WHERE id = ?");
    query.addBindValue(budgetId);

    if (!query.exec()) {
        qWarning() << "Error ejecutando consulta:" << query.lastError().text();
        return;
    }
    
    if (!query.next()) {
        qWarning() << "No se encontró el presupuesto con ID" << budgetId;
        QMessageBox::warning(this, "Error", "No se encontró el presupuesto seleccionado.");
        return;
    }

    int clientId = query.value(0).toInt();
    
    // Cargar todos los campos en el orden correcto
    cbTipoLocal->setCurrentText(query.value(1).toString());
    sbMetros->setValue(query.value(2).toDouble());
    cbTipoCubierta->setCurrentText(query.value(3).toString());
    cbExtractor->setCurrentText(query.value(4).toString());
    cbElevador->setCurrentText(query.value(5).toString());
    sbElevPortes->setValue(query.value(6).toDouble());
    spElevDia->setValue(query.value(7).toInt());
    spElevPrecDia->setValue(query.value(8).toInt());
    cbZona->setCurrentText(query.value(9).toString());
    spDietas->setValue(query.value(10).toInt());
    spDiasDieta->setValue(query.value(11).toInt());
    sbPrecioDiet->setValue(query.value(12).toInt());
    spFurgonetas->setValue(query.value(13).toInt());
    sbCosteFurgo->setValue(query.value(14).toInt());
    
    // Localidad
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidadObra");
    if (leLocalidad) 
        leLocalidad->setText(query.value(15).toString());
    
    sbKM->setValue(query.value(16).toDouble());
    sbLitros->setValue(query.value(17).toDouble());
    spOperarios->setValue(query.value(18).toInt());
    spDias->setValue(query.value(19).toInt());
    sbHoras->setValue(query.value(20).toDouble());
    sbHorasViaje->setValue(query.value(21).toDouble());

    qDebug() << "Presupuesto cargado - Client ID:" << clientId 
             << "Operarios:" << query.value(22).toInt()
             << "Días:" << query.value(23).toInt();
    QString status = query.value(24).toString().toLower();

    // 3️⃣ Cargar datos del cliente
    QSqlQuery clientQuery(Database::instance());
    clientQuery.prepare("SELECT budget, name, company, cif, contact, address, phone, email, fecha FROM clients WHERE id = ?");
    clientQuery.addBindValue(clientId);
    if (clientQuery.exec() && clientQuery.next()) {
        leNumPresu->setText(clientQuery.value(0).toString());
        leClientName->setText(clientQuery.value(1).toString());
        leCompany->setText(clientQuery.value(2).toString());
        leCIF->setText(clientQuery.value(3).toString());
        leContact->setText(clientQuery.value(4).toString());
        leAddress->setText(clientQuery.value(5).toString());
        lePhone->setText(clientQuery.value(6).toString());
        leEmail->setText(clientQuery.value(7).toString());
        leFecha->setText(clientQuery.value(8).toString());
    } else {
        qWarning() << "No se encontró el cliente con ID" << clientId 
                   << "Error:" << clientQuery.lastError().text();
    }

    // 4️⃣ Cargar materiales
    twMaterials->setRowCount(0);
    QSqlQuery materialsQuery(Database::instance());
    materialsQuery.prepare("SELECT name, quantity, unit_price, cost_price FROM materials WHERE budget_id = ?");
    materialsQuery.addBindValue(budgetId);
    if (materialsQuery.exec()) {
        int rowCount = 0;
        while (materialsQuery.next()) {
            int r = twMaterials->rowCount();
            twMaterials->insertRow(r);

            QString name = materialsQuery.value(0).toString();
            double qty  = materialsQuery.value(1).toDouble();
            double sellPrice = materialsQuery.value(2).toDouble();  // Precio venta
            double costPrice = materialsQuery.value(3).toDouble();  // Precio compra
            double total = qty * sellPrice;

            twMaterials->setItem(r, 0, new QTableWidgetItem(name));
            twMaterials->setItem(r, 1, new QTableWidgetItem(QString::number(qty, 'f', 2)));
            twMaterials->setItem(r, 2, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
            twMaterials->setItem(r, 3, new QTableWidgetItem(QString::number(costPrice, 'f', 2)));  // Precio compra
            twMaterials->setItem(r, 4, new QTableWidgetItem(QString::number(total, 'f', 2)));      // Total venta
            rowCount++;
        }
        qDebug() << "Materiales cargados:" << rowCount << "filas";
    }
    else {
        qWarning() << "No se pudieron cargar materiales para presupuesto ID" << budgetId
                   << "Error:" << materialsQuery.lastError().text();
    }

    // 5️⃣ Recalcular totales
    onCalculate();

    // 6️⃣ APLICAR LÓGICA DE ESTADO (¡El paso final!)
    currentBudgetID = QString::number(budgetId); // Guardar el ID
    currentBudgetStatus = status;               // Guardar el estado

    bool isEnabled = (currentBudgetStatus == "abierta");
    toggleInputFields(isEnabled); // Bloquear/Desbloquear campos

    // Actualizar el botón de Cerrar/Reabrir Obra
    QPushButton *btnCloseProject = findChild<QPushButton*>("btnCloseProject");
    if (btnCloseProject) {
        btnCloseProject->setText(isEnabled ? "  Cerrar obra  " : "  Reabrir obra  ");
    }
    
    QMessageBox::information(this, "Cargado", 
        QString("Presupuesto %1 cargado correctamente.\n%2 materiales cargados.")
            .arg(budgetId)
            .arg(twMaterials->rowCount()));
} */

void MainWindow::onLoadSelectedBudget()
{
    // 1️⃣ Verificar que haya un ítem seleccionado
    QListWidgetItem *currentItem = lwBudgets->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "Abrir presupuesto", "No se ha seleccionado ningún presupuesto.");
        return;
    }

    int budgetId = currentItem->data(Qt::UserRole).toInt();
    if (budgetId <= 0) {
        qWarning() << "ID de presupuesto inválido";
        return;
    }
    qDebug() << "Cargando presupuesto ID:" << budgetId;

    // 2️⃣ Cargar datos del presupuesto: Se asegura de cargar el campo 'status' (columna 24)
    QSqlQuery query(Database::instance());
    query.prepare("SELECT client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, "
                  "dias_elevador, precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, "
                  "operarios, dias_trabajo, horas, horas_viaje, base_price, total_no_iva, total_con_iva, status " // ⬅️ CAMPO STATUS
                  "FROM budgets WHERE id = ?");
    query.addBindValue(budgetId);

    if (!query.exec()) {
        qWarning() << "Error ejecutando consulta:" << query.lastError().text();
        return;
    }
    
    if (!query.next()) {
        qWarning() << "No se encontró el presupuesto con ID" << budgetId;
        QMessageBox::warning(this, "Error", "No se encontró el presupuesto seleccionado.");
        return;
    }

    int clientId = query.value(0).toInt();
    
    // Cargar todos los campos en el orden correcto
    cbTipoLocal->setCurrentText(query.value(1).toString());
    sbMetros->setValue(query.value(2).toDouble());
    cbTipoCubierta->setCurrentText(query.value(3).toString());
    cbExtractor->setCurrentText(query.value(4).toString());
    cbElevador->setCurrentText(query.value(5).toString());
    sbElevPortes->setValue(query.value(6).toDouble());
    spElevDia->setValue(query.value(7).toInt());
    spElevPrecDia->setValue(query.value(8).toInt());
    cbZona->setCurrentText(query.value(9).toString());
    spDietas->setValue(query.value(10).toInt());
    spDiasDieta->setValue(query.value(11).toInt());
    sbPrecioDiet->setValue(query.value(12).toInt());
    spFurgonetas->setValue(query.value(13).toInt());
    sbCosteFurgo->setValue(query.value(14).toInt());
    
    // Localidad
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidadObra");
    if (leLocalidad) 
        leLocalidad->setText(query.value(15).toString());
    
    sbKM->setValue(query.value(16).toDouble());
    sbLitros->setValue(query.value(17).toDouble());
    spOperarios->setValue(query.value(18).toInt());
    spDias->setValue(query.value(19).toInt());
    sbHoras->setValue(query.value(20).toDouble());
    sbHorasViaje->setValue(query.value(21).toDouble());

    // ⬇️ Cargar el estado (índice 24) y registrarlo
    QString status = query.value(24).toString().toLower();

    // 3️⃣ Cargar datos del cliente (Sin cambios)
    QSqlQuery clientQuery(Database::instance());
    clientQuery.prepare("SELECT budget, name, company, cif, contact, address, phone, email, fecha FROM clients WHERE id = ?");
    clientQuery.addBindValue(clientId);
    if (clientQuery.exec() && clientQuery.next()) {
        leNumPresu->setText(clientQuery.value(0).toString());
        leClientName->setText(clientQuery.value(1).toString());
        leCompany->setText(clientQuery.value(2).toString());
        leCIF->setText(clientQuery.value(3).toString());
        leContact->setText(clientQuery.value(4).toString());
        leAddress->setText(clientQuery.value(5).toString());
        lePhone->setText(clientQuery.value(6).toString());
        leEmail->setText(clientQuery.value(7).toString());
        leFecha->setText(clientQuery.value(8).toString());
    } else {
        qWarning() << "No se encontró el cliente con ID" << clientId 
                   << "Error:" << clientQuery.lastError().text();
    }

    // 4️⃣ Cargar materiales (Sin cambios)
    twMaterials->setRowCount(0);
    QSqlQuery materialsQuery(Database::instance());
    materialsQuery.prepare("SELECT name, quantity, unit_price, cost_price FROM materials WHERE budget_id = ?");
    materialsQuery.addBindValue(budgetId);
    if (materialsQuery.exec()) {
        int rowCount = 0;
        while (materialsQuery.next()) {
            int r = twMaterials->rowCount();
            twMaterials->insertRow(r);

            QString name = materialsQuery.value(0).toString();
            double qty  = materialsQuery.value(1).toDouble();
            double sellPrice = materialsQuery.value(2).toDouble();
            double costPrice = materialsQuery.value(3).toDouble();
            double total = qty * sellPrice;

            twMaterials->setItem(r, 0, new QTableWidgetItem(name));
            twMaterials->setItem(r, 1, new QTableWidgetItem(QString::number(qty, 'f', 2)));
            twMaterials->setItem(r, 2, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
            twMaterials->setItem(r, 3, new QTableWidgetItem(QString::number(costPrice, 'f', 2)));
            twMaterials->setItem(r, 4, new QTableWidgetItem(QString::number(total, 'f', 2)));
            rowCount++;
        }
        qDebug() << "Materiales cargados:" << rowCount << "filas";
    }
    else {
        qWarning() << "No se pudieron cargar materiales para presupuesto ID" << budgetId
                   << "Error:" << materialsQuery.lastError().text();
    }

    // 5️⃣ Recalcular totales
    onCalculate();

    // 6️⃣ APLICAR LÓGICA DE ESTADO
    
    // ⬇️ Actualizar variables de estado
    currentBudgetId = budgetId;
    currentBudgetStatus = status;               
    currentStatus = currentBudgetStatus; 

    bool isEnabled = (currentBudgetStatus == "abierta");
    toggleInputFields(isEnabled); // Bloquear/Desbloquear campos
    
    // Actualizar el botón de Cerrar/Reabrir Obra
    QPushButton *btnCloseProject = findChild<QPushButton*>("btnCloseProject");
    if (btnCloseProject) {
        btnCloseProject->setText(isEnabled ? "  Cerrar obra  " : "  Reabrir obra  ");
    }
    
    QMessageBox::information(this, "Cargado", 
        QString("Presupuesto %1 cargado correctamente.\nEstado: %2.")
            .arg(budgetId)
            .arg(currentBudgetStatus.toUpper()));
}