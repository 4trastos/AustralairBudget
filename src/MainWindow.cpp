#include "MainWindow.hpp"
#include "Database.hpp"
#include "AustralairBudget.hpp"
#include "MaterialsWindow.hpp"

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

    // ---------------------- Bloque Datos del Cliente ----------------------
    auto *customerGroup = new QGroupBox("Datos del cliente");
    auto *customerLayout = new QGridLayout(customerGroup);

    // Campos cliente
    leClientName = new QLineEdit; 
    leCompany = new QLineEdit;
    leContact = new QLineEdit; 
    leAddress = new QLineEdit;
    lePhone = new QLineEdit; 
    leEmail = new QLineEdit;
    leCIF = new QLineEdit; 
    leNumPresu = new QLineEdit;
    leFecha = new QLineEdit;

    // Fila 0
    customerLayout->addWidget(new QLabel("Nº Presp.:"), 0, 0);
    customerLayout->addWidget(leNumPresu, 0, 1);
    customerLayout->addWidget(new QLabel("Cliente (Empresa):"), 0, 2);
    customerLayout->addWidget(leClientName, 0, 3);
    customerLayout->addWidget(new QLabel("Dirección de Obra:"), 0, 4);
    customerLayout->addWidget(leCompany, 0, 5);

    // Fila 1
    customerLayout->addWidget(new QLabel("CIF:"), 1, 0);
    customerLayout->addWidget(leCIF, 1, 1);
    customerLayout->addWidget(new QLabel("Contacto Gerencia:"), 1, 2);
    customerLayout->addWidget(leContact, 1, 3);
    customerLayout->addWidget(new QLabel("Contacto Administración:"), 1, 4);
    customerLayout->addWidget(lePhone, 1, 5);

    // Fila 2
    customerLayout->addWidget(new QLabel("Dirección:"), 2, 0);
    customerLayout->addWidget(leAddress, 2, 1);
    customerLayout->addWidget(new QLabel("Email:"), 2, 2);
    customerLayout->addWidget(leEmail, 2, 3);
    customerLayout->addWidget(new QLabel("Fecha:"), 2, 4);
    customerLayout->addWidget(leFecha, 2, 5);

    // Márgenes proporcionales
    customerLayout->setContentsMargins(10, 10, 10, 10);
    customerLayout->setHorizontalSpacing(12);
    customerLayout->setVerticalSpacing(8);

    // Añadir al layout principal
    formLayout->addRow(customerGroup);

    // ---------------------- Bloque Obra ----------------------
    cbZona = new QComboBox;
    leLocalidadObra = new QLineEdit; 
    cbElevador = new QComboBox; cbElevador->addItems({"No","Si"});
    sbElevPortes = new QDoubleSpinBox; sbElevPortes->setRange(0, 1e6); sbElevPortes->setSuffix(" €");
    spElevDia = new QSpinBox; spElevDia->setRange(0,1000);
    spElevPrecDia = new QSpinBox; spElevPrecDia->setRange(0, 3000);
    cbDietasYes = new QComboBox; cbDietasYes->addItems({"No","Si"});
    leExtra = new QLineEdit;

    // ---------------------- Bloque Obra completo en marco ----------------------
    auto *obraZonaGroup = new QGroupBox("Datos de la obra");
    auto *obraZonaLayout = new QGridLayout(obraZonaGroup);

    // ---- Campos principales de obra ----
    sbMetros = new QDoubleSpinBox; sbMetros->setRange(0, 1e6); sbMetros->setSuffix(" m²");
    cbTipoLocal = new QComboBox; cbTipoLocal->addItems({" ··· ","Nave industrial","Local comercial","Vivienda","Complejo Deportivo"});
    cbTipoCubierta = new QComboBox; cbTipoCubierta->addItems({" ··· ","Chapa","Teja","Cubierta plana","Panel sandwich", "Uralita"});
    sbKM = new QDoubleSpinBox; sbKM->setRange(0,10000); sbKM->setSuffix(" km");
    sbLitros = new QDoubleSpinBox; sbLitros->setRange(0,10000); sbLitros->setSuffix(" lts");
    sbHoras = new QDoubleSpinBox; sbHoras->setRange(0,1e5);
    spDias = new QSpinBox; spDias->setRange(0,365);
    leLocalidadObra = new QLineEdit;
    spOperarios = new QSpinBox;spOperarios->setRange(0,60);
    cbExtractor = new QComboBox; cbExtractor->addItems({"No", "Si"});
    sbHorasViaje = new QDoubleSpinBox; sbHorasViaje->setRange(0,1e5);
    spPrecioDiet = new QSpinBox; spPrecioDiet->setRange(0,1000);
    spFurgonetas = new QSpinBox; spFurgonetas->setRange(0,100);

    // ---- Elevador ----
    cbElevador = new QComboBox; cbElevador->addItems({"No","Si"});
    sbElevPortes = new QDoubleSpinBox; sbElevPortes->setRange(0, 1e6); sbElevPortes->setSuffix(" €");
    spElevDia = new QSpinBox; spElevDia->setRange(0,1000);
    spElevPrecDia = new QSpinBox; spElevPrecDia->setRange(0, 3000);
    sbElevPortes->setEnabled(false);
    spElevDia->setEnabled(false);
    spElevPrecDia->setEnabled(false);

    // ---- Zona y dietas ----
    cbZona = new QComboBox; 
    cbZona->addItems({"Zona Centro","Otras Zonas"});
    cbDietasYes = new QComboBox; cbDietasYes->addItems({"No","Si"});
    rbCorta = new QRadioButton("Corta Distancia");
    rbMedia  = new QRadioButton("Media Distancia");
    rbLarga  = new QRadioButton("Larga Distancia");
    spDietas = new QSpinBox; spDietas->setRange(0,1000);
    spDiasDieta = new QSpinBox; spDiasDieta->setRange(0,365);
    spDietas->setEnabled(false);
    spDiasDieta->setEnabled(false);
    cbDietasYes->setEnabled(false);

    // --- FILA 1: Tipo local, metros, tipo cubierta ---
    obraZonaLayout->addWidget(new QLabel("Tipo de local:"), 0, 0);
    obraZonaLayout->addWidget(cbTipoLocal, 0, 1);
    obraZonaLayout->addWidget(new QLabel("Tipo de cubierta:"), 0, 2);
    obraZonaLayout->addWidget(cbTipoCubierta, 0, 3);
    obraZonaLayout->addWidget(new QLabel("Metros cuadrados:"), 0, 4);
    obraZonaLayout->addWidget(sbMetros, 0, 5);
    obraZonaLayout->addWidget(new QLabel("Extracción:"), 0, 6);
    obraZonaLayout->addWidget(cbExtractor, 0, 7);

    // --- FILA 2: Elevación, portes, días ---
    obraZonaLayout->addWidget(new QLabel("Elevación:"), 1, 0);
    obraZonaLayout->addWidget(cbElevador, 1, 1);
    obraZonaLayout->addWidget(new QLabel("Portes:"), 1, 2);
    obraZonaLayout->addWidget(sbElevPortes, 1, 3);
    obraZonaLayout->addWidget(new QLabel("Días:"), 1, 4);
    obraZonaLayout->addWidget(spElevDia, 1, 5);
    obraZonaLayout->addWidget(new QLabel("Precio x Dia:"), 1, 6);
    obraZonaLayout->addWidget(spElevPrecDia, 1, 7);

    // --- FILA 3: Zona centro ---
    obraZonaLayout->addWidget(new QLabel("Zona:"), 2, 0);
    obraZonaLayout->addWidget(cbZona, 2, 1);
    obraZonaLayout->addWidget(rbCorta, 2, 3);
    //obraZonaLayout->addWidget(rbMedia, 2, 4);
    obraZonaLayout->addWidget(rbLarga, 2, 5);

    // --- FILA 4: Dietas otras zonas ---
    obraZonaLayout->addWidget(new QLabel("Dietas:"), 3, 0);
    obraZonaLayout->addWidget(cbDietasYes, 3, 1);
    obraZonaLayout->addWidget(new QLabel("Nº Operarios:"), 3, 2);
    obraZonaLayout->addWidget(spDietas, 3, 3);
    obraZonaLayout->addWidget(new QLabel("Días:"), 3, 4);
    obraZonaLayout->addWidget(spDiasDieta, 3, 5);
    obraZonaLayout->addWidget(new QLabel("Precio X Día:"), 3, 6);
    obraZonaLayout->addWidget(spPrecioDiet, 3, 7);

    // --- FILA 5: Localidad y Kilometros ---
    //obraZonaLayout->addWidget(new QLabel("Localidad:"), 4, 0);
    //obraZonaLayout->addWidget(leLocalidadObra, 4, 1);
    obraZonaLayout->addWidget(new QLabel("Furgonetas:"), 4, 0);
    obraZonaLayout->addWidget(spFurgonetas, 4, 1);
    obraZonaLayout->addWidget(new QLabel("KM desplazamiento:"), 4, 2);
    obraZonaLayout->addWidget(sbKM, 4, 3);
    obraZonaLayout->addWidget(new QLabel("Combustible:"), 4, 4);
    obraZonaLayout->addWidget(sbLitros, 4, 5);
    //obraZonaLayout->addWidget(new QLineEdit, 4, 3);

    // --- FILA 6: Horas y Operarios ---
    obraZonaLayout->addWidget(new QLabel("Nº Operarios:"), 5, 0);
    obraZonaLayout->addWidget(spOperarios, 5, 1);
    obraZonaLayout->addWidget(new QLabel("Días"), 5, 2);
    obraZonaLayout->addWidget(spDias, 5, 3);
    obraZonaLayout->addWidget(new QLabel("Horas Obra:"), 5, 4);
    obraZonaLayout->addWidget(sbHoras, 5, 5);
    obraZonaLayout->addWidget(new QLabel("Horas Viaje:"), 5, 6);
    obraZonaLayout->addWidget(sbHorasViaje, 5, 7);
    leExtra->setEnabled(false);

    // --- Proporciones y márgenes ---
    obraZonaLayout->setColumnStretch(1, 1);
    obraZonaLayout->setColumnStretch(3, 1);
    obraZonaLayout->setColumnStretch(5, 1);
    obraZonaLayout->setContentsMargins(10, 10, 10, 10);
    obraZonaLayout->setHorizontalSpacing(12);
    obraZonaLayout->setVerticalSpacing(8);

    // Añadir el bloque completo al formulario principal
    formLayout->addRow(obraZonaGroup);

    // ---------------------- Lógica de desbloqueo ----------------------
    connect(cbElevador, &QComboBox::currentTextChanged, this, [=](const QString &elev){
        bool enabled = (elev == "Si");
        sbElevPortes->setEnabled(enabled);
        spElevDia->setEnabled(enabled);
        spElevPrecDia->setEnabled(enabled);
    });

    connect(cbZona, &QComboBox::currentTextChanged, this, [=](const QString &zona){
        if (zona == "Zona Centro") {
            rbCorta->setEnabled(true);
            rbMedia->setEnabled(true);
            rbLarga->setEnabled(true);
            cbDietasYes->setCurrentText("No");
            cbDietasYes->setEnabled(false);
            spDietas->setEnabled(false);
            spDiasDieta->setEnabled(false);
        } else if (zona == "Otras Zonas") {
            rbCorta->setEnabled(false);
            rbMedia->setEnabled(false);
            rbLarga->setEnabled(false);
            cbDietasYes->setEnabled(true);
            cbDietasYes->setCurrentText("Si");
            spDietas->setEnabled(true);
            spDiasDieta->setEnabled(true);
        } else {
            rbCorta->setEnabled(false);
            rbMedia->setEnabled(false);
            rbLarga->setEnabled(false);
            cbDietasYes->setEnabled(false);
            spDietas->setEnabled(false);
            spDiasDieta->setEnabled(false);
        }
    });

    // --- Sincronizar operarios y días generales con dietas ---
    auto syncGeneralFromDietas = [=]() {
        if (cbDietasYes->currentText() == "Si") {
            spOperarios->setValue(spDietas->value());
            spDias->setValue(spDiasDieta->value());
            spOperarios->setEnabled(false);
            spDias->setEnabled(false);
        } else {
            spOperarios->setEnabled(true);
            spDias->setEnabled(true);
        }
    };

    // --- Conectar cambios ---
    connect(cbDietasYes, &QComboBox::currentTextChanged, this, syncGeneralFromDietas);
    connect(spDietas, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        if (cbDietasYes->currentText() == "Si") spOperarios->setValue(val);
    });
    connect(spDiasDieta, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        if (cbDietasYes->currentText() == "Si") spDias->setValue(val);
    });

    // --- Inicializar estado correcto ---
    syncGeneralFromDietas();


    // ---------------------- Lógica Elevador ----------------------
    connect(cbElevador, &QComboBox::currentTextChanged, this, [=](const QString &elevador){
        bool elevSi = (elevador == "Si");

        sbElevPortes->setEnabled(elevSi);
        spElevDia->setEnabled(elevSi);
    });

    // --- Conexión para Kilometros / Combustible ---
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::updateDistanceAndFuel);

    // --- Conexión para Kilometros / Combustible ---
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::updateDistanceAndFuel);


    // ---------------------- Bloque Materiales en marco ----------------------
    auto *materialsGroup = new QGroupBox("Materiales");
    auto *materialsLayout = new QVBoxLayout(materialsGroup);

    // --- Tabla de materiales ---
    twMaterials = new QTableWidget(0, 5);
    twMaterials->setHorizontalHeaderLabels({"Nombre", "Cantidad", "Precio Venta", "Precio Compra", "Total Venta"});
    twMaterials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    materialsLayout->addWidget(twMaterials);

    // --- Cargar materiales desde archivo (member variable) ---
    //materialsMap = loadMaterialsFromFile(QCoreApplication::applicationDirPath() + "/materials.txt");

    // --- Combobox y botones ---
    cbMaterials = new QComboBox;
    cbMaterials->addItem("Abrir Gestor de Materiales..."); // ABRIR VENTANA MATERIALES

    btnAddMat = new QPushButton("Añadir material (NUEVO)");
    btnRemoveMat = new QPushButton("Eliminar material");

    QPushButton *btnOpenMaterials = new QPushButton("Abrir Gestor de Materiales");
    btnOpenMaterials->setIcon(QIcon::fromTheme("document-open"));

    auto *matBtns = new QHBoxLayout;
    matBtns->addWidget(btnOpenMaterials); 
    matBtns->addWidget(btnAddMat);
    matBtns->addWidget(btnRemoveMat);
    materialsLayout->addLayout(matBtns);

    materialsLayout->setContentsMargins(10,10,10,10);

    // --- Conectar el nuevo botón al slot ---
    connect(btnOpenMaterials, &QPushButton::clicked, this, &MainWindow::onOpenMaterialsWindow);

    connect(cbMaterials, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        if (index == 0) { // Siempre será 0
            onOpenMaterialsWindow();
        }
    });

    // --- Conexiones MODIFICADAS para Añadir material (1.c) ---
    connect(btnAddMat, &QPushButton::clicked, this, [this]() {
        // El botón ahora SOLO tiene la función de añadir material manual
        bool ok;
        QString materialName = QInputDialog::getText(this, "Nuevo material manual", "Nombre del material:", QLineEdit::Normal, "", &ok);
        if (!ok || materialName.isEmpty()) return;

        double sellPrice = QInputDialog::getDouble(this, "Nuevo material manual", "Precio de VENTA al cliente:", 0.0, 0, 1e6, 2, &ok);
        if (!ok) return;
        
        // Se mantiene la lógica de costo
        double costPrice = QInputDialog::getDouble(this, "Nuevo material manual", "Precio de COMPRA (costo real):", sellPrice * 0.7, 0, 1e6, 2, &ok);
        if (!ok) return;

        // Se mantiene la inserción en la tabla
        int row = twMaterials->rowCount();
        twMaterials->insertRow(row);
        twMaterials->setItem(row, 0, new QTableWidgetItem(materialName));
        twMaterials->setItem(row, 1, new QTableWidgetItem("1"));
        twMaterials->setItem(row, 2, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
        twMaterials->setItem(row, 3, new QTableWidgetItem(QString::number(costPrice, 'f', 2)));
        twMaterials->setItem(row, 4, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));

        // NO necesitas añadirlo a materialsMap porque es un material "manual" para este presupuesto
    });

    connect(twMaterials, &QTableWidget::cellChanged, this, [this](int row, int column) {
        // Solo nos interesa cuando cambia la columna de Cantidad (columna 1)
        if (column == 1) {
            updateMaterialTotal(row);
        }
    });

    // Configurar selección de filas completas (una sola vez)
    twMaterials->setSelectionBehavior(QAbstractItemView::SelectRows);
    twMaterials->setSelectionMode(QAbstractItemView::ExtendedSelection); // permite varias filas

    // Conexión corregida para eliminar materiales
    connect(btnRemoveMat, &QPushButton::clicked, this, [this]() {
        auto selectedRows = twMaterials->selectionModel()->selectedRows();

        // recorre en orden inverso para no desordenar el índice
        std::sort(selectedRows.begin(), selectedRows.end(), [](const QModelIndex &a, const QModelIndex &b){
            return a.row() > b.row();
        });

        for (const auto &index : selectedRows) {
            twMaterials->removeRow(index.row());
        }

        // recalcular totales después de eliminar
        onCalculate();
    });


    // --- Añadir el grupo al form layout ---
    formLayout->addRow(materialsGroup);

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

    // --- Cálculo Beneficios --------------------------------- 
    QLabel *lbl3 = new QLabel("Costo estimado:");  
    lblCostoEstimado = new QLabel("0.00 €");   
    QLabel *lbl4 = new QLabel("Beneficio estimado:"); 
    lblBeneficioEstimado = new QLabel("0.00 €");     

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

    totalsLayout->addWidget(lbl3, 2, 0, Qt::AlignRight);     
    totalsLayout->addWidget(lblCostoEstimado, 2, 1, Qt::AlignLeft);  
    totalsLayout->addWidget(lbl4, 3, 0, Qt::AlignRight);          
    totalsLayout->addWidget(lblBeneficioEstimado, 3, 1, Qt::AlignLeft);  

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

        QString estadoTexto = (st == "cerrada") ? "CERRADA" : "ABIERTA";
        QString emoji = (st == "cerrada") ? "●" : "○";
        
        QString label = QString("%1 - %2  |  %3  (%4) %5")
                            .arg(id)
                            .arg(clientName.isEmpty() ? "Sin cliente" : clientName)
                            .arg(ts)
                            .arg(estadoTexto)
                            .arg(emoji);
        auto *it = new QListWidgetItem(label);
        it->setData(Qt::UserRole, id);
        
        lwBudgets->addItem(it);
    }
}

