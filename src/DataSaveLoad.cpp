#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

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
    qb.addBindValue(spPrecioDiet->value());         // Precio dieta X día
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
    query.prepare("SELECT client_id, tipo_local, metros, tipo_cubierta, extraccion, elevador, portes, "
                  "dias_elevador, precio_dia, zona, dietas, dias_dieta, precio_dieta, furgonetas, coste_furgonetas, km, combustible, "
                  "operarios, dias_trabajo, horas, horas_viaje, base_price, total_no_iva, total_con_iva "
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
    spPrecioDiet->setValue(query.value(12).toInt());
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