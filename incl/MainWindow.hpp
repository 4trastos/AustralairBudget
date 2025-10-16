#ifndef MAINWINDOW_HPP
# define MAINWINDOW_HPP
# pragma once
# include <QMainWindow>
# include <QSqlTableModel>
# include <QtWidgets>
# include <QSqlQuery>
# include <QApplication>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QSpinBox;
class QTextEdit;
class QDoubleSpinBox;
class QTableWidget;
class QPushButton;
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
private:
    void setupUi();
    void loadSettings();
    double getSettingDouble(const QString &key, double def=0.0);
    QString generateBudgetHtml(int id);
    // UI widgets
    QLineEdit *leClientName, *leCompany, *leContact, *leAddress, *lePhone, *leEmail;
    QDoubleSpinBox *sbMetros, *sbKM, *sbHoras;
    QSpinBox *spDietas, *spDias;
    QComboBox *cbTipoLocal, *cbTipoCubierta;
    QTableWidget *twMaterials;
    QPushButton *btnAddMat, *btnRemoveMat, *btnCalc, *btnSave, *btnPDF, *btnPrint;;
    QListWidget *lwBudgets;
    QLabel *lblTotalNoIVA, *lblTotalConIVA;
};

#endif