#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"

void MainWindow::onSaveBudget()
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
            client_id, tipo_local, metros, tipo_cubierta, elevador, portes, dias_elevador, 
            zona, dietas, dias_dieta, localidad, km, combustible, operarios, dias_trabajo, 
            horas, base_price, total_no_iva, total_con_iva
        ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
    )");

    qb.addBindValue(clientId);                        // client_id
    qb.addBindValue(cbTipoLocal->currentText());     // tipo_local
    qb.addBindValue(sbMetros->value());              // metros
    qb.addBindValue(cbTipoCubierta->currentText());  // tipo_cubierta
    qb.addBindValue(cbElevador->currentText());      // elevador
    qb.addBindValue(sbElevPortes->value());          // portes
    qb.addBindValue(spElevDia->value());             // dias_elevador
    qb.addBindValue(cbZona->currentText());          // zona
    qb.addBindValue(spDietas->value());             // dietas
    qb.addBindValue(spDiasDieta->value());          // dias_dieta
    qb.addBindValue(leLocalidadObra ? leLocalidadObra->text() : QString()); // localidad
    qb.addBindValue(sbKM->value());                  // km
    qb.addBindValue(sbLitros->value());             // combustible
    qb.addBindValue(spOperarios->value());          // operarios
    qb.addBindValue(spDias->value());               // dias_trabajo
    qb.addBindValue(sbHoras->value());              // horas
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
}

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

    // 2️⃣ Cargar datos del presupuesto - CONSULTA CORREGIDA
    QSqlQuery query(Database::instance());
    query.prepare("SELECT client_id, tipo_local, metros, tipo_cubierta, elevador, portes, "
                  "dias_elevador, zona, dietas, dias_dieta, localidad, km, combustible, "
                  "operarios, dias_trabajo, horas, base_price, total_no_iva, total_con_iva "
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
    cbElevador->setCurrentText(query.value(4).toString());
    sbElevPortes->setValue(query.value(5).toDouble());
    spElevDia->setValue(query.value(6).toInt());
    cbZona->setCurrentText(query.value(7).toString());
    spDietas->setValue(query.value(8).toInt());
    spDiasDieta->setValue(query.value(9).toInt());
    
    // Localidad
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidadObra");
    if (leLocalidad) 
        leLocalidad->setText(query.value(10).toString());
    
    sbKM->setValue(query.value(11).toDouble());
    sbLitros->setValue(query.value(12).toDouble());
    spOperarios->setValue(query.value(13).toInt());
    spDias->setValue(query.value(14).toInt());
    sbHoras->setValue(query.value(15).toDouble());

    qDebug() << "Presupuesto cargado - Client ID:" << clientId 
             << "Operarios:" << query.value(13).toInt()
             << "Días:" << query.value(14).toInt();

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
    
    QMessageBox::information(this, "Cargado", 
        QString("Presupuesto %1 cargado correctamente.\n%2 materiales cargados.")
            .arg(budgetId)
            .arg(twMaterials->rowCount()));
}