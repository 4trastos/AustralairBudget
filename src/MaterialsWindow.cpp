#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

// const QMap<QString, QMap<QString, QPair<double, double>>> MaterialsData = {
//     {"MÁQUINAS", {
//         {"TBS", {1500, 1000}},
//         {"CW3", {3500, 1200}},
//         {"COOLAIR", {960, 500}},
//         {"ICON", {1250, 1000}},
//         {"TBSi", {1800, 1400}}
//     }},
//     {"EXTRACTOR", {
//         {"SETA", {500, 400}},
//         {"GRAN CAUDAL", {800, 500}}
//     }},
//     {"FONTANERIA", {{"FONTANERIA", {600, 300}}}},
//     {"ELECTRICIDAD", {{"ELECTRICIDAD", {700, 400}}}},
//     {"CONDUCTOS", {{"CONDUCTOS", {100, 30}}}},
//     {"SUPLEMENTOS CONDUCTOS", {{"SUPLEMENTOS CONDUCTOS", {250, 100}}}},
//     {"REGILLAS", {{"REGILLAS", {150, 120}}}},
//     {"PASARELA POR CUBIERTA", {{"PASARELA POR CUBIERTA", {250, 130}}}},
//     {"LINEA DE VIDA", {{"LINEA DE VIDA", {60, 20}}}},
//     {"APERTURA HUECO", {{"APERTURA HUECO", {200, 80}}}},
//     {"SELLAMIENTO", {{"SELLAMIENTO", {120, 90}}}},
//     {"SOPORTACIÓN", {{"SOPORTACIÓN", {100, 20}}}},
// };

static QMap<QString, QMap<QString, QPair<double, double>>> &getMasterMaterialsData()
{
    static QMap<QString, QMap<QString, QPair<double, double>>> s_materialsData = {
        {"MÁQUINAS", {
            {"TBS", {3500, 1000}},
            {"CW3", {5500, 1200}},
            {"COOLAIR", {1960, 900}},
            {"ICON", {1250, 1000}},
            {"TBSi", {1800, 1400}}
        }},
        {"EXTRACTOR", {
            {"SETA", {500, 400}},
            {"GRAN CAUDAL", {800, 500}}
        }},
        {"FONTANERIA", {{"FONTANERIA", {600, 300}}}},
        {"ELECTRICIDAD", {{"ELECTRICIDAD", {700, 400}}}},
        {"CONDUCTOS", {{"CONDUCTOS", {100, 30}}}},
        {"SUPLEMENTOS CONDUCTOS", {{"SUPLEMENTOS CONDUCTOS", {250, 100}}}},
        {"REGILLAS", {{"REGILLAS", {150, 120}}}},
        {"PASARELA POR CUBIERTA", {{"PASARELA POR CUBIERTA", {250, 130}}}},
        {"LINEA DE VIDA", {{"LINEA DE VIDA", {60, 20}}}},
        {"APERTURA HUECO", {{"APERTURA HUECO", {200, 80}}}},
        {"SELLAMIENTO", {{"SELLAMIENTO", {120, 90}}}},
        {"SOPORTACIÓN", {{"SOPORTACIÓN", {100, 20}}}},
    };
    return s_materialsData;
}

MaterialsWindow::MaterialsWindow(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Gestión de Materiales - Base de Datos");
    setModal(true);
    materialsData = getMasterMaterialsData(); // Carga la base de datos
    setupUi();
    loadMaterials();
}

//MaterialsWindow::~MaterialsWindow(){}

