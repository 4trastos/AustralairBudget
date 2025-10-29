#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

const QMap<QString, QMap<QString, QPair<double, double>>> MaterialsData = {
    {"MÁQUINAS", {
        {"TBS", {1500, 1000}},
        {"CW3", {3500, 1200}},
        {"COOLAIR", {960, 500}},
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

MaterialsWindow::MaterialsWindow(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Gestión de Materiales - Base de Datos");
    setModal(true);
    materialsData = MaterialsData; // Carga la base de datos
    setupUi();
    loadMaterials();
}

MaterialsWindow::~MaterialsWindow(){}

void MaterialsWindow::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *hLayout = new QHBoxLayout;
    
    // --- 1. Panel de Categorías ---
    lwCategories = new QListWidget;
    lwCategories->setMaximumWidth(150);
    hLayout->addWidget(lwCategories);

    // --- 2. Panel de Materiales por Categoría ---
    lwMaterials = new QListWidget;
    hLayout->addWidget(lwMaterials);

    // --- 3. Panel de Controles (Añadir/Eliminar) ---
    auto *vControls = new QVBoxLayout;
    btnAdd = new QPushButton("Añadir »");
    btnRemove = new QPushButton("« Eliminar");
    vControls->addWidget(btnAdd);
    vControls->addWidget(btnRemove);
    vControls->addStretch(); // Relleno para centrar botones
    hLayout->addLayout(vControls);

    // --- 4. Tabla de Selección ---
    twSelection = new QTableWidget(0, 4);
    twSelection->setHorizontalHeaderLabels({"Nombre", "PVP", "Costo", "Cantidad"});
    twSelection->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Para que se pueda modificar la cantidad
    twSelection->setSelectionBehavior(QAbstractItemView::SelectRows);
    twSelection->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Conexiones
    connect(lwCategories, &QListWidget::currentTextChanged, this, &MaterialsWindow::onCategoryChanged);
    connect(btnAdd, &QPushButton::clicked, this, &MaterialsWindow::onAddMaterialClicked);
    connect(btnRemove, &QPushButton::clicked, this, &MaterialsWindow::onRemoveMaterialClicked);
    connect(twSelection, &QTableWidget::cellChanged, this, [this](int row, int column){
        // Solo nos interesa la columna de Cantidad (columna 3)
        if (column == 3) {
            bool ok;
            int newQty = twSelection->item(row, column)->text().toInt(&ok);
            if (ok && row < selectedMaterials.size()) {
                selectedMaterials[row].quantity = newQty;
            }
        }
    });

    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(new QLabel("Materiales a añadir al presupuesto:"));
    mainLayout->addWidget(twSelection);
    
    btnSave = new QPushButton("Guardar Selección y Cerrar");
    connect(btnSave, &QPushButton::clicked, this, &MaterialsWindow::onSaveSelection);
    mainLayout->addWidget(btnSave, 0, Qt::AlignRight);

    setMinimumSize(1200, 600);
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