#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

void MainWindow::updateDistanceAndFuel() {
    // Constante: consumo de las furgonetas (litros cada 100 km)
    const double FURGONETA_CONSUMO = 10.0; // ejemplo: 10 l/100km

    // Leer los kilómetros desde el QDoubleSpinBox
    double km = sbKM->value();

    // Calcular litros necesarios
    double litros = km * (FURGONETA_CONSUMO / 100.0);

    // Actualizar el QDoubleSpinBox de combustible
    sbLitros->setValue(litros);
}

void MainWindow::updateDietasStatus() {
    QString zona = cbZona->currentText();

    if (zona == "Zona Centro") {
        // Zona Centro: no se puede activar dietas
        spDietas->setEnabled(false);
        spDiasDieta->setEnabled(false);
        rbDietasSi->setEnabled(false);
        rbDietasNo->setChecked(true); // Siempre No
        rbDietasNo->setEnabled(false);
    } else {
        // Otras Zonas: dietas habilitadas normalmente
        rbDietasSi->setEnabled(true);
        rbDietasNo->setEnabled(true);
        bool siChecked = rbDietasSi->isChecked();
        spDietas->setEnabled(siChecked);
        spDiasDieta->setEnabled(siChecked);
    }
}

