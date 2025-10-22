#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    if (!Database::init())
    {
        QMessageBox::critical(nullptr, "DB", "No se pudo abrir la base de datos");
        exit(1);
    }
    setupUi();
    loadSettings();
    setNextBudgetNumberAndDate();
    showMaximized();
}

void MainWindow::setupUi()
{
    QWidget *w = new QWidget;
    auto *mainLayout = new QHBoxLayout;

    // ---------------------- Formulario ----------------------
    auto *formLayout = new QFormLayout;

    // --- Campos cliente ---
    leClientName = new QLineEdit; leCompany = new QLineEdit;
    leContact = new QLineEdit; leAddress = new QLineEdit;
    lePhone = new QLineEdit; leEmail = new QLineEdit;
    leCIF = new QLineEdit; leNumPresu = new QLineEdit;
    leFecha = new QLineEdit;

    auto *customerLayout = new QGridLayout;
    customerLayout->addWidget(new QLabel("Nº Presp.:"), 0, 0);
    customerLayout->addWidget(leNumPresu, 0, 1);
    customerLayout->addWidget(new QLabel("Cliente (Empresa):"), 0, 2);
    customerLayout->addWidget(leClientName, 0, 3);
    customerLayout->addWidget(new QLabel("Obra:"), 0, 4);
    customerLayout->addWidget(leCompany, 0, 5);

    customerLayout->addWidget(new QLabel("CIF:"), 1, 0);
    customerLayout->addWidget(leCIF, 1, 1);
    customerLayout->addWidget(new QLabel("Contacto:"), 1, 2);
    customerLayout->addWidget(leContact, 1, 3);
    customerLayout->addWidget(new QLabel("Teléfono:"), 1, 4);
    customerLayout->addWidget(lePhone, 1, 5);

    customerLayout->addWidget(new QLabel("Dirección:"), 2, 0);
    customerLayout->addWidget(leAddress, 2, 1);
    customerLayout->addWidget(new QLabel("Email:"), 2, 2);
    customerLayout->addWidget(leEmail, 2, 3);
    customerLayout->addWidget(new QLabel("Fecha: "), 2, 4);
    customerLayout->addWidget(leFecha, 2, 5);

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
        if (zona == "Zona Centro")
        {
            // Zona Centro: dietas siempre desactivadas
            rbDietasSi->setChecked(false);
            rbDietasSi->setEnabled(false);
            rbDietasNo->setChecked(true);
            rbDietasNo->setEnabled(false);
            spDietas->setEnabled(false);
            spDiasDieta->setEnabled(false);
        }
        else
        {
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
    twMaterials = new QTableWidget(0, 4);
    twMaterials->setHorizontalHeaderLabels({"Nombre", "Cantidad", "Precio unit.", "Total"});
    twMaterials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // --- Cargar materiales desde archivo ---
    //QMap<QString, double> materialsMap = loadMaterialsFromFile("/materials.txt");
    QMap<QString, double> materialsMap = loadMaterialsFromFile(QCoreApplication::applicationDirPath() + "/materials.txt");


    // --- Combobox para seleccionar materiales ---
    cbMaterials = new QComboBox;
    for (auto it = materialsMap.begin(); it != materialsMap.end(); ++it)
        cbMaterials->addItem(it.key());
    cbMaterials->addItem("Añadir material"); // opción para introducir manualmente

    // --- Botones ---
    btnAddMat = new QPushButton("Añadir material");
    btnRemoveMat = new QPushButton("Eliminar material");

    // --- Conexiones ---
    connect(btnAddMat, &QPushButton::clicked, this, [=]() {
        QString materialName = cbMaterials->currentText();
        double unitPrice = 0.0;

        if (materialName == "Añadir material")
        {
            bool ok;
            materialName = QInputDialog::getText(this, "Nuevo material", "Nombre del material:", QLineEdit::Normal, "", &ok);
            if (!ok || materialName.isEmpty())
                return;

            unitPrice = QInputDialog::getDouble(this, "Nuevo material", "Precio unitario:", 0.0, 0, 1e6, 2, &ok);
            if (!ok)
                return;
        }
        else
        {
            unitPrice = materialsMap.value(materialName, 0.0);
        }

        int row = twMaterials->rowCount();
        twMaterials->insertRow(row);

        twMaterials->setItem(row, 0, new QTableWidgetItem(materialName));
        twMaterials->setItem(row, 1, new QTableWidgetItem("1")); // cantidad inicial 1
        twMaterials->setItem(row, 2, new QTableWidgetItem(QString::number(unitPrice, 'f', 2)));
        twMaterials->setItem(row, 3, new QTableWidgetItem(QString::number(unitPrice, 'f', 2)));
    });

    connect(btnRemoveMat, &QPushButton::clicked, this, [=]() {
        auto selectedRows = twMaterials->selectionModel()->selectedRows();
        for (auto it = selectedRows.rbegin(); it != selectedRows.rend(); ++it)
        {
            twMaterials->removeRow(it->row());
        }
    });

    // --- Layout ---
    QWidget *matWidget = new QWidget;
    auto *matLayout = new QVBoxLayout;
    matLayout->addWidget(twMaterials);

    auto *matBtns = new QHBoxLayout;
    matBtns->addWidget(cbMaterials);
    matBtns->addWidget(btnAddMat);
    matBtns->addWidget(btnRemoveMat);

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

    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbHoras, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spDietas, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spDiasDieta, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(twMaterials, &QTableWidget::itemChanged, this, &MainWindow::onCalculate);

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

    // --- Buscador de clientes con botón de limpiar ---
    auto *searchLayout = new QHBoxLayout;
    QLineEdit *leSearchClient = new QLineEdit;
    leSearchClient->setPlaceholderText("Buscar cliente...");

    QPushButton *btnClearSearch = new QPushButton("✕");
    btnClearSearch->setToolTip("Limpiar búsqueda");
    btnClearSearch->setFixedSize(24, 24);

    // Conexión para filtrar dinámicamente
    connect(leSearchClient, &QLineEdit::textChanged, this, [this](const QString &text) {
        for (int i = 0; i < lwBudgets->count(); ++i) {
            QListWidgetItem *item = lwBudgets->item(i);
            item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
        }
    });

    // Conexión del botón para limpiar
    connect(btnClearSearch, &QPushButton::clicked, leSearchClient, &QLineEdit::clear);

    // Añadimos los widgets al layout
    searchLayout->addWidget(leSearchClient);
    searchLayout->addWidget(btnClearSearch);

    // Agregamos a la columna derecha
    rightV->addWidget(new QLabel("Presupuestos guardados"));
    rightV->addLayout(searchLayout);
    rightV->addWidget(lwBudgets);

    // rightV->addWidget(new QLabel("Presupuestos guardados"));
    // rightV->addWidget(lwBudgets);
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
    q.exec(R"(
        SELECT b.id, c.name, b.created_at, b.status
        FROM budgets b
        LEFT JOIN clients c ON b.client_id = c.id
        ORDER BY b.created_at DESC
    )");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString clientName = q.value(1).toString();
        QString ts = q.value(2).toString();
        QString st = q.value(3).toString();

        QString label = QString("%1 - %2  |  %3  (%4)")
                            .arg(id)
                            .arg(clientName.isEmpty() ? "Sin cliente" : clientName)
                            .arg(ts)
                            .arg(st.toUpper());
        auto *it = new QListWidgetItem(label);
        it->setData(Qt::UserRole, id);
        lwBudgets->addItem(it);
    }

}