void MainWindow::onOpenMaterialsWindow()
{
    // Crear la ventana de materiales
    MaterialsWindow *mw = new MaterialsWindow(this);
    
    // Mostrar la ventana modal. Si el usuario hace clic en "Guardar Selección y Cerrar" (accept)...
    if (mw->exec() == QDialog::Accepted) {
        QList<SelectedMaterial> newMaterials = mw->getSelectedMaterials();
        
        for (const auto &mat : newMaterials) {
            // Insertar fila en la tabla principal twMaterials
            int row = twMaterials->rowCount();
            twMaterials->insertRow(row);
            twMaterials->setItem(row, 0, new QTableWidgetItem(mat.name));
            twMaterials->setItem(row, 1, new QTableWidgetItem(QString::number(mat.quantity))); // Cantidad
            twMaterials->setItem(row, 2, new QTableWidgetItem(QString::number(mat.pvp, 'f', 2))); // Precio Venta
            twMaterials->setItem(row, 3, new QTableWidgetItem(QString::number(mat.cost, 'f', 2))); // Precio Compra
            twMaterials->setItem(row, 4, new QTableWidgetItem(QString::number(mat.pvp * mat.quantity, 'f', 2))); // Total Venta (inicial)
            
            // Si quieres que el material esté disponible para el cálculo de costos (aunque ya lo obtienes de la tabla)
            materialsMap[mat.name] = qMakePair(mat.pvp, mat.cost);
        }
        
        if (!newMaterials.isEmpty()) {
            onCalculate(); // Recalcular después de añadir
        }
    }
    delete mw;
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

