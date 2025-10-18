#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    if (!Database::init()) {
        QMessageBox::critical(nullptr, "DB", "No se pudo abrir la base de datos");
        exit(1);
    }
    setupUi();
    loadSettings();
    showMaximized();
}

void MainWindow::setupUi() {
    QWidget *w = new QWidget;
    auto *mainLayout = new QHBoxLayout;

    // ---------------------- Formulario ----------------------
    auto *formLayout = new QFormLayout;

    // --- Campos cliente ---
    leClientName = new QLineEdit; leCompany = new QLineEdit; leContact = new QLineEdit;
    leAddress = new QLineEdit; lePhone = new QLineEdit; leEmail = new QLineEdit;

    auto *customerLayout = new QGridLayout;
    customerLayout->addWidget(new QLabel("Cliente (nombre):"), 0, 0);
    customerLayout->addWidget(leClientName, 0, 1);
    customerLayout->addWidget(new QLabel("Empresa:"), 0, 2);
    customerLayout->addWidget(leCompany, 0, 3);

    customerLayout->addWidget(new QLabel("Contacto:"), 1, 0);
    customerLayout->addWidget(leContact, 1, 1);
    customerLayout->addWidget(new QLabel("Teléfono:"), 1, 2);
    customerLayout->addWidget(lePhone, 1, 3);

    customerLayout->addWidget(new QLabel("Dirección:"), 2, 0);
    customerLayout->addWidget(leAddress, 2, 1);
    customerLayout->addWidget(new QLabel("Email:"), 2, 2);
    customerLayout->addWidget(leEmail, 2, 3);

    formLayout->addRow(customerLayout);

    // ---------------------- Línea azul separadora ----------------------
    QFrame *blueLine = new QFrame;
    blueLine->setFrameShape(QFrame::HLine);
    blueLine->setFrameShadow(QFrame::Sunken);
    blueLine->setStyleSheet("color: #3498DB; background-color: #3498DB; max-height: 3px;");
    formLayout->addRow(blueLine);

    // ---------------------- Campos proyecto ----------------------
    sbMetros = new QDoubleSpinBox; sbMetros->setRange(0, 1e6); sbMetros->setSuffix(" m²");
    cbTipoLocal = new QComboBox; cbTipoLocal->addItems({" ··· ","Nave industrial","Local comercial","Vivienda","Complejo Deportivo"});
    cbTipoCubierta = new QComboBox; cbTipoCubierta->addItems({" ··· ","Chapa","Teja","Cubierta plana","Panel sandwich", "Uralita"});
    sbKM = new QDoubleSpinBox; sbKM->setRange(0,10000); sbKM->setSuffix(" km");
    sbLitros = new QDoubleSpinBox; sbLitros->setRange(0,10000); sbLitros->setSuffix(" lts");
    sbHoras = new QDoubleSpinBox; sbHoras->setRange(0,1e5);
    spDietas = new QSpinBox; spDietas->setRange(0,1000);
    spDiasDieta = new QSpinBox; spDiasDieta->setRange(0,365); // días asociados a dietas
    spDias = new QSpinBox; spDias->setRange(0,365); // días de trabajo generales
    cbZona = new QComboBox; cbZona->addItems({" ··· ","Zona Centro","Otras Zonas"});
    leLocalidadObra = new QLineEdit;

    // --- Campos proyecto básicos ---
    formLayout->addRow("Tipo de local:", cbTipoLocal);
    formLayout->addRow("Metros cuadrados:", sbMetros);
    formLayout->addRow("Tipo de cubierta:", cbTipoCubierta);
    formLayout->addRow("Zona:", cbZona);

    // --- Bloque Dietas (Sí/No) ---
    QHBoxLayout *dietasLayout = new QHBoxLayout;
    rbDietasSi = new QRadioButton("Sí");   // declarados como miembros de la clase
    rbDietasNo = new QRadioButton("No");
    rbDietasNo->setChecked(true);

    dietasLayout->addWidget(rbDietasSi);
    dietasLayout->addWidget(rbDietasNo);

    QWidget *dietasWidget = new QWidget;
    dietasWidget->setLayout(dietasLayout);

    formLayout->addRow("Dietas:", dietasWidget);
    formLayout->addRow("Empleados (dieta):", spDietas);
    formLayout->addRow("Días (dieta):", spDiasDieta);

    // --- Desactivar inicialmente ---
    spDietas->setEnabled(false);
    spDiasDieta->setEnabled(false);

    // --- Conexión para controlar dietas ---
    connect(rbDietasSi, &QRadioButton::toggled, this, [this](bool checked){
        spDietas->setEnabled(checked);
        spDiasDieta->setEnabled(checked);
    });

    // --- Conexión para controlar según zona ---
    connect(cbZona, &QComboBox::currentTextChanged, this, [this](const QString &zona){
        if (zona == "Zona Centro") {
            // Zona Centro: dietas siempre desactivadas
            rbDietasSi->setChecked(false);
            rbDietasSi->setEnabled(false);
            rbDietasNo->setChecked(true);
            rbDietasNo->setEnabled(false);
            spDietas->setEnabled(false);
            spDiasDieta->setEnabled(false);
        } else {
            // Otras Zonas: dietas habilitadas
            rbDietasSi->setEnabled(true);
            rbDietasNo->setEnabled(true);
            spDietas->setEnabled(rbDietasSi->isChecked());
            spDiasDieta->setEnabled(rbDietasSi->isChecked());
        }
    });


    // --- Conexión para Kilometros / Combustible ---
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::updateDistanceAndFuel);


    /* // --- Campos proyecto básicos ---
    formLayout->addRow("Tipo de local:", cbTipoLocal);
    formLayout->addRow("Metros cuadrados:", sbMetros);
    formLayout->addRow("Tipo de cubierta:", cbTipoCubierta);
    formLayout->addRow("Zona:", cbZona);

    // --- Bloque Dietas (Sí/No) ---
    QHBoxLayout *dietasLayout = new QHBoxLayout;
    QRadioButton *rbDietasSi = new QRadioButton("Sí");
    QRadioButton *rbDietasNo = new QRadioButton("No");
    rbDietasNo->setChecked(true);
    dietasLayout->addWidget(rbDietasSi);
    dietasLayout->addWidget(rbDietasNo);
    QWidget *dietasWidget = new QWidget;
    dietasWidget->setLayout(dietasLayout);

    formLayout->addRow("Dietas:", dietasWidget);
    formLayout->addRow("Empleados (dieta):", spDietas);
    formLayout->addRow("Días (dieta):", spDiasDieta);

    // --- Desactivar inicialmente ---
    spDietas->setEnabled(false);
    spDiasDieta->setEnabled(false);

    // --- Conexión para activar/desactivar dietas y Kilometros / Combustible ---
    connect(rbDietasSi, &QRadioButton::toggled, this, [this](bool checked){
        spDietas->setEnabled(checked);
        spDiasDieta->setEnabled(checked);
    }); */

    // --- Conexión para Kilometros / Combustible ---
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::updateDistanceAndFuel);

    // --- Campos restantes ---
    formLayout->addRow("Localidad:", leLocalidadObra);
    formLayout->addRow("KM desplazamiento:", sbKM);
    formLayout->addRow("Combustible:", sbLitros);
    formLayout->addRow("Horas estimadas:", sbHoras);
    formLayout->addRow("Días de trabajo:", spDias); // días generales


    // --- Tabla materiales ---
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

    // ---------------------- Botones ----------------------
    btnCalc = new QPushButton("Calcular");
    btnSave = new QPushButton("Guardar presupuesto");
    btnPDF = new QPushButton("Guardar en PDF");
    btnPrint = new QPushButton("Imprimir presupuesto");
    btnStart = new QPushButton("Mantenimientos");
    QPushButton *btnEditPrices = new QPushButton("Editar precios base");

    QLabel *lbl1 = new QLabel("Total sin IVA:");
    lblTotalNoIVA = new QLabel("0.00 €");
    QLabel *lbl2 = new QLabel("Total con IVA (21%):");
    lblTotalConIVA = new QLabel("0.00 €");

    // ---------------------- Conexiones ----------------------
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveBudget);
    connect(btnEditPrices, &QPushButton::clicked, this, &MainWindow::onEditPrices);
    connect(btnPDF, &QPushButton::clicked, this, &MainWindow::onExportPDF);
    connect(btnPrint, &QPushButton::clicked, this, &MainWindow::onPrintBudget);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onBackToStart);

    // ---------------------- Bloque de botones ----------------------
    auto *colLeft = new QVBoxLayout;
    colLeft->addWidget(btnCalc);
    colLeft->addWidget(btnPDF);
    colLeft->addWidget(btnEditPrices);

    auto *colRight = new QVBoxLayout;
    colRight->addWidget(btnSave);
    colRight->addWidget(btnPrint);
    colRight->addWidget(btnStart);

    // --- Reducir espacio entre botones ---
    colLeft->setSpacing(6);
    colRight->setSpacing(6);

    // --- Contenedor para las dos columnas ---
    auto *buttonsGrid = new QHBoxLayout;
    buttonsGrid->addLayout(colLeft);
    buttonsGrid->addSpacing(20);
    buttonsGrid->addLayout(colRight);
    buttonsGrid->setContentsMargins(0, 0, 0, 0);

    // --- Agrupar botones en bloque con margen visual ---
    auto *buttonsGroup = new QGroupBox("Acciones");
    auto *buttonsGroupLayout = new QVBoxLayout(buttonsGroup);
    buttonsGroupLayout->addLayout(buttonsGrid);
    buttonsGroupLayout->setContentsMargins(10, 10, 10, 10);

    // ---------------------- Bloque de resultados ----------------------
    auto *totalsGroup = new QGroupBox("Resumen de presupuesto");
    auto *totalsLayout = new QGridLayout(totalsGroup);
    totalsLayout->addWidget(lbl1, 0, 0, Qt::AlignRight);
    totalsLayout->addWidget(lblTotalNoIVA, 0, 1, Qt::AlignLeft);
    totalsLayout->addWidget(lbl2, 1, 0, Qt::AlignRight);
    totalsLayout->addWidget(lblTotalConIVA, 1, 1, Qt::AlignLeft);
    totalsLayout->setContentsMargins(10, 10, 10, 10);

    // ---------------------- Layout principal (izquierda) ----------------------
    auto *leftV = new QVBoxLayout;
    leftV->addLayout(formLayout);
    leftV->addWidget(buttonsGroup);
    leftV->addWidget(totalsGroup);
    leftV->addStretch();

    // ---------------------- Right Column (Presupuestos) ----------------------
    auto *rightV = new QVBoxLayout;
    lwBudgets = new QListWidget;
    QPushButton *btnLoad = new QPushButton("Abrir seleccionado");
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadSelectedBudget);
    QPushButton *btnDelete = new QPushButton("Eliminar seleccionado");
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteSelectedBudget);

    // --- Nuevos botones ---
    QPushButton *btnClearFields = new QPushButton("Borrar campos");
    connect(btnClearFields, &QPushButton::clicked, this, &MainWindow::onDeleteFields);
    /* QPushButton *btnCloseProject = new QPushButton("Cerrar obra");
    connect(btnCloseProject, &QPushButton::clicked, this, &MainWindow::onToggleStatus); */
    QPushButton *btnCloseProject = new QPushButton("Cerrar obra");
    btnCloseProject->setObjectName("btnCloseProject");
    connect(btnCloseProject, &QPushButton::clicked, this, &MainWindow::onToggleStatus);

    // Layouts de botones
    auto *btnsLayout1 = new QHBoxLayout;
    btnsLayout1->addWidget(btnLoad);
    btnsLayout1->addWidget(btnDelete);

    auto *btnsLayout2 = new QHBoxLayout;
    btnsLayout2->addWidget(btnClearFields);
    btnsLayout2->addWidget(btnCloseProject);

    rightV->addWidget(new QLabel("Presupuestos guardados"));
    rightV->addWidget(lwBudgets);
    rightV->addLayout(btnsLayout1);
    rightV->addLayout(btnsLayout2);


    // ---------------------- Main Layout ----------------------
    mainLayout->addLayout(leftV, 3);
    mainLayout->addLayout(rightV, 1);
    w->setLayout(mainLayout);
    setCentralWidget(w);
    setWindowTitle("Australair - Gestor de presupuestos");

    // ---------------------- Cargar presupuestos guardados ----------------------
    QSqlQuery q(Database::instance());
    // q.exec("SELECT id, created_at FROM budgets ORDER BY created_at DESC");
    // while (q.next()) {
    //     int id = q.value(0).toInt();
    //     QString ts = q.value(1).toString();
    //     auto *it = new QListWidgetItem(QString::number(id) + " - " + ts);
    //     it->setData(Qt::UserRole, id);
    //     lwBudgets->addItem(it);
    // }
    q.exec("SELECT id, created_at, status FROM budgets ORDER BY created_at DESC");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString ts = q.value(1).toString();
        QString st = q.value(2).toString();
        QString label = QString("%1 - %2  (%3)").arg(id).arg(ts).arg(st.toUpper());
        auto *it = new QListWidgetItem(label);
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

