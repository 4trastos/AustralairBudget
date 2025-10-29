#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"
#include "MaterialsWindow.hpp"

// --- Slot para Imprimir Presupuesto ---

void MainWindow::onPrintBudget() {
    // 1. Asegurarse de que el cálculo esté hecho
    onCalculate();

    // 2. Generar el contenido HTML
    int currentId = lwBudgets->currentItem() ? lwBudgets->currentItem()->data(Qt::UserRole).toInt() : 0;
    QString htmlContent = generateBudgetHtml(currentId);
    
    // 3. Crear el documento
    QTextDocument document;
    document.setHtml(htmlContent);

    // 4. Configurar impresora y mostrar diálogo
    QPrinter printer(QPrinter::PrinterResolution);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        document.print(&printer);
    }
}

void MainWindow::onPrint() {
    // TODO: implementar impresión directa si es necesario
}

void MainWindow::onPrintPreview() {
    // TODO: implementar vista previa de impresión si es necesario
}
