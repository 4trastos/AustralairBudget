#include "MainWindow.hpp"
#include "Database.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    if (!Database::init()) {
        QMessageBox::critical(nullptr, "DB", "No se pudo abrir la base de datos");
        exit(1);
    }
    setupUi();
    loadSettings();
}

void MainWindow::setupUi() {
    QWidget *w = new QWidget;
    auto *mainLayout = new QHBoxLayout;

    // Left: form
    auto *formLayout = new QFormLayout;

    leClientName = new QLineEdit; leCompany = new QLineEdit; leContact = new QLineEdit;
    leAddress = new QLineEdit; lePhone = new QLineEdit; leEmail = new QLineEdit;

    formLayout->addRow("Cliente (nombre):", leClientName);
    formLayout->addRow("Empresa:", leCompany);
    formLayout->addRow("Contacto:", leContact);
    formLayout->addRow("Dirección:", leAddress);
    formLayout->addRow("Teléfono:", lePhone);
    formLayout->addRow("Email:", leEmail);

    sbMetros = new QDoubleSpinBox; sbMetros->setRange(0, 1e6); sbMetros->setSuffix(" m²");
    cbTipoLocal = new QComboBox; cbTipoLocal->addItems({"Nave industrial","Local comercial","Vivienda","Pista de pádel indoor"});
    cbTipoCubierta = new QComboBox; cbTipoCubierta->addItems({"Chapa","Teja","Hormigón","Otros"});
    sbKM = new QDoubleSpinBox; sbKM->setRange(0,10000); sbKM->setSuffix(" km");
    spDietas = new QSpinBox; spDietas->setRange(0,1000);
    sbHoras = new QDoubleSpinBox; sbHoras->setRange(0,1e5);
    spDias = new QSpinBox; spDias->setRange(0,365);

    formLayout->addRow("Metros cuadrados:", sbMetros);
    formLayout->addRow("Tipo de local:", cbTipoLocal);
    formLayout->addRow("Localidad:", new QLineEdit); // inline local field
    formLayout->itemAt(formLayout->rowCount()-1)->widget()->setObjectName("leLocalidad");
    formLayout->addRow("Tipo de cubierta:", cbTipoCubierta);
    formLayout->addRow("KM desplazamiento:", sbKM);
    formLayout->addRow("Dietas (empleados):", spDietas);
    formLayout->addRow("Horas estimadas:", sbHoras);
    formLayout->addRow("Días de trabajo:", spDias);

    // Materials table
    twMaterials = new QTableWidget(0,4);
    twMaterials->setHorizontalHeaderLabels({"Nombre","Cantidad","Precio unit.","Total"});
    twMaterials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    btnAddMat = new QPushButton("Añadir material");
    btnRemoveMat = new QPushButton("Eliminar material");
    connect(btnAddMat, &QPushButton::clicked, this, &MainWindow::onNewMaterial);
    connect(btnRemoveMat, &QPushButton::clicked, this, &MainWindow::onRemoveMaterial);

    QWidget *matWidget = new QWidget;
    auto *matLayout = new QVBoxLayout;
    matLayout->addWidget(twMaterials);
    auto *matBtns = new QHBoxLayout;
    matBtns->addWidget(btnAddMat); matBtns->addWidget(btnRemoveMat);
    matLayout->addLayout(matBtns);
    matWidget->setLayout(matLayout);
    formLayout->addRow("Materiales:", matWidget);

    // Buttons and totals
    btnCalc = new QPushButton("Calcular");
    btnSave = new QPushButton("Guardar presupuesto");
    QLabel *lbl1 = new QLabel("Total sin IVA:"); lblTotalNoIVA = new QLabel("0.00 €");
    QLabel *lbl2 = new QLabel("Total con IVA (21%):"); lblTotalConIVA = new QLabel("0.00 €");
    QPushButton *btnEditPrices = new QPushButton("Editar precios base");

    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveBudget);
    connect(btnEditPrices, &QPushButton::clicked, this, &MainWindow::onEditPrices);

    //auto *bottomLayout = new QHBoxLayout;
    auto *leftV = new QVBoxLayout;
    leftV->addLayout(formLayout);
    leftV->addWidget(btnCalc);
    leftV->addWidget(btnSave);
    leftV->addWidget(btnEditPrices);
    leftV->addStretch();
    leftV->addWidget(lbl1); leftV->addWidget(lblTotalNoIVA);
    leftV->addWidget(lbl2); leftV->addWidget(lblTotalConIVA);

    // Right: saved budgets list
    auto *rightV = new QVBoxLayout;
    lwBudgets = new QListWidget;
    QPushButton *btnLoad = new QPushButton("Abrir seleccionado");
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadSelectedBudget);
    rightV->addWidget(new QLabel("Presupuestos guardados"));
    rightV->addWidget(lwBudgets);
    rightV->addWidget(btnLoad);

    mainLayout->addLayout(leftV, 3);
    mainLayout->addLayout(rightV, 1);

    w->setLayout(mainLayout);
    setCentralWidget(w);
    setWindowTitle("Australair - Gestor de presupuestos");

    // Load saved budgets into list
    QSqlQuery q(Database::instance());
    q.exec("SELECT id, created_at FROM budgets ORDER BY created_at DESC");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString ts = q.value(1).toString();
        auto *it = new QListWidgetItem(QString::number(id) + " - " + ts);
        it->setData(Qt::UserRole, id);
        lwBudgets->addItem(it);
    }
}

