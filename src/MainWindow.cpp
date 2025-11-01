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
    spPrecioDiet = new QSpinBox; spPrecioDiet->setRange(0,1000);
    spFurgonetas = new QSpinBox; spFurgonetas->setRange(0,100);
    sbCosteFurgo = new QDoubleSpinBox; sbCosteFurgo->setRange(0, 1e6); sbCosteFurgo->setSuffix(" €");

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
    obraZonaLayout->addWidget(spPrecioDiet, 3, 7);

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

        int row = twMaterials->rowCount();
        twMaterials->insertRow(row);
        twMaterials->setItem(row, 0, new QTableWidgetItem(materialName));
        twMaterials->setItem(row, 1, new QTableWidgetItem("1"));
        twMaterials->setItem(row, 2, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
        twMaterials->setItem(row, 3, new QTableWidgetItem(QString::number(costPrice, 'f', 2)));
        twMaterials->setItem(row, 4, new QTableWidgetItem(QString::number(sellPrice, 'f', 2)));
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
    
    btnCalc = new QPushButton("Calcular");
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
    
    /* lblTotalNoIVA = new QLabel("0.00 €");
    lblTotalConIVA = new QLabel("0.00 €");
    lblCostoEstimado = new QLabel("0.00 €");   
    lblBeneficioEstimado = new QLabel("0.00 €");

    totalsLayout->addWidget(new QLabel("Total sin IVA:"), 0, 0, Qt::AlignRight);
    totalsLayout->addWidget(lblTotalNoIVA, 0, 1, Qt::AlignLeft);
    totalsLayout->addWidget(new QLabel("Total con IVA (21%):"), 1, 0, Qt::AlignRight);
    totalsLayout->addWidget(lblTotalConIVA, 1, 1, Qt::AlignLeft);
    totalsLayout->addWidget(new QLabel("Costo estimado:"), 2, 0, Qt::AlignRight);     
    totalsLayout->addWidget(lblCostoEstimado, 2, 1, Qt::AlignLeft);  
    totalsLayout->addWidget(new QLabel("Beneficio estimado:"), 3, 0, Qt::AlignRight);          
    totalsLayout->addWidget(lblBeneficioEstimado, 3, 1, Qt::AlignLeft);

    totalsLayout->setContentsMargins(10, 10, 10, 10); */

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
    connect(btnCloseProject, &QPushButton::clicked, this, &MainWindow::onToggleStatus);
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
        }
        
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
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveBudget);
    connect(btnEditPrices, &QPushButton::clicked, this, &MainWindow::onShowMaterialsList);
    connect(btnPDF, &QPushButton::clicked, this, &MainWindow::onExportPDF);
    connect(btnPrint, &QPushButton::clicked, this, &MainWindow::onPrintBudget);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onBackToStart);
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

/* void MainWindow::onEditBasePrices()
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
} */

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
    
    // ====================================================
    // 1. TARIFAS Y CARGOS GENERALES
    // ====================================================
    
    // --- Precios de Mano de Obra (Venta) ---
    auto *sbPriceBase = new QDoubleSpinBox; 
    sbPriceBase->setRange(0, 1e9); sbPriceBase->setDecimals(2);
    sbPriceBase->setValue(getSettingDouble("price_base", 80.0)); // Precio/Hora VENTA
    mainLayout->addRow("1. Precio/Hora Mano Obra VENTA (€):", sbPriceBase);

    auto *sbIncrement = new QDoubleSpinBox; 
    sbIncrement->setRange(0, 1e9); sbIncrement->setDecimals(2);
    sbIncrement->setValue(getSettingDouble("increment_per_field", 5.0));
    mainLayout->addRow("2. Incremento por campo (€):", sbIncrement);

    // --- Precio VENTA de Dieta ---
    auto *sbDietaPrice = new QDoubleSpinBox;
    sbDietaPrice->setRange(0, 1e4); sbDietaPrice->setDecimals(2);
    sbDietaPrice->setValue(getSettingDouble("dieta_price", 150.0)); // Precio VENTA Dieta
    mainLayout->addRow("3. Precio VENTA Dieta/Día (€):", sbDietaPrice);
    
    // --- Precio Litro Combustible (Venta) ---
    auto *sbFuelPrice = new QDoubleSpinBox;
    sbFuelPrice->setRange(0, 10); sbFuelPrice->setDecimals(3);
    sbFuelPrice->setValue(getSettingDouble("fuel_liter_price", 1.80)); 
    mainLayout->addRow("4. Precio Combustible (€/Litro):", sbFuelPrice);
    
    // ====================================================
    // 2. COSTOS REALES ESTIMADOS
    // ====================================================

    // --- Costo Real de Mano de Obra ---
    auto *sbCostPerHour = new QDoubleSpinBox;
    sbCostPerHour->setRange(0, 1e4); sbCostPerHour->setDecimals(2);
    sbCostPerHour->setValue(getSettingDouble("cost_per_hour", 25.0)); 
    mainLayout->addRow("5. Costo Real/Hora (€):", sbCostPerHour);

    // --- Costo Real de Dieta ---
    auto *sbCostPerDieta = new QDoubleSpinBox;
    sbCostPerDieta->setRange(0, 1e4); sbCostPerDieta->setDecimals(2);
    sbCostPerDieta->setValue(getSettingDouble("cost_per_dieta", 25.0)); 
    mainLayout->addRow("6. Costo Real Dieta/Día (€):", sbCostPerDieta);
    
    // --- NUEVO: Costo Real Diario Furgoneta ---
    auto *sbCostVanDay = new QDoubleSpinBox;
    sbCostVanDay->setRange(0, 1e4); sbCostVanDay->setDecimals(2);
    sbCostVanDay->setValue(getSettingDouble("cost_van_day", 45.0)); 
    mainLayout->addRow("7. Costo Real Furgoneta/Día (€):", sbCostVanDay);

    // ====================================================
    // 3. IVA
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
        saveSetting("increment_per_field", sbIncrement->value());
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
