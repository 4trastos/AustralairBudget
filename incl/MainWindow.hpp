#ifndef MAINWINDOW_HPP
# define MAINWINDOW_HPP
# pragma once
# include <QMainWindow>
# include <QSqlTableModel>
# include <QtWidgets>
# include <QSqlQuery>
# include <QApplication>
# include <QtPrintSupport/QPrinter>
# include <QtPrintSupport/QPrintDialog>
# include "MaterialsWindow.hpp"
# include <QtPrintSupport/QPrintPreviewDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QSpinBox;
class QTextEdit;
class QDoubleSpinBox;
class QTableWidget;
class QPushButton;
class QListWidget;
class QListWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onCalculate();
    void onSaveBudget();
    void onLoadSelectedBudget();
    void onNewMaterial();
    void onRemoveMaterial();
    void onEditPrices();
    void onExportPDF();
    void onPrintBudget();
    void onPrint();
    void onPrintPreview();
    void onBackToStart();
    void onDeleteSelectedBudget();
    void onDeleteFields();
    void refreshBudgetsList();
    void onToggleStatus();
    void updateDistanceAndFuel();
    void updateDietasStatus();
    void setNextBudgetNumberAndDate();
    void onOpenMaterialsWindow();

private:
    void setupUi();
    void loadSettings();
    double getSettingDouble(const QString &key, double def=0.0);
    QString generateBudgetHtml(int id);
    QMap<QString, QPair<double, double>> loadMaterialsFromFile(const QString &filename); // NUEVA
    QMap<QString, double> loadPricesFromFile(const QString &filename);
    QMap<QString, QPair<double, double>> materialsMap;  // NUEVO
    void onMaterialChanged(QTableWidgetItem *item);
    void calculateCostsAndBenefits(double &totalSelling, double &totalCost, double &estimatedBenefit);  // NUEVA
    void createSampleMaterialsFile(const QString &filename);                    // NUEVA
    void updateMaterialTotal(int row);      // NUEVA


    // --- Variables de estado ----
    int currentBudgetId = 0;
    QString currentStatus = "Abierta";
    
    // --- Widgets----
    QLineEdit *leClientName, *leCompany, *leContact, *leAddress, *lePhone, *leEmail, *leCIF, *leNumPresu, *leLocalidadObra, *leFecha, *leExtra;
    QDoubleSpinBox *sbMetros, *sbKM, *sbHoras, *sbLitros, *sbElevPortes, *sbHorasViaje;
    QSpinBox *spDietas, *spDias, *spDiasDieta, *spElevDia, *spOperarios, *spElevPrecDia, *spPrecioDiet;
    QComboBox *cbTipoLocal, *cbTipoCubierta, *cbZona, *cbDietasNo, *cbDietasYes, *cbMaterials, *cbElevador, *cbExtractor;
    QTableWidget *twMaterials;
    QPushButton *btnAddMat, *btnRemoveMat, *btnCalc, *btnSave, *btnPDF, *btnPrint, *btnDelFields, *btnToggleStatus, *btnNewBudget, *btnStart;
    QListWidget *lwBudgets;
    QRadioButton *rbDietasSi, *rbDietasNo, *rbCorta, *rbMedia, *rbLarga;

    // --- Totales ---
    QLabel *lblTotalNoIVA, *lblTotalConIVA;
    QLabel *lblTotalNoIVA_Abierta, *lblTotalConIVA_Abierta;
    QLabel *lblTotalNoIVA_Cerrada, *lblTotalConIVA_Cerrada;
    QLabel *lblCosteEmpresa, *lblBeneficio;
    QLabel *lblCostoEstimado, *lblBeneficioEstimado;        // NUEVO
};

#endif