double MainWindow::getSettingDouble(const QString &key, double def) {
    QSqlQuery q(Database::instance());
    q.prepare("SELECT value FROM settings WHERE key = ?");
    q.addBindValue(key);
    if (q.exec() && q.next()) {
        bool ok=false;
        double v = q.value(0).toString().toDouble(&ok);
        if (ok) return v;
    }
    return def;
}

void MainWindow::loadSettings() {
    // nothing to do now, will read when calculating
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

void MainWindow::onCalculate() {
    double base = getSettingDouble("price_base", 10.0);
    double incr = getSettingDouble("increment_per_field", 5.0);
    double iva = getSettingDouble("iva_pct", 21.0);

    // Count filled "apartados": vamos a contar campos no vacíos entre los apartados clave
    int count = 0;
    if (!leClientName->text().isEmpty()) count++;
    if (sbMetros->value() > 0) count++;
    if (!cbTipoLocal->currentText().isEmpty()) count++;
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidad");
    if (leLocalidad && !leLocalidad->text().isEmpty()) count++;
    if (!cbTipoCubierta->currentText().isEmpty()) count++;
    if (sbKM->value() > 0) count++;
    if (spDietas->value() > 0) count++;
    if (twMaterials->rowCount() > 0) count++;
    if (sbHoras->value() > 0) count++;
    if (spDias->value() > 0) count++;

    double total = base + (count * incr);

    // add materials
    for (int r=0; r<twMaterials->rowCount(); ++r) {
        double qty = twMaterials->item(r,1)->text().toDouble();
        double up = twMaterials->item(r,2)->text().toDouble();
        double line = qty * up;
        twMaterials->item(r,3)->setText(QString::number(line, 'f', 2));
        total += line;
    }

    double totalConIva = total * (1.0 + iva/100.0);
    lblTotalNoIVA->setText(QString::number(total, 'f', 2) + " €");
    lblTotalConIVA->setText(QString::number(totalConIva, 'f', 2) + " €");
}

void MainWindow::onSaveBudget() {
    // grab client -> insert or reuse
    QSqlDatabase d = Database::instance();
    QSqlQuery q(d);
    QString client = leClientName->text();

    q.prepare("INSERT INTO clients(name,company,contact,address,phone,email) VALUES(?,?,?,?,?,?)");
    q.addBindValue(leClientName->text());
    q.addBindValue(leCompany->text());
    q.addBindValue(leContact->text());
    q.addBindValue(leAddress->text());
    q.addBindValue(lePhone->text());
    q.addBindValue(leEmail->text());
    if (!q.exec()) {
        QMessageBox::warning(this, "DB", "Error al guardar cliente: " + q.lastError().text());
        return;
    }
    int clientId = q.lastInsertId().toInt();

    // budget
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidad");
    double base = getSettingDouble("price_base", 10.0);
    double totalNoIva = lblTotalNoIVA->text().replace(" €","").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €","").toDouble();

    QSqlQuery qb(d);
    qb.prepare(R"(
        INSERT INTO budgets(client_id, metros, tipo_local, localidad, tipo_cubierta, km, dietas, horas, dias, base_price, total_no_iva, total_con_iva)
        VALUES(?,?,?,?,?,?,?,?,?,?,?,?)
    )");
    qb.addBindValue(clientId);
    qb.addBindValue(sbMetros->value());
    qb.addBindValue(cbTipoLocal->currentText());
    qb.addBindValue(leLocalidad ? leLocalidad->text() : QString());
    qb.addBindValue(cbTipoCubierta->currentText());
    qb.addBindValue(sbKM->value());
    qb.addBindValue(spDietas->value());
    qb.addBindValue(sbHoras->value());
    qb.addBindValue(spDias->value());
    qb.addBindValue(base);
    qb.addBindValue(totalNoIva);
    qb.addBindValue(totalConIva);
    if (!qb.exec()) {
        QMessageBox::warning(this, "DB", "Error al guardar presupuesto: " + qb.lastError().text());
        return;
    }
    int budgetId = qb.lastInsertId().toInt();

    // materials
    QSqlQuery qm(d);
    qm.prepare("INSERT INTO materials(budget_id,name,quantity,unit_price) VALUES(?,?,?,?)");
    for (int r=0; r<twMaterials->rowCount(); ++r) {
        QString name = twMaterials->item(r,0)->text();
        double qty = twMaterials->item(r,1)->text().toDouble();
        double up = twMaterials->item(r,2)->text().toDouble();
        qm.addBindValue(budgetId);
        qm.addBindValue(name);
        qm.addBindValue(qty);
        qm.addBindValue(up);
        if (!qm.exec()) {
            qDebug() << "Error saving material:" << qm.lastError().text();
        }
    }

    // add to list widget
    auto *it = new QListWidgetItem(QString::number(budgetId) + " - " + QDateTime::currentDateTime().toString());
    it->setData(Qt::UserRole, budgetId);
    lwBudgets->insertItem(0, it);

    QMessageBox::information(this, "Guardado", "Presupuesto guardado con id " + QString::number(budgetId));
}

void MainWindow::onLoadSelectedBudget() {
    auto *it = lwBudgets->currentItem();
    if (!it) return;
    int id = it->data(Qt::UserRole).toInt();
    QSqlQuery qb(Database::instance());
    qb.prepare("SELECT client_id, metros, tipo_local, localidad, tipo_cubierta, km, dietas, horas, dias FROM budgets WHERE id = ?");
    qb.addBindValue(id);
    if (!qb.exec() || !qb.next()) return;
    int clientId = qb.value(0).toInt();
    sbMetros->setValue(qb.value(1).toDouble());
    cbTipoLocal->setCurrentText(qb.value(2).toString());
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidad");
    if (leLocalidad) leLocalidad->setText(qb.value(3).toString());
    cbTipoCubierta->setCurrentText(qb.value(4).toString());
    sbKM->setValue(qb.value(5).toDouble());
    spDietas->setValue(qb.value(6).toInt());
    sbHoras->setValue(qb.value(7).toDouble());
    spDias->setValue(qb.value(8).toInt());

    // load client
    QSqlQuery qc(Database::instance());
    qc.prepare("SELECT name,company,contact,address,phone,email FROM clients WHERE id = ?");
    qc.addBindValue(clientId);
    if (qc.exec() && qc.next()) {
        leClientName->setText(qc.value(0).toString());
        leCompany->setText(qc.value(1).toString());
        leContact->setText(qc.value(2).toString());
        leAddress->setText(qc.value(3).toString());
        lePhone->setText(qc.value(4).toString());
        leEmail->setText(qc.value(5).toString());
    }

    // load materials
    twMaterials->setRowCount(0);
    QSqlQuery qm(Database::instance());
    qm.prepare("SELECT name,quantity,unit_price FROM materials WHERE budget_id = ?");
    qm.addBindValue(id);
    if (qm.exec()) {
        while (qm.next()) {
            int r = twMaterials->rowCount();
            twMaterials->insertRow(r);
            twMaterials->setItem(r,0, new QTableWidgetItem(qm.value(0).toString()));
            twMaterials->setItem(r,1, new QTableWidgetItem(qm.value(1).toString()));
            twMaterials->setItem(r,2, new QTableWidgetItem(qm.value(2).toString()));
            double line = qm.value(1).toDouble() * qm.value(2).toDouble();
            twMaterials->setItem(r,3, new QTableWidgetItem(QString::number(line,'f',2)));
        }
    }
    onCalculate();
}

void MainWindow::onEditPrices() {
    bool ok;
    double newBase = QInputDialog::getDouble(this, "Precio base", "Precio base (€):", getSettingDouble("price_base",10.0), 0, 1e9, 2, &ok);
    if (!ok) return;
    double newInc = QInputDialog::getDouble(this, "Incremento por apartado", "Incremento (€):", getSettingDouble("increment_per_field",5.0), 0, 1e9, 2, &ok);
    if (!ok) return;
    double newIva = QInputDialog::getDouble(this, "IVA (%)", "IVA porcentaje:", getSettingDouble("iva_pct",21.0), 0, 100, 2, &ok);
    if (!ok) return;

    QSqlQuery q(Database::instance());
    q.prepare("REPLACE INTO settings(key,value) VALUES(?,?)");
    q.addBindValue("price_base"); q.addBindValue(QString::number(newBase)); q.exec();
    q.addBindValue("increment_per_field"); q.addBindValue(QString::number(newInc)); q.exec();
    q.addBindValue("iva_pct"); q.addBindValue(QString::number(newIva)); q.exec();

    QMessageBox::information(this, "Ajustes", "Precios actualizados.");
}
