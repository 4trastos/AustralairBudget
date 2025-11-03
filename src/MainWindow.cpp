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

//MainWindow::~MainWindow(){}

// DESKTOP 
void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    auto *mainSplitter = new QSplitter(Qt::Horizontal, central);

    // ==================== COLUMNA IZQUIERDA ====================
    auto *leftWidget = new QWidget;
    auto *leftV = new QVBoxLayout(leftWidget);

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

    customerLayout->setContentsMargins(10, 10, 10, 10);
    customerLayout->setHorizontalSpacing(12);
    customerLayout->setVerticalSpacing(8);

    formLayout->addRow(customerGroup);

    // ---------------------- Bloque Obra ----------------------
    auto *obraZonaGroup = new QGroupBox("Datos de la obra");
    auto *obraZonaLayout = new QGridLayout(obraZonaGroup);

    // Campos de obra
    sbMetros = new QDoubleSpinBox; sbMetros->setRange(0, 1e6); sbMetros->setSuffix(" m²");
    cbTipoLocal = new QComboBox; cbTipoLocal->addItems({" ··· ","Nave industrial","Local comercial","Vivienda","Complejo Deportivo"});
    cbTipoCubierta = new QComboBox; cbTipoCubierta->addItems({" ··· ","Chapa","Teja","Cubierta plana","Panel sandwich", "Uralita"});
    sbKM = new QDoubleSpinBox; sbKM->setRange(0,10000); sbKM->setSuffix(" km");
    sbLitros = new QDoubleSpinBox; sbLitros->setRange(0,10000); sbLitros->setSuffix(" lts");
    sbHoras = new QDoubleSpinBox; sbHoras->setRange(0,1e5);
    spDias = new QSpinBox; spDias->setRange(0,365);
    leLocalidadObra = new QLineEdit;
    spOperarios = new QSpinBox; spOperarios->setRange(0,60);
    cbExtractor = new QComboBox; cbExtractor->addItems({"No", "Si"});
    sbHorasViaje = new QDoubleSpinBox; sbHorasViaje->setRange(0,1e5);
    spFurgonetas = new QSpinBox; spFurgonetas->setRange(0,100);
    
    sbCosteFurgo = new QDoubleSpinBox; sbCosteFurgo->setRange(0, 1e6); sbCosteFurgo->setSuffix(" €");
    sbCosteFurgo->setValue(getSettingDouble("cost_van_day", 95.0));
    sbCosteFurgo->setEnabled(false);
    sbPrecioDiet = new QDoubleSpinBox; sbPrecioDiet->setRange(0,1e6); sbPrecioDiet->setSuffix(" €");
    sbPrecioDiet->setValue(getSettingDouble("dieta_price", 150.0));
    sbPrecioDiet->setEnabled(false);

    cbElevador = new QComboBox; cbElevador->addItems({"No","Si"});
    sbElevPortes = new QDoubleSpinBox; sbElevPortes->setRange(0, 1e6); sbElevPortes->setSuffix(" €");
    spElevDia = new QSpinBox; spElevDia->setRange(0,1000);
    spElevPrecDia = new QSpinBox; spElevPrecDia->setRange(0, 3000);
    sbElevPortes->setEnabled(false);
    spElevDia->setEnabled(false);
    spElevPrecDia->setEnabled(false);

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
    obraZonaLayout->addWidget(rbLarga, 2, 5);

    // --- FILA 4: Dietas otras zonas ---
    obraZonaLayout->addWidget(new QLabel("Dietas:"), 3, 0);
    obraZonaLayout->addWidget(cbDietasYes, 3, 1);
    obraZonaLayout->addWidget(new QLabel("Nº Operarios:"), 3, 2);
    obraZonaLayout->addWidget(spDietas, 3, 3);
    obraZonaLayout->addWidget(new QLabel("Días:"), 3, 4);
    obraZonaLayout->addWidget(spDiasDieta, 3, 5);
    obraZonaLayout->addWidget(new QLabel("Precio X Día:"), 3, 6);
    obraZonaLayout->addWidget(sbPrecioDiet, 3, 7);

    // --- FILA 5: Furgonetas y combustible ---
    obraZonaLayout->addWidget(new QLabel("Furgonetas:"), 4, 0);
    obraZonaLayout->addWidget(spFurgonetas, 4, 1);
    obraZonaLayout->addWidget(new QLabel("Coste Furgoneta:"), 4, 2);
    obraZonaLayout->addWidget(sbCosteFurgo, 4, 3);
    obraZonaLayout->addWidget(new QLabel("KM desplazamiento:"), 4, 4);
    obraZonaLayout->addWidget(sbKM, 4, 5);
    obraZonaLayout->addWidget(new QLabel("Combustible:"), 4, 6);
    obraZonaLayout->addWidget(sbLitros, 4, 7);

    // --- FILA 6: Horas y Operarios ---
    obraZonaLayout->addWidget(new QLabel("Nº Operarios:"), 5, 0);
    obraZonaLayout->addWidget(spOperarios, 5, 1);
    obraZonaLayout->addWidget(new QLabel("Días"), 5, 2);
    obraZonaLayout->addWidget(spDias, 5, 3);
    obraZonaLayout->addWidget(new QLabel("Horas Obra:"), 5, 4);
    obraZonaLayout->addWidget(sbHoras, 5, 5);
    obraZonaLayout->addWidget(new QLabel("Horas Viaje:"), 5, 6);
    obraZonaLayout->addWidget(sbHorasViaje, 5, 7);

    obraZonaLayout->setColumnStretch(1, 1);
    obraZonaLayout->setColumnStretch(3, 1);
    obraZonaLayout->setColumnStretch(5, 1);
    obraZonaLayout->setContentsMargins(10, 10, 10, 10);
    obraZonaLayout->setHorizontalSpacing(12);
    obraZonaLayout->setVerticalSpacing(8);

    formLayout->addRow(obraZonaGroup);

    // ---------------------- Bloque Materiales ----------------------
    auto *materialsGroup = new QGroupBox("Materiales");
    auto *materialsLayout = new QVBoxLayout(materialsGroup);

    twMaterials = new QTableWidget(0, 5);
    twMaterials->setHorizontalHeaderLabels({"Nombre", "Cantidad", "Precio Venta", "Precio Compra", "Total Venta"});
    twMaterials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    materialsLayout->addWidget(twMaterials);

    cbMaterials = new QComboBox;
    cbMaterials->addItem("Abrir Gestor de Materiales...");

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
    formLayout->addRow(materialsGroup);

    // ---------------------- Conexiones del formulario ----------------------
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

    connect(cbDietasYes, &QComboBox::currentTextChanged, this, syncGeneralFromDietas);
    connect(spDietas, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        if (cbDietasYes->currentText() == "Si") spOperarios->setValue(val);
    });
    connect(spDiasDieta, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        if (cbDietasYes->currentText() == "Si") spDias->setValue(val);
    });
    syncGeneralFromDietas();

    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::updateDistanceAndFuel);

    connect(btnOpenMaterials, &QPushButton::clicked, this, &MainWindow::onOpenMaterialsWindow);
    connect(cbMaterials, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        if (index == 0) {
            onOpenMaterialsWindow();
        }
    });

    connect(btnAddMat, &QPushButton::clicked, this, [this]() {
        bool ok;
        QString materialName = QInputDialog::getText(this, "Nuevo material manual", "Nombre del material:", QLineEdit::Normal, "", &ok);
        if (!ok || materialName.isEmpty()) return;

        double sellPrice = QInputDialog::getDouble(this, "Nuevo material manual", "Precio de VENTA al cliente:", 0.0, 0, 1e6, 2, &ok);
        if (!ok) return;
        
        double costPrice = QInputDialog::getDouble(this, "Nuevo material manual", "Precio de COMPRA (costo real):", sellPrice * 0.7, 0, 1e6, 2, &ok);
        if (!ok) return;

        if (!materialsManager) {
            materialsManager = new MaterialsWindow(this);
        }

        materialsManager->addMaterialToMasterList("OTROS", materialName, sellPrice, costPrice);

        int row = twMaterials->rowCount();
        twMaterials->insertRow(row);
        twMaterials->setItem(row, 0, new QTableWidgetItem(materialName));
        twMaterials->setItem(row, 1, new QTableWidgetItem("1"));
        twMaterials->setItem(row, 2, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
        twMaterials->setItem(row, 3, new QTableWidgetItem(QString::number(costPrice, 'f', 2)));
        twMaterials->setItem(row, 4, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));

        onCalculate();
    });

    connect(twMaterials, &QTableWidget::cellChanged, this, [this](int row, int column) {
        if (column == 1) {
            updateMaterialTotal(row);
        }
    });

    twMaterials->setSelectionBehavior(QAbstractItemView::SelectRows);
    twMaterials->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(btnRemoveMat, &QPushButton::clicked, this, [this]() {
        auto selectedRows = twMaterials->selectionModel()->selectedRows();
        std::sort(selectedRows.begin(), selectedRows.end(), [](const QModelIndex &a, const QModelIndex &b){
            return a.row() > b.row();
        });

        for (const auto &index : selectedRows) {
            twMaterials->removeRow(index.row());
        }
        onCalculate();
    });

    // ---------------------- Botones de acción ----------------------
    auto *buttonsGroup = new QGroupBox("Acciones");
    auto *buttonsGroupLayout = new QVBoxLayout(buttonsGroup);
    
    auto *colLeft = new QVBoxLayout;
    auto *colRight = new QVBoxLayout;
    
    btnCalc = new QPushButton("Calcular Desviación");
    btnSave = new QPushButton("Guardar presupuesto");
    btnPDF = new QPushButton("Guardar en PDF");
    btnPrint = new QPushButton("Imprimir presupuesto");
    btnStart = new QPushButton("Mantenimientos");
    //QPushButton *btnEditPrices = new QPushButton("Editar precios base");
    QPushButton *btnEditPrices = new QPushButton("Mostrar lista de materiales");

    colLeft->addWidget(btnEditPrices);
    colLeft->addWidget(btnPDF);
    colLeft->addWidget(btnCalc);

    colRight->addWidget(btnSave);
    colRight->addWidget(btnPrint);
    colRight->addWidget(btnStart);

    auto *buttonsGrid = new QHBoxLayout;
    buttonsGrid->addLayout(colLeft);
    buttonsGrid->addSpacing(20);
    buttonsGrid->addLayout(colRight);
    buttonsGroupLayout->addLayout(buttonsGrid);
    buttonsGroupLayout->setContentsMargins(10, 10, 10, 10);

    // ---------------------- Totales ----------------------
    auto *totalsGroup = new QGroupBox("Resumen de presupuesto");
    //auto *totalsLayout = new QGridLayout(totalsGroup);
    auto *totalsHLayout = new QHBoxLayout(totalsGroup);

    // ---------------------- Columna Izquierda de Totales (Controles) ----------------------
    auto *totalsLeft = new QVBoxLayout;

    // Botón Mostrar IVA
    QPushButton *btnToggleIVA = new QPushButton("Mostrar IVA (NO)");
    btnToggleIVA->setCheckable(true);
    connect(btnToggleIVA, &QPushButton::toggled, this, &MainWindow::onToggleIVA);
    
    // Botón Imprevistos
    QPushButton *btnToggleImprevistos = new QPushButton("Imprevistos (10% - NO)");
    btnToggleImprevistos->setCheckable(true);
    connect(btnToggleImprevistos, &QPushButton::toggled, this, &MainWindow::onToggleImprevistos);
    
    totalsLeft->addWidget(btnToggleIVA);
    totalsLeft->addWidget(btnToggleImprevistos);
    totalsLeft->addStretch();

    // ---------------------- Columna Derecha de Totales (Valores) ----------------------
    auto *totalsRight = new QGridLayout;
    
    lblTotalNoIVA = new QLabel("0.00 €");
    lblTotalConIVA = new QLabel("0.00 €");
    lblCostoEstimado = new QLabel("0.00 €");   
    lblBeneficioEstimado = new QLabel("0.00 €");

    // Fila 0: Total sin IVA (Siempre visible)
    totalsRight->addWidget(new QLabel("Total sin IVA:"), 0, 0, Qt::AlignRight);
    totalsRight->addWidget(lblTotalNoIVA, 0, 1, Qt::AlignLeft);
    
    // Fila 1: Total con IVA (Inicialmente oculta)
    // Usamos un QWidget para envolver la fila de IVA para poder ocultarla fácilmente
    ivaRowWidget = new QWidget;
    auto *ivaRowLayout = new QGridLayout(ivaRowWidget);
    lblIVAPct = new QLabel("(21%)"); // Etiqueta para el porcentaje de IVA
    ivaRowLayout->setContentsMargins(0,0,0,0);
    ivaRowLayout->setHorizontalSpacing(12);
    ivaRowLayout->addWidget(new QLabel("Total con IVA:"), 0, 0, Qt::AlignRight);
    ivaRowLayout->addWidget(lblTotalConIVA, 0, 2, Qt::AlignLeft);
    ivaRowLayout->addWidget(lblIVAPct, 0, 1, Qt::AlignLeft);
    
    totalsRight->addWidget(ivaRowWidget, 1, 0, 1, 2);
    ivaRowWidget->setVisible(isIVAShown); // Inicialmente 'false'

    // Fila 2: Costo estimado
    totalsRight->addWidget(new QLabel("Costo estimado:"), 2, 0, Qt::AlignRight);     
    totalsRight->addWidget(lblCostoEstimado, 2, 1, Qt::AlignLeft);  
    // Fila 3: Beneficio estimado
    totalsRight->addWidget(new QLabel("Beneficio estimado:"), 3, 0, Qt::AlignRight);          
    totalsRight->addWidget(lblBeneficioEstimado, 3, 1, Qt::AlignLeft);

    lblDesviacionPVP = new QLabel("N/A €");
    totalsRight->addWidget(new QLabel("Desviación PVP:"), 4, 0, Qt::AlignRight);     
    totalsRight->addWidget(lblDesviacionPVP, 4, 1, Qt::AlignLeft);

    totalsRight->setContentsMargins(10, 10, 10, 10);
    
    // Ensamblar GroupBox Totales
    totalsHLayout->addLayout(totalsLeft);
    totalsHLayout->addSpacing(20);
    totalsHLayout->addLayout(totalsRight);

    // --- Ensamblar columna izquierda ---
    leftV->addLayout(formLayout);
    leftV->addWidget(buttonsGroup);
    leftV->addWidget(totalsGroup);
    leftV->addStretch();
    leftWidget->setLayout(leftV);

    // ==================== COLUMNA DERECHA ====================
    auto *rightWidget = new QWidget;
    auto *rightV = new QVBoxLayout(rightWidget);

    lwBudgets = new QListWidget;
    QPushButton *btnLoad = new QPushButton("     Abrir     ");
    QPushButton *btnDelete = new QPushButton("   Eliminar   ");
    QPushButton *btnClearFields = new QPushButton("  Borrar campos  ");
    QPushButton *btnCloseProject = new QPushButton("  Cerrar obra  ");
    QPushButton *btnPrice = new QPushButton("Editar precios base");
    btnCloseProject->setObjectName("btnCloseProject");

    // --- Buscador ---
    auto *searchLayout = new QHBoxLayout;
    QLineEdit *leSearchClient = new QLineEdit;
    leSearchClient->setPlaceholderText("Buscar cliente...");
    QPushButton *btnClearSearch = new QPushButton("✕");
    btnClearSearch->setToolTip("Limpiar búsqueda");
    btnClearSearch->setFixedSize(24, 24);

    connect(leSearchClient, &QLineEdit::textChanged, this, [this](const QString &text) {
        for (int i = 0; i < lwBudgets->count(); ++i) {
            QListWidgetItem *item = lwBudgets->item(i);
            item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
        }
    });

    connect(btnClearSearch, &QPushButton::clicked, leSearchClient, &QLineEdit::clear);
    
    searchLayout->addWidget(leSearchClient);
    searchLayout->addWidget(btnClearSearch);

    // --- Layouts de botones ---
    auto *btnsLayout1 = new QHBoxLayout;
    btnsLayout1->addWidget(btnLoad);
    btnsLayout1->addWidget(btnDelete);

    auto *btnsLayout2 = new QHBoxLayout;
    btnsLayout2->addWidget(btnClearFields);
    btnsLayout2->addWidget(btnCloseProject);

    auto *btnsLayout3 = new QHBoxLayout;
    btnsLayout3->addWidget(btnPrice);

    // --- Conexiones columna derecha ---
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadSelectedBudget);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteSelectedBudget);
    connect(btnClearFields, &QPushButton::clicked, this, &MainWindow::onDeleteFields);
    //connect(btnCloseProject, &QPushButton::clicked, this, &MainWindow::onToggleStatus);
    connect(btnCloseProject, &QPushButton::clicked, this, &MainWindow::onCloseProject);
    connect(btnPrice, &QPushButton::clicked, this, &MainWindow::onEditBasePrices);

    // --- Ensamblar columna derecha ---
    rightV->addWidget(new QLabel("Presupuestos guardados"));
    rightV->addLayout(searchLayout);
    rightV->addWidget(lwBudgets);
    rightV->addLayout(btnsLayout1);
    rightV->addLayout(btnsLayout2);
    rightV->addLayout(btnsLayout3);
    rightV->addStretch();
    rightWidget->setLayout(rightV);

    // ==================== CONFIGURAR SPLITTER ====================
    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);
    //mainSplitter->setSizes({900, 300});

    // ==================== LAYOUT FINAL ====================
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->addWidget(mainSplitter);
    central->setLayout(mainLayout);

    setCentralWidget(central);
    setWindowTitle("Australair - Gestor de presupuestos");
    resize(1400, 800);

    // ====================================================
    // SOLUCIÓN ESCALADO PROPORCIONAL Y TÁCTIL (REFINADO)
    
    central->setStyleSheet(R"(
        QSpinBox, QDoubleSpinBox, QComboBox {
            min-height: 10px; 
            padding: 2px; 
        }Espera, ¡hay una confusión en tu requisito!

    Un presupuesto nuevo se guarda con status = "abierta".

    Solo los presupuestos con status = "cerrada" deben estar bloqueados.

    Si quieres que el presupuesto recién guardado se quede bloqueado, debes guardarlo con status = "cerrada".

        Si lo guardas como "abierta", debe permanecer desbloqueado para edición, hasta que pulses "Cerrar obra".

Aclaración y Solución de Bloqueo: Si el requisito es que, tras guardar un presupuesto nuevo, la interfaz se bloquee para evitar cambios accidentales, haz esto:


        
        QLineEdit {
            min-height: 10px;
            padding: 2px;
        }
    )");

    //  FIN DE LA SOLUCIÓN TÁCTIL
    // ====================================================

    // ==================== CARGAR DATOS ====================
    refreshBudgetsList();

    // ==================== CONEXIONES FINALES ====================
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculateDesviation);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveBudget);
    connect(btnEditPrices, &QPushButton::clicked, this, &MainWindow::onShowMaterialsList);
    connect(btnPDF, &QPushButton::clicked, this, &MainWindow::onExportPDF);
    connect(btnPrint, &QPushButton::clicked, this, &MainWindow::onPrintBudget);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onBackToStart);

    // ==================== CONEXIONES DE RECALCULO AUTOMÁTICO ====================
    
    // CAMPOS DE OBRA
    connect(sbMetros, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(cbTipoLocal, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(cbTipoCubierta, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    
    // DESPLAZAMIENTO Y COMBUSTIBLE
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbLitros, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);

    // TIEMPOS
    connect(sbHoras, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbHorasViaje, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate); // ¡Crucial!
    connect(spDias, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spOperarios, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    
    // EXTRAS (EXTRACTOR, FURGONETAS)
    connect(cbExtractor, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(spFurgonetas, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbCosteFurgo, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);

    // EXTRAS (ELEVACIÓN)
    connect(cbElevador, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(sbElevPortes, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spElevDia, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spElevPrecDia, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    
    // EXTRAS (ZONA, DISTANCIA Y DIETAS)
    connect(cbZona, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(rbCorta, &QRadioButton::toggled, this, &MainWindow::onCalculate);
    connect(rbMedia, &QRadioButton::toggled, this, &MainWindow::onCalculate);
    connect(rbLarga, &QRadioButton::toggled, this, &MainWindow::onCalculate);
    connect(cbDietasYes, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(spDietas, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spDiasDieta, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbPrecioDiet, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    
    // TABLA DE MATERIALES (Añadir conexión para Precio Venta/Compra)
    // Actualmente solo tienes la cantidad (columna 1). Debes añadir Precio Venta y Precio Compra.
    connect(twMaterials, &QTableWidget::cellChanged, this, [this](int row, int column) {
        // Columna 1=Cantidad, 2=Precio Venta, 3=Precio Compra
        if (column == 1 || column == 2 || column == 3) { 
            updateMaterialTotal(row); // Esta función debe llamar internamente a onCalculate
        }
    });

    // ====================================================
}

// APP
/* void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    
    // ==================== LAYOUT PRINCIPAL CON SPLITTER ====================
    auto *mainSplitter = new QSplitter(Qt::Horizontal, central);
    
    // ==================== INICIALIZAR TODAS LAS LABELS PRIMERO ====================
    // (Para que estén disponibles desde cualquier pantalla)
    lblTotalNoIVA = new QLabel("0.00 €");
    lblTotalConIVA = new QLabel("0.00 €");
    lblCostoEstimado = new QLabel("0.00 €");   
    lblBeneficioEstimado = new QLabel("0.00 €");
    lblDesviacionPVP = new QLabel("N/A €");
    lblIVAPct = new QLabel("(21%)");
    ivaRowWidget = new QWidget; // También crear el widget del IVA aquí
    
    // ==================== COLUMNA IZQUIERDA (STACKED WIDGET) ====================
    auto *leftWidget = new QWidget;
    auto *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    stackedWidget = new QStackedWidget(this);
    
    // ---------------------- PANTALLA 1: DATOS DEL CLIENTE ----------------------
    auto *clientPage = new QWidget;
    auto *clientLayout = new QVBoxLayout(clientPage);
    
    auto *clientGroup = new QGroupBox("Datos del cliente");
    auto *clientForm = new QGridLayout(clientGroup);
    
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
    
    // Configurar tamaños para tablet
    QList<QLineEdit*> lineEdits = {leClientName, leCompany, leContact, leAddress, 
                                  lePhone, leEmail, leCIF, leNumPresu, leFecha};
    for (auto *le : lineEdits) {
        le->setMinimumHeight(45);
    }
    
    // Layout cliente
    clientForm->addWidget(new QLabel("Cliente:"), 0, 0);
    clientForm->addWidget(leClientName, 0, 1);
    clientForm->addWidget(new QLabel("Empresa:"), 0, 2);
    clientForm->addWidget(leCompany, 0, 3);
    
    clientForm->addWidget(new QLabel("Contacto:"), 1, 0);
    clientForm->addWidget(leContact, 1, 1);
    clientForm->addWidget(new QLabel("Teléfono:"), 1, 2);
    clientForm->addWidget(lePhone, 1, 3);
    
    clientForm->addWidget(new QLabel("Email:"), 2, 0);
    clientForm->addWidget(leEmail, 2, 1);
    clientForm->addWidget(new QLabel("Fecha:"), 2, 2);
    clientForm->addWidget(leFecha, 2, 3);
    
    clientForm->addWidget(new QLabel("Dirección:"), 3, 0);
    clientForm->addWidget(leAddress, 3, 1, 1, 3);
    
    clientForm->addWidget(new QLabel("CIF:"), 4, 0);
    clientForm->addWidget(leCIF, 4, 1);
    clientForm->addWidget(new QLabel("Nº Presupuesto:"), 4, 2);
    clientForm->addWidget(leNumPresu, 4, 3);
    
    for (int i = 0; i < 4; ++i) {
        clientForm->setColumnStretch(i, 1);
    }
    
    clientLayout->addWidget(clientGroup);
    clientLayout->addStretch();
    
    // Botón siguiente
    auto *btnNext1 = new QPushButton("Siguiente → Datos de la Obra");
    btnNext1->setMinimumHeight(50);
    clientLayout->addWidget(btnNext1);
    
    // ---------------------- PANTALLA 2: DATOS DE LA OBRA (CON SCROLL) ----------------------
    auto *workPage = new QWidget;
    auto *workMainLayout = new QVBoxLayout(workPage);
    
    // Crear scroll area
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    auto *scrollWidget = new QWidget;
    auto *workLayout = new QVBoxLayout(scrollWidget);
    
    auto *workGroup = new QGroupBox("Datos de la obra");
    auto *workForm = new QGridLayout(workGroup);
    
    // Campos de obra
    sbMetros = new QDoubleSpinBox; sbMetros->setRange(0, 1e6); sbMetros->setSuffix(" m²");
    cbTipoLocal = new QComboBox; cbTipoLocal->addItems({" ··· ","Nave industrial","Local comercial","Vivienda","Complejo Deportivo"});
    cbTipoCubierta = new QComboBox; cbTipoCubierta->addItems({" ··· ","Chapa","Teja","Cubierta plana","Panel sandwich", "Uralita"});
    sbKM = new QDoubleSpinBox; sbKM->setRange(0,10000); sbKM->setSuffix(" km");
    sbLitros = new QDoubleSpinBox; sbLitros->setRange(0,10000); sbLitros->setSuffix(" lts");
    sbHoras = new QDoubleSpinBox; sbHoras->setRange(0,1e5);
    spDias = new QSpinBox; spDias->setRange(0,365);
    spOperarios = new QSpinBox; spOperarios->setRange(0,60);
    cbExtractor = new QComboBox; cbExtractor->addItems({"No", "Si"});
    sbHorasViaje = new QDoubleSpinBox; sbHorasViaje->setRange(0,1e5);
    spFurgonetas = new QSpinBox; spFurgonetas->setRange(0,100);
    
    sbCosteFurgo = new QDoubleSpinBox; sbCosteFurgo->setRange(0, 1e6); sbCosteFurgo->setSuffix(" €");
    sbCosteFurgo->setValue(getSettingDouble("cost_van_day", 95.0));
    sbCosteFurgo->setEnabled(false);
    sbPrecioDiet = new QDoubleSpinBox; sbPrecioDiet->setRange(0,1e6); sbPrecioDiet->setSuffix(" €");
    sbPrecioDiet->setValue(getSettingDouble("dieta_price", 150.0));
    sbPrecioDiet->setEnabled(false);

    cbElevador = new QComboBox; cbElevador->addItems({"No","Si"});
    sbElevPortes = new QDoubleSpinBox; sbElevPortes->setRange(0, 1e6); sbElevPortes->setSuffix(" €");
    spElevDia = new QSpinBox; spElevDia->setRange(0,1000);
    spElevPrecDia = new QSpinBox; spElevPrecDia->setRange(0, 3000);
    sbElevPortes->setEnabled(false);
    spElevDia->setEnabled(false);
    spElevPrecDia->setEnabled(false);

    cbZona = new QComboBox; 
    cbZona->addItems({"Zona Centro","Otras Zonas"});
    cbDietasYes = new QComboBox; cbDietasYes->addItems({"No","Si"});
    rbCorta = new QRadioButton("Corta");
    rbMedia  = new QRadioButton("Media");
    rbLarga  = new QRadioButton("Larga");
    spDietas = new QSpinBox; spDietas->setRange(0,1000);
    spDiasDieta = new QSpinBox; spDiasDieta->setRange(0,365);
    spDietas->setEnabled(false);
    spDiasDieta->setEnabled(false);
    cbDietasYes->setEnabled(false);
    
    // Configurar controles
    QList<QComboBox*> combos = {cbTipoLocal, cbTipoCubierta, cbExtractor, cbElevador, cbZona, cbDietasYes};
    QList<QDoubleSpinBox*> dSpinBoxes = {sbMetros, sbKM, sbLitros, sbHoras, sbHorasViaje, sbCosteFurgo, sbPrecioDiet, sbElevPortes};
    QList<QSpinBox*> spinBoxes = {spDias, spOperarios, spFurgonetas, spElevDia, spElevPrecDia, spDietas, spDiasDieta};
    
    for (auto *combo : combos) combo->setMinimumHeight(45);
    for (auto *spin : dSpinBoxes) spin->setMinimumHeight(45);
    for (auto *spin : spinBoxes) spin->setMinimumHeight(45);
    
    // Layout obra
    int row = 0;
    workForm->addWidget(new QLabel("Tipo local:"), row, 0);
    workForm->addWidget(cbTipoLocal, row, 1);
    workForm->addWidget(new QLabel("m²:"), row, 2);
    workForm->addWidget(sbMetros, row, 3); row++;
    
    workForm->addWidget(new QLabel("Cubierta:"), row, 0);
    workForm->addWidget(cbTipoCubierta, row, 1);
    workForm->addWidget(new QLabel("Extracción:"), row, 2);
    workForm->addWidget(cbExtractor, row, 3); row++;
    
    workForm->addWidget(new QLabel("Elevación:"), row, 0);
    workForm->addWidget(cbElevador, row, 1);
    workForm->addWidget(new QLabel("Portes:"), row, 2);
    workForm->addWidget(sbElevPortes, row, 3); row++;
    
    workForm->addWidget(new QLabel("Días elev.:"), row, 0);
    workForm->addWidget(spElevDia, row, 1);
    workForm->addWidget(new QLabel("Precio/día:"), row, 2);
    workForm->addWidget(spElevPrecDia, row, 3); row++;
    
    workForm->addWidget(new QLabel("Zona:"), row, 0);
    workForm->addWidget(cbZona, row, 1);
    workForm->addWidget(new QLabel("Distancia:"), row, 2);
    auto *distanceLayout = new QHBoxLayout;
    distanceLayout->addWidget(rbCorta);
    distanceLayout->addWidget(rbMedia);
    distanceLayout->addWidget(rbLarga);
    auto *distanceWidget = new QWidget;
    distanceWidget->setLayout(distanceLayout);
    workForm->addWidget(distanceWidget, row, 3); row++;
    
    workForm->addWidget(new QLabel("Dietas:"), row, 0);
    workForm->addWidget(cbDietasYes, row, 1);
    workForm->addWidget(new QLabel("Operarios:"), row, 2);
    workForm->addWidget(spDietas, row, 3); row++;
    
    workForm->addWidget(new QLabel("Días dieta:"), row, 0);
    workForm->addWidget(spDiasDieta, row, 1);
    workForm->addWidget(new QLabel("Precio/día:"), row, 2);
    workForm->addWidget(sbPrecioDiet, row, 3); row++;
    
    workForm->addWidget(new QLabel("Furgonetas:"), row, 0);
    workForm->addWidget(spFurgonetas, row, 1);
    workForm->addWidget(new QLabel("Coste/furgo:"), row, 2);
    workForm->addWidget(sbCosteFurgo, row, 3); row++;
    
    workForm->addWidget(new QLabel("KM:"), row, 0);
    workForm->addWidget(sbKM, row, 1);
    workForm->addWidget(new QLabel("Combustible:"), row, 2);
    workForm->addWidget(sbLitros, row, 3); row++;
    
    workForm->addWidget(new QLabel("Operarios:"), row, 0);
    workForm->addWidget(spOperarios, row, 1);
    workForm->addWidget(new QLabel("Días:"), row, 2);
    workForm->addWidget(spDias, row, 3); row++;
    
    workForm->addWidget(new QLabel("Horas obra:"), row, 0);
    workForm->addWidget(sbHoras, row, 1);
    workForm->addWidget(new QLabel("Horas viaje:"), row, 2);
    workForm->addWidget(sbHorasViaje, row, 3);
    
    for (int i = 0; i < 4; ++i) {
        workForm->setColumnStretch(i, 1);
    }
    
    workLayout->addWidget(workGroup);
    workLayout->addStretch();
    
    scrollArea->setWidget(scrollWidget);
    workMainLayout->addWidget(scrollArea);
    
    // Botones navegación obra (fuera del scroll)
    auto *workButtonsLayout = new QHBoxLayout;
    auto *btnBack2 = new QPushButton("← Atrás");
    auto *btnNext2 = new QPushButton("Siguiente → Materiales y Acciones");
    btnBack2->setMinimumHeight(50);
    btnNext2->setMinimumHeight(50);
    workButtonsLayout->addWidget(btnBack2);
    workButtonsLayout->addWidget(btnNext2);
    workMainLayout->addLayout(workButtonsLayout);
    
    // ---------------------- PANTALLA 3: MATERIALES Y ACCIONES (UNIFICADA) ----------------------
    auto *materialsActionsPage = new QWidget;
    auto *materialsActionsLayout = new QVBoxLayout(materialsActionsPage);

    // --- Sección Materiales ---
    auto *materialsGroup = new QGroupBox("Materiales");
    auto *materialsForm = new QVBoxLayout(materialsGroup);

    twMaterials = new QTableWidget(0, 5);
    twMaterials->setHorizontalHeaderLabels({"Nombre", "Cantidad", "Precio Venta", "Precio Compra", "Total Venta"});
    twMaterials->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    twMaterials->verticalHeader()->setDefaultSectionSize(45);

    materialsForm->addWidget(twMaterials);

    // Botones materiales
    auto *materialsButtons = new QHBoxLayout;
    btnAddMat = new QPushButton("Añadir Material");
    btnRemoveMat = new QPushButton("Eliminar Material");
    auto *btnOpenMaterials = new QPushButton("Gestor de Materiales");

    btnAddMat->setMinimumHeight(45);
    btnRemoveMat->setMinimumHeight(45);
    btnOpenMaterials->setMinimumHeight(45);

    materialsButtons->addWidget(btnOpenMaterials);
    materialsButtons->addWidget(btnAddMat);
    materialsButtons->addWidget(btnRemoveMat);
    materialsForm->addLayout(materialsButtons);

    materialsActionsLayout->addWidget(materialsGroup);

    // --- Sección Acciones ---
    auto *actionsGroup = new QGroupBox("Acciones");
    auto *actionsForm = new QGridLayout(actionsGroup);

    // Botones de acción
    btnCalc = new QPushButton("Calcular Desviación");
    btnSave = new QPushButton("Guardar Presupuesto");
    btnPDF = new QPushButton("Guardar en PDF");
    btnPrint = new QPushButton("Imprimir Presupuesto");
    btnStart = new QPushButton("Mantenimientos");
    auto *btnEditPrices = new QPushButton("Lista de Materiales");

    QList<QPushButton*> actionButtons = {btnCalc, btnSave, btnPDF, btnPrint, btnStart, btnEditPrices};
    for (auto *btn : actionButtons) {
        btn->setMinimumHeight(60);
    }

    actionsForm->addWidget(btnEditPrices, 0, 0);
    actionsForm->addWidget(btnPDF, 0, 1);
    actionsForm->addWidget(btnCalc, 0, 2);
    actionsForm->addWidget(btnSave, 1, 0);
    actionsForm->addWidget(btnPrint, 1, 1);
    actionsForm->addWidget(btnStart, 1, 2);

    materialsActionsLayout->addWidget(actionsGroup);
    materialsActionsLayout->addStretch();

    // Botones navegación materiales y acciones
    auto *materialsActionsNavLayout = new QHBoxLayout;
    auto *btnBack3 = new QPushButton("← Atrás");
    auto *btnNext3 = new QPushButton("Siguiente → Resumen");
    btnBack3->setMinimumHeight(50);
    btnNext3->setMinimumHeight(50);
    materialsActionsNavLayout->addWidget(btnBack3);
    materialsActionsNavLayout->addWidget(btnNext3);
    materialsActionsLayout->addLayout(materialsActionsNavLayout);
    
    // ---------------------- PANTALLA 4: RESUMEN ----------------------
    auto *summaryPage = new QWidget;
    auto *summaryLayout = new QVBoxLayout(summaryPage);
    
    auto *summaryGroup = new QGroupBox("Resumen del Presupuesto");
    auto *summaryForm = new QHBoxLayout(summaryGroup);
    
    // Columna controles
    auto *summaryLeft = new QVBoxLayout;
    QPushButton *btnToggleIVA = new QPushButton("Mostrar IVA (NO)");
    QPushButton *btnToggleImprevistos = new QPushButton("Imprevistos (10% - NO)");
    btnToggleIVA->setCheckable(true);
    btnToggleImprevistos->setCheckable(true);
    btnToggleIVA->setMinimumHeight(45);
    btnToggleImprevistos->setMinimumHeight(45);
    
    summaryLeft->addWidget(btnToggleIVA);
    summaryLeft->addWidget(btnToggleImprevistos);
    summaryLeft->addStretch();
    
    // Columna valores - USAR LAS LABELS QUE YA EXISTEN
    auto *summaryRight = new QGridLayout;
    
    // Configurar fuentes para las labels existentes
    QList<QLabel*> totalLabels = {lblTotalNoIVA, lblTotalConIVA, lblCostoEstimado, lblBeneficioEstimado, lblDesviacionPVP};
    QFont totalFont;
    totalFont.setPointSize(12);
    totalFont.setBold(true);
    for (auto *label : totalLabels) {
        label->setFont(totalFont);
        label->setMinimumHeight(30);
    }
    
    // Usar las labels existentes (no crear nuevas)
    summaryRight->addWidget(new QLabel("Total sin IVA:"), 0, 0, Qt::AlignRight);
    summaryRight->addWidget(lblTotalNoIVA, 0, 1, Qt::AlignLeft);
    
    // Configurar el widget del IVA (usando lblIVAPct que ya existe)
    auto *ivaRowLayout = new QGridLayout(ivaRowWidget);
    ivaRowLayout->setContentsMargins(0,0,0,0);
    ivaRowLayout->setHorizontalSpacing(8);
    ivaRowLayout->addWidget(new QLabel("Total con IVA:"), 0, 0, Qt::AlignRight);
    ivaRowLayout->addWidget(lblTotalConIVA, 0, 2, Qt::AlignLeft);
    ivaRowLayout->addWidget(lblIVAPct, 0, 1, Qt::AlignLeft);
    
    summaryRight->addWidget(ivaRowWidget, 1, 0, 1, 2);
    ivaRowWidget->setVisible(isIVAShown);
    
    summaryRight->addWidget(new QLabel("Costo estimado:"), 2, 0, Qt::AlignRight);     
    summaryRight->addWidget(lblCostoEstimado, 2, 1, Qt::AlignLeft);  
    summaryRight->addWidget(new QLabel("Beneficio estimado:"), 3, 0, Qt::AlignRight);          
    summaryRight->addWidget(lblBeneficioEstimado, 3, 1, Qt::AlignLeft);
    summaryRight->addWidget(new QLabel("Desviación PVP:"), 4, 0, Qt::AlignRight);     
    summaryRight->addWidget(lblDesviacionPVP, 4, 1, Qt::AlignLeft);
    
    summaryForm->addLayout(summaryLeft);
    summaryForm->addSpacing(20);
    summaryForm->addLayout(summaryRight);
    
    summaryLayout->addWidget(summaryGroup);
    summaryLayout->addStretch();
    
    // Botones navegación resumen
    auto *summaryNavLayout = new QHBoxLayout;
    auto *btnBack4 = new QPushButton("← Atrás");
    auto *btnFinish = new QPushButton("Finalizar");
    btnBack4->setMinimumHeight(50);
    btnFinish->setMinimumHeight(50);
    summaryNavLayout->addWidget(btnBack4);
    summaryNavLayout->addWidget(btnFinish);
    summaryLayout->addLayout(summaryNavLayout);
    
    // ==================== AÑADIR PANTALLAS AL STACK ====================
    stackedWidget->addWidget(clientPage);              // Índice 0
    stackedWidget->addWidget(workPage);                // Índice 1  
    stackedWidget->addWidget(materialsActionsPage);    // Índice 2 (unificada)
    stackedWidget->addWidget(summaryPage);             // Índice 3
    
    leftLayout->addWidget(stackedWidget);
    
    // ==================== COLUMNA DERECHA (LISTA DE PRESUPUESTOS) ====================
    auto *rightWidget = new QWidget;
    auto *rightV = new QVBoxLayout(rightWidget);
    rightV->setContentsMargins(5, 5, 5, 5);
    rightV->setSpacing(8);

    // --- Buscador ---
    auto *searchLayout = new QHBoxLayout;
    QLineEdit *leSearchClient = new QLineEdit;
    leSearchClient->setPlaceholderText("Buscar cliente...");
    leSearchClient->setMinimumHeight(35);
    QPushButton *btnClearSearch = new QPushButton("✕");
    btnClearSearch->setToolTip("Limpiar búsqueda");
    btnClearSearch->setFixedSize(35, 35);

    connect(leSearchClient, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (!lwBudgets) return;
        for (int i = 0; i < lwBudgets->count(); ++i) {
            QListWidgetItem *item = lwBudgets->item(i);
            item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
        }
    });

    connect(btnClearSearch, &QPushButton::clicked, leSearchClient, &QLineEdit::clear);
    
    searchLayout->addWidget(leSearchClient);
    searchLayout->addWidget(btnClearSearch);

    // --- Lista de presupuestos ---
    lwBudgets = new QListWidget;
    lwBudgets->setMinimumWidth(250);
    lwBudgets->setAlternatingRowColors(true);

    // --- Botones columna derecha ---
    QPushButton *btnLoad = new QPushButton("Abrir");
    QPushButton *btnDelete = new QPushButton("Eliminar");
    QPushButton *btnClearFields = new QPushButton("Borrar campos");
    QPushButton *btnCloseProject = new QPushButton("Cerrar obra");
    QPushButton *btnPrice = new QPushButton("Editar precios base");
    btnCloseProject->setObjectName("btnCloseProject");

    QList<QPushButton*> rightButtons = {btnLoad, btnDelete, btnClearFields, btnCloseProject, btnPrice};
    for (auto *btn : rightButtons) {
        btn->setMinimumHeight(40);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    // --- Layouts de botones ---
    auto *btnsLayout1 = new QHBoxLayout;
    btnsLayout1->setSpacing(6);
    btnsLayout1->addWidget(btnLoad);
    btnsLayout1->addWidget(btnDelete);

    auto *btnsLayout2 = new QHBoxLayout;
    btnsLayout2->setSpacing(6);
    btnsLayout2->addWidget(btnClearFields);
    btnsLayout2->addWidget(btnCloseProject);

    auto *btnsLayout3 = new QHBoxLayout;
    btnsLayout3->addWidget(btnPrice);

    // --- Ensamblar columna derecha ---
    rightV->addWidget(new QLabel("Presupuestos guardados"));
    rightV->addLayout(searchLayout);
    rightV->addWidget(lwBudgets);
    rightV->addLayout(btnsLayout1);
    rightV->addLayout(btnsLayout2);
    rightV->addLayout(btnsLayout3);
    rightV->addStretch(1);

    // ==================== CONFIGURAR SPLITTER ====================
    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setStretchFactor(0, 3);  // Izquierda 75%
    mainSplitter->setStretchFactor(1, 1);  // Derecha 25%
    mainSplitter->setHandleWidth(8);

    // ==================== LAYOUT FINAL ====================
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mainSplitter);

    setCentralWidget(central);
    setWindowTitle("Australair - Gestor de presupuestos");
    resize(1200, 800);

    // ==================== CONEXIONES DE NAVEGACIÓN ====================
    connect(btnNext1, &QPushButton::clicked, this, [this]() { stackedWidget->setCurrentIndex(1); }); // Cliente → Obra
    connect(btnNext2, &QPushButton::clicked, this, [this]() { stackedWidget->setCurrentIndex(2); }); // Obra → Materiales y Acciones
    connect(btnNext3, &QPushButton::clicked, this, [this]() { 
        onCalculate(); // Calcular antes de mostrar resumen
        stackedWidget->setCurrentIndex(3); // Materiales y Acciones → Resumen
    });
    
    connect(btnBack2, &QPushButton::clicked, this, [this]() { stackedWidget->setCurrentIndex(0); }); // Obra → Cliente
    connect(btnBack3, &QPushButton::clicked, this, [this]() { stackedWidget->setCurrentIndex(1); }); // Materiales y Acciones → Obra
    connect(btnBack4, &QPushButton::clicked, this, [this]() { stackedWidget->setCurrentIndex(2); }); // Resumen → Materiales y Acciones
    
    connect(btnFinish, &QPushButton::clicked, this, [this]() { 
        stackedWidget->setCurrentIndex(0); // Volver al inicio
    });

    // ==================== CONEXIONES DE FUNCIONALIDAD ====================
    
    // Conexiones de elevador
    connect(cbElevador, &QComboBox::currentTextChanged, this, [=](const QString &elev){
        bool enabled = (elev == "Si");
        if (sbElevPortes) sbElevPortes->setEnabled(enabled);
        if (spElevDia) spElevDia->setEnabled(enabled);
        if (spElevPrecDia) spElevPrecDia->setEnabled(enabled);
    });

    // Conexiones de zona y dietas
    connect(cbZona, &QComboBox::currentTextChanged, this, [=](const QString &zona){
        if (zona == "Zona Centro") {
            if (rbCorta) rbCorta->setEnabled(true);
            if (rbMedia) rbMedia->setEnabled(true);
            if (rbLarga) rbLarga->setEnabled(true);
            if (cbDietasYes) {
                cbDietasYes->setCurrentText("No");
                cbDietasYes->setEnabled(false);
            }
            if (spDietas) spDietas->setEnabled(false);
            if (spDiasDieta) spDiasDieta->setEnabled(false);
        } else if (zona == "Otras Zonas") {
            if (rbCorta) rbCorta->setEnabled(false);
            if (rbMedia) rbMedia->setEnabled(false);
            if (rbLarga) rbLarga->setEnabled(false);
            if (cbDietasYes) {
                cbDietasYes->setEnabled(true);
                cbDietasYes->setCurrentText("Si");
            }
            if (spDietas) spDietas->setEnabled(true);
            if (spDiasDieta) spDiasDieta->setEnabled(true);
        }
    });

    // Sincronización dietas
    auto syncGeneralFromDietas = [=]() {
        if (cbDietasYes && cbDietasYes->currentText() == "Si") {
            if (spOperarios && spDietas) spOperarios->setValue(spDietas->value());
            if (spDias && spDiasDieta) spDias->setValue(spDiasDieta->value());
            if (spOperarios) spOperarios->setEnabled(false);
            if (spDias) spDias->setEnabled(false);
        } else {
            if (spOperarios) spOperarios->setEnabled(true);
            if (spDias) spDias->setEnabled(true);
        }
    };

    connect(cbDietasYes, &QComboBox::currentTextChanged, this, syncGeneralFromDietas);
    connect(spDietas, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        if (cbDietasYes && cbDietasYes->currentText() == "Si" && spOperarios) spOperarios->setValue(val);
    });
    connect(spDiasDieta, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val){
        if (cbDietasYes && cbDietasYes->currentText() == "Si" && spDias) spDias->setValue(val);
    });
    syncGeneralFromDietas();

    // Combustible y distancia
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::updateDistanceAndFuel);

    // Materiales
    connect(btnOpenMaterials, &QPushButton::clicked, this, &MainWindow::onOpenMaterialsWindow);
    connect(btnAddMat, &QPushButton::clicked, this, [this]() {
        bool ok;
        QString materialName = QInputDialog::getText(this, "Nuevo material manual", "Nombre del material:", QLineEdit::Normal, "", &ok);
        if (!ok || materialName.isEmpty()) return;

        double sellPrice = QInputDialog::getDouble(this, "Nuevo material manual", "Precio de VENTA al cliente:", 0.0, 0, 1e6, 2, &ok);
        if (!ok) return;
        
        double costPrice = QInputDialog::getDouble(this, "Nuevo material manual", "Precio de COMPRA (costo real):", sellPrice * 0.7, 0, 1e6, 2, &ok);
        if (!ok) return;

        if (!materialsManager) {
            materialsManager = new MaterialsWindow(this);
        }

        materialsManager->addMaterialToMasterList("OTROS", materialName, sellPrice, costPrice);

        int row = twMaterials->rowCount();
        twMaterials->insertRow(row);
        twMaterials->setItem(row, 0, new QTableWidgetItem(materialName));
        twMaterials->setItem(row, 1, new QTableWidgetItem("1"));
        twMaterials->setItem(row, 2, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
        twMaterials->setItem(row, 3, new QTableWidgetItem(QString::number(costPrice, 'f', 2)));
        twMaterials->setItem(row, 4, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));

        onCalculate();
    });

    connect(twMaterials, &QTableWidget::cellChanged, this, [this](int row, int column) {
        if (column == 1) {
            updateMaterialTotal(row);
        }
    });

    connect(btnRemoveMat, &QPushButton::clicked, this, [this]() {
        auto selectedRows = twMaterials->selectionModel()->selectedRows();
        std::sort(selectedRows.begin(), selectedRows.end(), [](const QModelIndex &a, const QModelIndex &b){
            return a.row() > b.row();
        });

        for (const auto &index : selectedRows) {
            twMaterials->removeRow(index.row());
        }
        onCalculate();
    });

    // Botones de acción
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::onCalculateDesviation);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveBudget);
    connect(btnEditPrices, &QPushButton::clicked, this, &MainWindow::onShowMaterialsList);
    connect(btnPDF, &QPushButton::clicked, this, &MainWindow::onExportPDF);
    connect(btnPrint, &QPushButton::clicked, this, &MainWindow::onPrintBudget);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onBackToStart);

    // Totales
    connect(btnToggleIVA, &QPushButton::toggled, this, &MainWindow::onToggleIVA);
    connect(btnToggleImprevistos, &QPushButton::toggled, this, &MainWindow::onToggleImprevistos);

    // ==================== CONEXIONES COLUMNA DERECHA ====================
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadSelectedBudget);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteSelectedBudget);
    connect(btnClearFields, &QPushButton::clicked, this, &MainWindow::onDeleteFields);
    connect(btnCloseProject, &QPushButton::clicked, this, &MainWindow::onCloseProject);
    connect(btnPrice, &QPushButton::clicked, this, &MainWindow::onEditBasePrices);

    // ==================== CONEXIONES DE RECÁLCULO AUTOMÁTICO ====================
    
    // CAMPOS DE OBRA
    connect(sbMetros, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(cbTipoLocal, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(cbTipoCubierta, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    
    // DESPLAZAMIENTO Y COMBUSTIBLE
    connect(sbKM, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbLitros, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);

    // TIEMPOS
    connect(sbHoras, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbHorasViaje, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spDias, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spOperarios, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    
    // EXTRAS
    connect(cbExtractor, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(spFurgonetas, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbCosteFurgo, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);

    // ELEVACIÓN
    connect(cbElevador, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(sbElevPortes, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spElevDia, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spElevPrecDia, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    
    // ZONA, DISTANCIA Y DIETAS
    connect(cbZona, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(rbCorta, &QRadioButton::toggled, this, &MainWindow::onCalculate);
    connect(rbMedia, &QRadioButton::toggled, this, &MainWindow::onCalculate);
    connect(rbLarga, &QRadioButton::toggled, this, &MainWindow::onCalculate);
    connect(cbDietasYes, &QComboBox::currentTextChanged, this, &MainWindow::onCalculate);
    connect(spDietas, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(spDiasDieta, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onCalculate);
    connect(sbPrecioDiet, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onCalculate);
    
    // TABLA DE MATERIALES
    connect(twMaterials, &QTableWidget::cellChanged, this, [this](int row, int column) {
        if (column == 1 || column == 2 || column == 3) { 
            updateMaterialTotal(row);
        }
    });

    // Stylesheet simple
    central->setStyleSheet(R"(
        QPushButton { 
            min-height: 40px; 
            font-size: 14px; 
            padding: 10px;
        }
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox { 
            min-height: 40px; 
            font-size: 14px;
            padding: 5px;
        }
        QLabel { 
            font-size: 14px; 
        }
        QGroupBox {
            font-size: 16px;
            font-weight: bold;
        }
        QTableWidget {
            font-size: 13px;
        }
    )");

    // ==================== CARGAR DATOS ====================
    refreshBudgetsList();
    setNextBudgetNumberAndDate();
} */


void MainWindow::onOpenMaterialsWindow()
{
    // Crear la ventana de materiales
    MaterialsWindow *mw = new MaterialsWindow(this);

    if (!materialsManager) {
        materialsManager = new MaterialsWindow(this);
    }
    
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

void MainWindow::onShowMaterialsList()
{
    // Verificar si hay materiales cargados en la tabla principal
    if (twMaterials->rowCount() == 0) {
        QMessageBox::information(this, "Lista de Materiales", "No hay materiales añadidos al presupuesto.");
        return;
    }
    
    // Si hay materiales, abrir el diálogo de edición
    showMaterialsListDialog();
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

void MainWindow::loadSettings(){}

void MainWindow::showMaterialsListDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Editar Materiales del Presupuesto");
    
    auto *mainLayout = new QVBoxLayout(&dialog);
    auto *twEdit = new QTableWidget(0, 4); // Nombre, Cantidad, Precio Venta, Precio Compra

    // Configuración de la tabla del diálogo
    twEdit->setHorizontalHeaderLabels({"Nombre", "Cantidad", "P. Venta (€)", "P. Compra (€)"});
    twEdit->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    twEdit->setSelectionBehavior(QAbstractItemView::SelectRows);
    twEdit->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Deshabilitar la edición de las columnas de Nombre, P. Venta y P. Compra (se modifican al cambiar la cantidad)
    twEdit->setEditTriggers(QAbstractItemView::AllEditTriggers); // Permitir la edición, luego restringimos por columna
    
    // 1. Cargar los materiales de la tabla principal (twMaterials)
    for (int i = 0; i < twMaterials->rowCount(); ++i) {
        twEdit->insertRow(i);
        
        // Columna 0: Nombre (No editable)
        QTableWidgetItem *nameItem = new QTableWidgetItem(twMaterials->item(i, 0)->text());
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        twEdit->setItem(i, 0, nameItem);
        
        // Columna 1: Cantidad (Editable)
        QTableWidgetItem *qtyItem = new QTableWidgetItem(twMaterials->item(i, 1)->text());
        qtyItem->setTextAlignment(Qt::AlignCenter);
        twEdit->setItem(i, 1, qtyItem);

        // Columna 2: Precio Venta (Editable)
        QTableWidgetItem *pvpItem = new QTableWidgetItem(twMaterials->item(i, 2)->text());
        twEdit->setItem(i, 2, pvpItem);

        // Columna 3: Precio Compra (Editable)
        QTableWidgetItem *costItem = new QTableWidgetItem(twMaterials->item(i, 3)->text());
        twEdit->setItem(i, 3, costItem);
    }

    // Conexión para recalcular el total de venta en la tabla principal cuando se edita
    // La columna 1 (Cantidad), Columna 2 (PVP) o Columna 3 (Costo)
    connect(twEdit, &QTableWidget::cellChanged, this, [=](int row, int column){
        // Usamos twEdit aquí, no twMaterials, para obtener los datos modificados.
        if (column == 1 || column == 2 || column == 3) {
            bool okQty, okPvp, okCost;
            // Se asume que los datos numéricos son válidos, si no, se usa 0.0
            int qty = twEdit->item(row, 1)->text().toInt(&okQty);
            double pvp = twEdit->item(row, 2)->text().toDouble(&okPvp);
            double cost = twEdit->item(row, 3)->text().toDouble(&okCost);

            if (!okQty || qty < 0) qty = 0;
            if (!okPvp || pvp < 0.0) pvp = 0.0;
            if (!okCost || cost < 0.0) cost = 0.0;
            
            // Actualizar el total de venta y los datos en la tabla principal (twMaterials)
            // Se desactiva la señal para evitar bucles infinitos con la celda 4 (Total Venta)
            twMaterials->blockSignals(true); 
            twMaterials->item(row, 1)->setText(QString::number(qty));
            twMaterials->item(row, 2)->setText(QString::number(pvp, 'f', 2));
            twMaterials->item(row, 3)->setText(QString::number(cost, 'f', 2));
            twMaterials->item(row, 4)->setText(QString::number(pvp * qty, 'f', 2)); // Total Venta
            twMaterials->blockSignals(false);
            
            // Recalcular el presupuesto
            onCalculate();
        }
    });

    // Botones del diálogo
    auto *btnAccept = new QPushButton("Aceptar y Cerrar");
    auto *btnCancel = new QPushButton("Cancelar");
    
    connect(btnAccept, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    auto *hLayout = new QHBoxLayout;
    hLayout->addWidget(btnAccept);
    hLayout->addWidget(btnCancel);

    mainLayout->addWidget(twEdit);
    mainLayout->addLayout(hLayout);

    dialog.resize(800, 600);
    dialog.exec(); // Mostrar el diálogo modal
}


void MainWindow::onEditBasePrices()
{
    // El antiguo slot ahora simplemente abre el diálogo
    showEditBasePricesDialog();
}

// ---------------------- NUEVAS FUNCIONES DE CONTROL ----------------------

void MainWindow::updateTotalsDisplay()
{
    // Esta función debe ser llamada al final de onCalculate()
    // 1. Manejar la visibilidad del IVA
    ivaRowWidget->setVisible(isIVAShown);
    
    // 2. Aquí podrías añadir lógica si los valores base cambian (lo haremos en onCalculate)
}

void MainWindow::onToggleIVA(bool checked)
{
    isIVAShown = checked;
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        btn->setText(checked ? "Ocultar IVA (SÍ)" : "Mostrar IVA (NO)");
    }
    updateTotalsDisplay();
}

void MainWindow::onToggleImprevistos(bool checked)
{
    isImprevistosApplied = checked;
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        btn->setText(checked ? "Imprevistos (10% - SÍ)" : "Imprevistos (10% - NO)");
    }
    // ¡Es crucial recalcular todo cuando se activa/desactiva el Imprevisto!
    onCalculate();
}

void MainWindow::showEditBasePricesDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Editar Precios Base y Configuraciones");
    
    auto *mainLayout = new QFormLayout(&dialog);

    // --- Precio VENTA de Dieta ---
    auto *sbDietaPrice = new QDoubleSpinBox;
    sbDietaPrice->setRange(0, 1e4); sbDietaPrice->setDecimals(2);
    sbDietaPrice->setValue(getSettingDouble("dieta_price", 150.0)); // Precio VENTA Dieta
    mainLayout->addRow("1. DIETAS -> Día (€):", sbDietaPrice);

    // --- Costo Real de Dieta ---
    auto *sbCostPerDieta = new QDoubleSpinBox;
    sbCostPerDieta->setRange(0, 1e4); sbCostPerDieta->setDecimals(2);
    sbCostPerDieta->setValue(getSettingDouble("cost_per_dieta", 150.0)); 
    mainLayout->addRow("2. DIETAS (coste) -> Día (€):", sbCostPerDieta);
    
    // --- Precio Litro Combustible (Venta) ---
    auto *sbFuelPrice = new QDoubleSpinBox;
    sbFuelPrice->setRange(0, 10); sbFuelPrice->setDecimals(3);
    sbFuelPrice->setValue(getSettingDouble("fuel_liter_price", 1.80)); 
    mainLayout->addRow("3. Precio Combustible (€/Litro):", sbFuelPrice);

    // --- Costo Real de Mano de Obra ---
    auto *sbPriceBase = new QDoubleSpinBox; 
    sbPriceBase->setRange(0, 1e9); sbPriceBase->setDecimals(2);
    sbPriceBase->setValue(getSettingDouble("price_base", 80.0)); // Precio/Hora VENTA
    mainLayout->addRow("4. MANO DE OBRA -> Hora Trabajada (€):", sbPriceBase);

    auto *sbCostPerHour = new QDoubleSpinBox;
    sbCostPerHour->setRange(0, 1e4); sbCostPerHour->setDecimals(2);
    sbCostPerHour->setValue(getSettingDouble("cost_per_hour", 65.0)); 
    mainLayout->addRow("5. MANO DE OBRA (Coste) -> Hora Trabajada (€):", sbCostPerHour);

    // --- Costo Real/Hora de Viaje (Punto 3: Fijo 65.0) ---
    auto *sbCostHourViaje = new QDoubleSpinBox;
    sbCostHourViaje->setRange(0, 1e4); sbCostHourViaje->setDecimals(2);
    sbCostHourViaje->setValue(getSettingDouble("cost_hour_viaje", 80.0)); 
    mainLayout->addRow("6. MANO DE OBRA -> Hora Viaje (€):", sbCostHourViaje);
    
    // --- NUEVO: Costo Real Diario Furgoneta ---
    auto *sbCostVanDay = new QDoubleSpinBox;
    sbCostVanDay->setRange(0, 1e4); sbCostVanDay->setDecimals(2);
    sbCostVanDay->setValue(getSettingDouble("cost_van_day", 95.0)); 
    mainLayout->addRow("7. FURGONETA -> Día (€):", sbCostVanDay);

    // ====================================================
    // IVA
    // ====================================================
    auto *sbIVA = new QDoubleSpinBox; 
    sbIVA->setRange(0, 100); sbIVA->setDecimals(2);
    sbIVA->setValue(getSettingDouble("iva_pct", 21.0));
    mainLayout->addRow("8. IVA Porcentaje (%):", sbIVA);
    
    // --- Botones de acción ---
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        // Guardar los nuevos valores en la base de datos
        QSqlQuery q(Database::instance());
        q.prepare("REPLACE INTO settings(key,value) VALUES(?,?)");
        
        auto saveSetting = [&](const QString &key, double value) {
            q.addBindValue(key);
            q.addBindValue(QString::number(value, 'f', 3));
            q.exec();
        };

        // 1. Venta
        saveSetting("price_base", sbPriceBase->value());
        //saveSetting("increment_per_field", sbIncrement->value());
        saveSetting("dieta_price", sbDietaPrice->value());
        saveSetting("fuel_liter_price", sbFuelPrice->value());
        
        // 2. Costo
        saveSetting("cost_per_hour", sbCostPerHour->value());
        saveSetting("cost_per_dieta", sbCostPerDieta->value());
        saveSetting("cost_van_day", sbCostVanDay->value()); // ¡Nuevo!

        // 3. IVA
        saveSetting("iva_pct", sbIVA->value());

        QMessageBox::information(this, "Ajustes", "Precios base actualizados. Recalculando presupuesto.");
        onCalculate();
    }
}

void MainWindow::toggleInputFields(bool enabled)
{
    // Bloque Datos del Cliente
    leClientName->setEnabled(enabled);
    leCompany->setEnabled(enabled);
    leContact->setEnabled(enabled);
    leAddress->setEnabled(enabled);
    lePhone->setEnabled(enabled);
    leEmail->setEnabled(enabled);
    leCIF->setEnabled(enabled);
    leNumPresu->setEnabled(enabled);
    leFecha->setEnabled(enabled);

    // Bloque Datos de la Obra 
    sbMetros->setEnabled(enabled);
    cbTipoLocal->setEnabled(enabled);
    cbTipoCubierta->setEnabled(enabled);
    cbExtractor->setEnabled(enabled);
    
    // Elevación: Si enabled es falso, se deshabilita todo. Si enabled es verdadero,
    // el control lo toma el callback de cbElevador.
    cbElevador->setEnabled(enabled);
    if (!enabled) { 
        sbElevPortes->setEnabled(false);
        spElevDia->setEnabled(false);
        spElevPrecDia->setEnabled(false);
    }
    
    // Zona y Dietas: Similar a elevación.
    cbZona->setEnabled(enabled);
    rbCorta->setEnabled(enabled);
    rbMedia->setEnabled(enabled);
    rbLarga->setEnabled(enabled);

    // Dietas (El spinbox debe permanecer deshabilitado por el precio fijo)
    // Pero el checkbox de 'Si/No' debe controlarse
    cbDietasYes->setEnabled(enabled); 
    if (!enabled) {
        spDietas->setEnabled(false);
        spDiasDieta->setEnabled(false);
    }
    
    // Furgonetas y Combustible
    spFurgonetas->setEnabled(enabled);
    sbKM->setEnabled(enabled);
    sbLitros->setEnabled(enabled);

    // Los campos de precio fijo (Precio Venta Dieta y Coste Furgo) deben 
    // permanecer deshabilitados SIEMPRE, ya que se editan en 'Editar precios base'.
    // Los campos sbPrecioDiet y sbCosteFurgo ya están deshabilitados en setupUi(), 
    // así que no necesitamos tocarlos aquí, a menos que quieras asegurarte de que 
    // NUNCA se habiliten por error.
    
    // Horas y Operarios
    spOperarios->setEnabled(enabled);
    spDias->setEnabled(enabled);
    sbHoras->setEnabled(enabled);
    sbHorasViaje->setEnabled(enabled);

    // Bloque Materiales
    twMaterials->setEnabled(enabled);
    QPushButton *btnOpenMaterials = findChild<QPushButton*>("btnOpenMaterials"); // Asumo que este es el nombre
    if (btnOpenMaterials) btnOpenMaterials->setEnabled(enabled);
    btnAddMat->setEnabled(enabled);
    btnRemoveMat->setEnabled(enabled);
    
    // Botones de acción principales
    QPushButton *btnCalc = findChild<QPushButton*>("btnCalc");
    QPushButton *btnSave = findChild<QPushButton*>("btnSave");
    if (btnCalc) btnCalc->setEnabled(enabled);
    if (btnSave) btnSave->setEnabled(enabled);

    // El botón de Cerrar/Reabrir Obra siempre debe estar habilitado, excepto si no hay presupuesto cargado.
}