double MainWindow::getSettingDouble(const QString &key, double def)
{
    QSqlQuery q(Database::instance());
    q.prepare("SELECT value FROM settings WHERE key = ?");
    q.addBindValue(key);
    if (q.exec() && q.next())
    {
        bool ok = false;
        double v = q.value(0).toString().toDouble(&ok);
        if (ok)
            return v;
    }
    return def;
}

void MainWindow::loadSettings()
{
    // nothing to do now, will read when calculating
}

void MainWindow::onCalculate()
{
    // --- Cargar precios desde archivo ---
    QMap<QString, double> prices = loadPricesFromFile(QCoreApplication::applicationDirPath() + "/prices.txt");
    double iva = getSettingDouble("iva_pct", 21.0);

    double total = 0.0;

    // --- Sumar materiales ---
    for (int r = 0; r < twMaterials->rowCount(); ++r)
    {
        QTableWidgetItem *qtyItem = twMaterials->item(r, 1);
        QTableWidgetItem *priceItem = twMaterials->item(r, 2);
        QTableWidgetItem *totalItem = twMaterials->item(r, 3);

        double qty = qtyItem ? qtyItem->text().toDouble() : 0.0;
        double up = priceItem ? priceItem->text().toDouble() : 0.0;
        double line = qty * up;

        if (totalItem)
            totalItem->setText(QString::number(line, 'f', 2));
        total += line;
    }

    // --- Coste combustible ---
    double fuelPrice = prices.value("fuel", 1.4);  // €/litro
    double litrosConsumidos = sbKM->value() * 0.1; // 10 L/100 km
    total += litrosConsumidos * fuelPrice;

    // --- Dietas ---
    double dietaPrice = prices.value("dieta", 300);
    total += spDietas->value() * spDiasDieta->value() * dietaPrice;

    // --- Horas de trabajo ---
    double horaPrice = prices.value("hora_trabajo", 80);
    total += sbHoras->value() * horaPrice;

    // --- Total con IVA ---
    double totalConIva = total * (1.0 + iva / 100.0);
    lblTotalNoIVA->setText(QString::number(total, 'f', 2) + " €");
    lblTotalConIVA->setText(QString::number(totalConIva, 'f', 2) + " €");
}