void MainWindow::onCalculate() {
    double base = getSettingDouble("price_base", 10.0);
    double incr = getSettingDouble("increment_per_field", 5.0);
    double iva = getSettingDouble("iva_pct", 21.0);

    // Count filled "apartados": vamos a contar campos no vacíos entre los apartados clave
    int count = 0;
    if (!leClientName->text().isEmpty()) count++;
    if (sbMetros->value() > 0) count++;
    if (!cbTipoLocal->currentText().isEmpty()) count++;
    if (!leLocalidadObra->text().isEmpty()) count++;
    if (!cbTipoCubierta->currentText().isEmpty()) count++;
    if (!cbZona->currentText().isEmpty()) count++;
    if (sbKM->value() > 0) count++;
    if (sbLitros->value() > 0) count++;
    if (spDietas->value() > 0) count++;
    if (spDiasDieta->value() > 0) count++;
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
    double base = getSettingDouble("price_base", 10.0);
    double totalNoIva = lblTotalNoIVA->text().replace(" €","").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €","").toDouble();

    QSqlQuery qb(d);
    qb.prepare(R"(
        INSERT INTO budgets(client_id, tipo_local, metros, tipo_cubierta, zona, localidad, km, combustible, dietas, dietas_dias, horas, dias, base_price, total_no_iva, total_con_iva)
        VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
    )");
    qb.addBindValue(clientId);
    qb.addBindValue(cbTipoLocal->currentText());
    qb.addBindValue(sbMetros->value());
    qb.addBindValue(cbTipoCubierta->currentText());
    qb.addBindValue(cbZona->currentText()); 
    qb.addBindValue(leLocalidadObra ? leLocalidadObra->text() : QString());
    qb.addBindValue(sbKM->value());
    qb.addBindValue(sbLitros->value());
    qb.addBindValue(spDietas->value());
    qb.addBindValue(spDiasDieta->value());
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
    qb.prepare("SELECT client_id, metros, tipo_local, localidad, tipo_cubierta, km, combustible, dietas, horas, dias, zona, dietas_dias FROM budgets WHERE id = ?");
    qb.addBindValue(id);
    if (!qb.exec() || !qb.next()) return;
    int clientId = qb.value(0).toInt();
    sbMetros->setValue(qb.value(1).toDouble());
    cbTipoLocal->setCurrentText(qb.value(2).toString());
    leLocalidadObra->setText(qb.value(3).toString());
    cbTipoCubierta->setCurrentText(qb.value(4).toString());
    sbKM->setValue(qb.value(5).toDouble());
    spDietas->setValue(qb.value(6).toInt());
    sbHoras->setValue(qb.value(7).toDouble());
    spDias->setValue(qb.value(8).toInt());
    cbZona->setCurrentText(qb.value(9).toString());
    sbLitros->setValue(qb.value(10).toDouble());
    spDiasDieta->setValue(qb.value(11).toInt());

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