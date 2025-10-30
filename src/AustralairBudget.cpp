#include "AustralairBudget.hpp"
#include "MainWindow.hpp"
#include <QApplication>
#include <QPixmap>
#include <QMessageBox>
#include <QFont>
#include <QHBoxLayout>

AustralairBudget::AustralairBudget(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Australair - Inicio");
    //setFixedSize(800, 500);
    showMaximized();

    // Logo
    lblLogo = new QLabel(this);
    QPixmap pix(":/images/logo.png");
    lblLogo->setPixmap(pix.scaled(288, 29, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    lblLogo->setAlignment(Qt::AlignCenter);

    // Título
    lblTitle = new QLabel("<h1>Australair Budget Manager</h1><h3>Creado por 4Trastos Digital Solutions</h3>", this);
    lblTitle->setAlignment(Qt::AlignCenter);

    // Botones estilo dashboard
    btnPresupuestos = new QPushButton(QIcon(":/icon_presupuesto.png"), "Presupuestos", this);
    btnMantenimientos = new QPushButton(QIcon(":/icon_mantenimiento.png"), "Mantenimientos", this);

    // Tamaño y estilo de botones
    QSize btnSize(250, 80);
    QFont btnFont;
    btnFont.setPointSize(14);
    btnFont.setBold(true);

    btnPresupuestos->setFixedSize(btnSize);
    btnMantenimientos->setFixedSize(btnSize);

    btnPresupuestos->setFont(btnFont);
    btnMantenimientos->setFont(btnFont);

    // Colores con estilo CSS
    btnPresupuestos->setStyleSheet("QPushButton { background-color: #007ACC; color: white; border-radius: 10px; }"
                                   "QPushButton:hover { background-color: #005999; }");
    btnMantenimientos->setStyleSheet("QPushButton { background-color: #28A745; color: white; border-radius: 10px; }"
                                     "QPushButton:hover { background-color: #1e7e34; }");

    connect(btnPresupuestos, &QPushButton::clicked, this, &AustralairBudget::openPresupuestos);
    connect(btnMantenimientos, &QPushButton::clicked, this, &AustralairBudget::openMantenimientos);

    // Layout de botones horizontal centrado
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btnPresupuestos);
    btnLayout->addSpacing(30);
    btnLayout->addWidget(btnMantenimientos);
    btnLayout->addStretch();

    // Layout vertical principal
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(lblLogo);
    mainLayout->addSpacing(30);
    mainLayout->addWidget(lblTitle);
    mainLayout->addSpacing(40);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

//AustralairBudget::~AustralairBudget(){}

// --- Slots ---
void AustralairBudget::openPresupuestos() {
    MainWindow *w = new MainWindow;
    w->show();
    this->close(); // cerrar ventana de inicio si quieres
}

void AustralairBudget::openMantenimientos() {
    QMessageBox::information(this, "Mantenimientos", "Funcionalidad no implementada aún.");
}