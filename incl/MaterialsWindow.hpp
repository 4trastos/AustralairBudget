#ifndef MATERIALSWINDOW_HPP
# define MATERIALSWINDOW_HPP
# pragma once
# include <QDialog>
# include <QtWidgets>

struct SelectedMaterial {
    QString name;
    double pvp;
    double cost;
    int quantity;
};

class MaterialsWindow : public QDialog
{
    Q_OBJECT
public:
    // El constructor recibe el puntero a MainWindow para poder comunicar si se añade un nuevo material
    MaterialsWindow(QWidget *parent = nullptr);
    //~MaterialsWindow() = default;
    QList<SelectedMaterial> getSelectedMaterials() const;

private slots:
    void onCategoryChanged(const QString &category);
    void onMaterialSelected(QListWidgetItem *item);
    void onAddMaterialClicked();
    void onRemoveMaterialClicked();
    void onSaveSelection();

private:
    void setupUi();

    void loadMaterials();
    
    QList<SelectedMaterial> selectedMaterials;
    QMap<QString, QMap<QString, QPair<double, double>>> materialsData;

    // Widgets para la nueva interfaz
    QListWidget *lwCategories;
    QListWidget *lwMaterials;
    QTableWidget *twSelection; // Tabla de selección temporal
    QPushButton *btnAdd;
    QPushButton *btnRemove;
    QPushButton *btnSave; // El botón "Cerrar" original será "Guardar Selección"
};

#endif // MATERIALSWINDOW_HPP