void MaterialsWindow::setupUi()
{
    // Crear todos los widgets PRIMERO
    lwCategories = new QListWidget(this);
    lwMaterials = new QListWidget(this);
    twSelection = new QTableWidget(0, 4, this);
    btnAdd = new QPushButton("Añadir »", this);
    btnRemove = new QPushButton("« Eliminar", this);
    btnSave = new QPushButton("Guardar Selección y Cerrar", this);

    auto *mainLayout = new QVBoxLayout(this);
    
    // Splitter principal
    auto *hSplitter = new QSplitter(Qt::Horizontal, this);

    // Panel izquierdo: Categorías
    lwCategories->setMaximumWidth(200);
    hSplitter->addWidget(lwCategories);

    // Panel central: Materiales  
    lwMaterials->setMinimumWidth(300);
    hSplitter->addWidget(lwMaterials);

    // Panel derecho: Controles
    auto *controlsPanel = new QWidget(this);
    auto *controlsLayout = new QVBoxLayout(controlsPanel);
    controlsLayout->addWidget(btnAdd);
    controlsLayout->addWidget(btnRemove);
    controlsLayout->addStretch();
    controlsPanel->setMaximumWidth(150);
    hSplitter->addWidget(controlsPanel);

    // Configurar splitter
    hSplitter->setStretchFactor(0, 1);  // Categorías
    hSplitter->setStretchFactor(1, 3);  // Materiales
    hSplitter->setStretchFactor(2, 1);  // Controles

    mainLayout->addWidget(hSplitter);

    // Tabla de selección
    auto *selectionLabel = new QLabel("Materiales seleccionados:", this);
    mainLayout->addWidget(selectionLabel);

    twSelection->setHorizontalHeaderLabels({"Nombre", "PVP", "Costo", "Cantidad"});
    twSelection->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    twSelection->setSelectionBehavior(QAbstractItemView::SelectRows);
    twSelection->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(twSelection);

    // Botón guardar
    mainLayout->addWidget(btnSave, 0, Qt::AlignRight);

    // Conexiones (igual que antes)
    connect(lwCategories, &QListWidget::currentTextChanged, this, &MaterialsWindow::onCategoryChanged);
    connect(btnAdd, &QPushButton::clicked, this, &MaterialsWindow::onAddMaterialClicked);
    connect(btnRemove, &QPushButton::clicked, this, &MaterialsWindow::onRemoveMaterialClicked);
    connect(btnSave, &QPushButton::clicked, this, &MaterialsWindow::onSaveSelection);
    
    connect(twSelection, &QTableWidget::cellChanged, this, [this](int row, int column){
        if (column == 3 && row >= 0 && row < selectedMaterials.size()) {
            bool ok;
            int newQty = twSelection->item(row, column)->text().toInt(&ok);
            if (ok) {
                selectedMaterials[row].quantity = newQty;
            }
        }
    });

    setMinimumSize(1000, 600);
}

void MaterialsWindow::loadMaterials()
{
    // Carga las categorías
    for (const QString &category : materialsData.keys()) {
        lwCategories->addItem(category);
    }
    // Selecciona la primera categoría para cargar los materiales
    if (lwCategories->count() > 0) {
        lwCategories->setCurrentRow(0);
    }
}

void MaterialsWindow::onCategoryChanged(const QString &category)
{
    lwMaterials->clear();
    if (materialsData.contains(category)) {
        for (const QString &materialName : materialsData.value(category).keys()) {
            QListWidgetItem *item = new QListWidgetItem(materialName);
            // Almacenar PVP y Costo en UserRole para fácil acceso
            QPair<double, double> prices = materialsData.value(category).value(materialName);
            item->setData(Qt::UserRole, prices.first); // PVP (Venta)
            item->setData(Qt::UserRole + 1, prices.second); // Costo (Compra)
            lwMaterials->addItem(item);
        }
    }
}

void MaterialsWindow::onAddMaterialClicked()
{
    QListWidgetItem *currentMat = lwMaterials->currentItem();
    if (!currentMat) return;

    // Extraer datos del material seleccionado
    SelectedMaterial mat;
    mat.name = currentMat->text();
    mat.pvp = currentMat->data(Qt::UserRole).toDouble();
    mat.cost = currentMat->data(Qt::UserRole + 1).toDouble();
    mat.quantity = 1; // Requisito: siempre se carga 1 por defecto

    // Añadir a la tabla de selección
    int row = twSelection->rowCount();
    twSelection->insertRow(row);
    twSelection->setItem(row, 0, new QTableWidgetItem(mat.name));
    twSelection->setItem(row, 1, new QTableWidgetItem(QString::number(mat.pvp, 'f', 2)));
    twSelection->setItem(row, 2, new QTableWidgetItem(QString::number(mat.cost, 'f', 2)));
    twSelection->setItem(row, 3, new QTableWidgetItem(QString::number(mat.quantity))); // Cantidad

    // Almacenar en la lista interna de materiales seleccionados
    selectedMaterials.append(mat);
}

void MaterialsWindow::onRemoveMaterialClicked()
{
    QModelIndexList selected = twSelection->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;

    // Eliminar siempre la primera fila seleccionada, y de la lista interna
    int row = selected.first().row();
    twSelection->removeRow(row);
    if (row < selectedMaterials.size()) {
        selectedMaterials.removeAt(row);
    }
}

void MaterialsWindow::onSaveSelection()
{
    // Esto cierra el diálogo y devuelve QDialog::Accepted
    accept();
}

QList<SelectedMaterial> MaterialsWindow::getSelectedMaterials() const
{
    return selectedMaterials;
}

void MaterialsWindow::onMaterialSelected(QListWidgetItem *item)
{
    // Función reservada para una posible doble-clic o selección avanzada.
    // De momento, solo la dejamos definida.
    Q_UNUSED(item); 
}

void MaterialsWindow::addMaterialToMasterList(const QString &category, const QString &name, double pvp, double cost)
{
    QPair<double, double> prices = qMakePair(pvp, cost);
    getMasterMaterialsData()[category].insert(name, prices);
    materialsData = getMasterMaterialsData();
}