void MainWindow::onSaveBudget()
{
    // grab client -> insert or reuse
    QSqlDatabase d = Database::instance();
    QSqlQuery q(d);
    QString client = leClientName->text();

    q.prepare("INSERT INTO clients(budget,name,company,cif,contact,address,phone,email,fecha) VALUES(?,?,?,?,?,?,?,?,?)");
    q.addBindValue(leNumPresu->text());
    q.addBindValue(leClientName->text());
    q.addBindValue(leCompany->text());
    q.addBindValue(leCIF->text());
    q.addBindValue(leContact->text());
    q.addBindValue(leAddress->text());
    q.addBindValue(lePhone->text());
    q.addBindValue(leEmail->text());
    q.addBindValue(leFecha->text());
    if (!q.exec())
    {
        QMessageBox::warning(this, "DB", "Error al guardar cliente: " + q.lastError().text());
        return;
    }
    int clientId = q.lastInsertId().toInt();

    // budget
    double base = getSettingDouble("price_base", 10.0);
    double totalNoIva = lblTotalNoIVA->text().replace(" €", "").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €", "").toDouble();

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
    if (!qb.exec())
    {
        QMessageBox::warning(this, "DB", "Error al guardar presupuesto: " + qb.lastError().text());
        return;
    }
    int budgetId = qb.lastInsertId().toInt();

    // materials
    QSqlQuery qm(d);
    qm.prepare("INSERT INTO materials(budget_id,name,quantity,unit_price) VALUES(?,?,?,?)");
    for (int r = 0; r < twMaterials->rowCount(); ++r)
    {
        QString name = twMaterials->item(r, 0)->text();
        double qty = twMaterials->item(r, 1)->text().toDouble();
        double up = twMaterials->item(r, 2)->text().toDouble();
        qm.addBindValue(budgetId);
        qm.addBindValue(name);
        qm.addBindValue(qty);
        qm.addBindValue(up);
        if (!qm.exec())
        {
            qDebug() << "Error saving material:" << qm.lastError().text();
        }
    }

    // add to list widget
   /*  auto *it = new QListWidgetItem(QString::number(budgetId) + " - " + QDateTime::currentDateTime().toString());
    it->setData(Qt::UserRole, budgetId);
    lwBudgets->insertItem(0, it); */
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
    auto *it = lwBudgets->currentItem();
    if (!it)
        return;
    int id = it->data(Qt::UserRole).toInt();
    QSqlQuery qb(Database::instance());
    qb.prepare("SELECT client_id, metros, tipo_local, localidad, tipo_cubierta, km, combustible, dietas, horas, dias, zona, dietas_dias FROM budgets WHERE id = ?");
    qb.addBindValue(id);
    if (!qb.exec() || !qb.next())
        return;
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
    if (qc.exec() && qc.next())
    {
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
    if (qm.exec())
    {
        while (qm.next())
        {
            int r = twMaterials->rowCount();
            twMaterials->insertRow(r);
            twMaterials->setItem(r, 0, new QTableWidgetItem(qm.value(0).toString()));
            twMaterials->setItem(r, 1, new QTableWidgetItem(qm.value(1).toString()));
            twMaterials->setItem(r, 2, new QTableWidgetItem(qm.value(2).toString()));
            double line = qm.value(1).toDouble() * qm.value(2).toDouble();
            twMaterials->setItem(r, 3, new QTableWidgetItem(QString::number(line, 'f', 2)));
        }
    }
    onCalculate();
}

void MainWindow::onEditPrices()
{
    bool ok;
    double newBase = QInputDialog::getDouble(this, "Precio base", "Precio base (€):", getSettingDouble("price_base", 10.0), 0, 1e9, 2, &ok);
    if (!ok)
        return;
    double newInc = QInputDialog::getDouble(this, "Incremento por apartado", "Incremento (€):", getSettingDouble("increment_per_field", 5.0), 0, 1e9, 2, &ok);
    if (!ok)
        return;
    double newIva = QInputDialog::getDouble(this, "IVA (%)", "IVA porcentaje:", getSettingDouble("iva_pct", 21.0), 0, 100, 2, &ok);
    if (!ok)
        return;

    QSqlQuery q(Database::instance());
    q.prepare("REPLACE INTO settings(key,value) VALUES(?,?)");
    q.addBindValue("price_base");
    q.addBindValue(QString::number(newBase));
    q.exec();
    q.addBindValue("increment_per_field");
    q.addBindValue(QString::number(newInc));
    q.exec();
    q.addBindValue("iva_pct");
    q.addBindValue(QString::number(newIva));
    q.exec();

    QMessageBox::information(this, "Ajustes", "Precios actualizados.");
}
