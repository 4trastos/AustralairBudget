#include "AustralairBudget.hpp"
#include "Database.hpp"
#include "MainWindow.hpp"

// --- Generación de Contenido HTML ---

QString MainWindow::generateBudgetHtml(int id) {
    QString html = R"(
        <style>
            body { font-family: Arial, sans-serif; }
            h1 { color: #004d99; border-bottom: 2px solid #ccc; padding-bottom: 5px; }
            .section { margin-top: 20px; padding: 10px; background-color: #f7f7f7; border: 1px solid #eee; }
            table { width: 100%; border-collapse: collapse; margin-top: 10px; }
            th, td { border: 1px solid #ccc; padding: 8px; text-align: left; }
            th { background-color: #e0e0e0; }
            .total-row td { font-weight: bold; background-color: #cceeff; }
        </style>
        <h1>Presupuesto Australair ID: %1</h1>
    )";
    html = html.arg(id);

    // --- Datos del Cliente ---
    html += R"(<div class="section"><h2>Datos del Cliente</h2>)";
    html += QString("<table><tr><td>**Cliente:** %1</td><td>**Contacto:** %2</td></tr>")
                .arg(leClientName->text(), leContact->text());
    html += QString("<tr><td>**Empresa:** %1</td><td>**Teléfono:** %2</td></tr>")
                .arg(leCompany->text(), lePhone->text());
    html += QString("<tr><td>**Dirección:** %1</td><td>**Email:** %2</td></tr></table></div>")
                .arg(leAddress->text(), leEmail->text());

    // --- Detalles del Proyecto ---
    QLineEdit *leLocalidad = findChild<QLineEdit*>("leLocalidad");
    html += R"(<div class="section"><h2>Detalles del Proyecto</h2>)";
    html += "<table>";
    html += QString("<tr><td>**Metros Cuadrados:** %1 m²</td><td>**Localidad:** %2</td></tr>")
                .arg(sbMetros->text(), leLocalidad ? leLocalidad->text() : QString());
    html += QString("<tr><td>**Tipo de Local:** %1</td><td>**Tipo de Cubierta:** %2</td></tr>")
                .arg(cbTipoLocal->currentText(), cbTipoCubierta->currentText());
    html += QString("<tr><td>**KM Desplazamiento:** %1 km</td><td>**Días de Trabajo:** %2</td></tr>")
                .arg(sbKM->text(), QString::number(spDias->value()));
    html += QString("<tr><td>**Horas Estimadas:** %1</td><td>**Dietas (empleados):** %2</td></tr>")
                .arg(sbHoras->text(), QString::number(spDietas->value()));
    html += "</table></div>";

    // --- Materiales ---
    html += R"(<div class="section"><h2>Materiales</h2>)";
    html += "<table><tr><th>Nombre</th><th>Cantidad</th><th>Precio Unit.</th><th>Total</th></tr>";
    for (int r = 0; r < twMaterials->rowCount(); ++r) {
        QString name = twMaterials->item(r, 0)->text();
        QString qty  = twMaterials->item(r, 1)->text();
        QString up   = twMaterials->item(r, 2)->text();
        QString totalLine = twMaterials->item(r, 3)->text();
        html += QString("<tr><td>%1</td><td>%2</td><td>%3 €</td><td>%4 €</td></tr>")
                    .arg(name, qty, up, totalLine);
    }
    html += "</table></div>";

    // --- Totales ---
    html += R"(<div class="section"><h2>Resumen Económico</h2>)";
    html += "<table>";

    double totalNoIva = lblTotalNoIVA->text().replace(" €","").toDouble();
    double totalConIva = lblTotalConIVA->text().replace(" €","").toDouble();
    double base = getSettingDouble("price_base", 0);
    double incrementPerField = getSettingDouble("increment_per_field", 0);
    int filledFields = 10; // Ajusta según la lógica de cálculo real
    double totalBaseAndIncrements = base + incrementPerField * filledFields;
    double totalMaterials = totalNoIva - totalBaseAndIncrements;
    double ivaPct = getSettingDouble("iva_pct", 21);

    html += QString("<tr><td style='width:70%%'>**Total Base + Incrementos**</td><td style='text-align:right'>%1 €</td></tr>")
                .arg(QString::number(totalBaseAndIncrements, 'f', 2));
    html += QString("<tr><td>**Total Materiales**</td><td style='text-align:right'>%1 €</td></tr>")
                .arg(QString::number(totalMaterials, 'f', 2));
    html += QString("<tr class='total-row'><td>**SUBTOTAL (Sin IVA)**</td><td style='text-align:right'>%1 €</td></tr>")
                .arg(QString::number(totalNoIva, 'f', 2));
    html += QString("<tr><td>**IVA (%1%%)**</td><td style='text-align:right'>%2 €</td></tr>")
                .arg(QString::number(ivaPct),
                     QString::number(totalConIva - totalNoIva, 'f', 2));
    html += QString("<tr class='total-row'><td>**TOTAL CON IVA**</td><td style='text-align:right'>%1 €</td></tr>")
                .arg(QString::number(totalConIva, 'f', 2));

    html += "</table></div>";

    return html;
}

// --- Slot para Guardar en PDF ---

void MainWindow::onExportPDF() {
    // 1. Asegurarse de que el cálculo esté hecho
    onCalculate();

    // 2. Pedir al usuario la ruta del archivo
    QString fileName = QFileDialog::getSaveFileName(this, "Guardar presupuesto como PDF",
                                                    "presupuesto_" + QDateTime::currentDateTime().toString("yyyyMMdd") + ".pdf",
                                                    "Archivos PDF (*.pdf)");
    if (fileName.isEmpty()) return;

    // 3. Generar el contenido HTML
    int currentId = lwBudgets->currentItem() ? lwBudgets->currentItem()->data(Qt::UserRole).toInt() : 0;
    QString htmlContent = generateBudgetHtml(currentId);

    // 4. Crear el documento e imprimir
    QTextDocument document;
    document.setHtml(htmlContent);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    document.print(&printer);

    QMessageBox::information(this, "Exportar PDF", "Presupuesto guardado exitosamente como PDF.");
}
