#ifndef AUSTRALAIRBUDGET_HPP
# define AUSTRALAIRBUDGET_HPP

# include <QWidget>
# include <QPushButton>
# include <QLabel>
# include <QVBoxLayout>
# include <QApplication>
# include <QPixmap>
# include <QMessageBox>

class AustralairBudget : public QWidget {
    Q_OBJECT 

public:
    explicit AustralairBudget(QWidget *parent = nullptr);

private slots:
    void openPresupuestos();
    void openMantenimientos();

private:
    QPushButton *btnPresupuestos;
    QPushButton *btnMantenimientos;
    QLabel *lblLogo;
    QLabel *lblTitle;
};

#endif