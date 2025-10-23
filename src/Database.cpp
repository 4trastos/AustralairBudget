#include "Database.hpp"

QSqlDatabase db;

bool Database::init(const QString &path) {
    if (QSqlDatabase::contains("australair_conn")) {
        db = QSqlDatabase::database("australair_conn");
        return db.isOpen();
    }
    db = QSqlDatabase::addDatabase("QSQLITE", "australair_conn");
    db.setDatabaseName(path);
    if (!db.open()) {
        qDebug() << "Cannot open DB:" << db.lastError().text();
        return false;
    }
    ensureSchema();
    return true;
}

QSqlDatabase Database::instance() {
    return db;
}

void Database::ensureSchema() {
    QSqlQuery q(db);
    q.exec(R"(
    CREATE TABLE IF NOT EXISTS clients (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      budget TEXT,
      name TEXT,
      company TEXT,
      cif TEXT,
      contact TEXT,
      address TEXT,
      phone TEXT,
      email TEXT,
      fecha TEXT
    ))");
    q.exec(R"(
    CREATE TABLE IF NOT EXISTS budgets (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      client_id INTEGER,
      tipo_local TEXT,
      metros REAL,
      tipo_cubierta TEXT,
      elevador TEXT,
      portes REAL,
      dias_elevador INTEGER,
      zona TEXT,
      dietas INTEGER,
      dias_dieta INTEGER,
      localidad TEXT,
      km REAL,
      combustible REAL,
      operarios INTEGER,
      dias_trabajo INTEGER,
      horas REAL,
      base_price REAL,
      total_no_iva REAL,
      total_con_iva REAL,
      status TEXT DEFAULT 'abierta',
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY(client_id) REFERENCES clients(id)
    ))");

    q.exec(R"(
    CREATE TABLE IF NOT EXISTS materials (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      budget_id INTEGER,
      name TEXT,
      quantity REAL,
      unit_price REAL,
      cost_price REAL,
      FOREIGN KEY(budget_id) REFERENCES budgets(id)
    ))");
    q.exec(R"(
    CREATE TABLE IF NOT EXISTS settings (
      key TEXT PRIMARY KEY,
      value TEXT
    ))");

    q.exec(R"(
      CREATE TABLE IF NOT EXISTS actual_costs (
    budget_id INTEGER,
    real_hours REAL,
    real_km REAL,
    real_fuel REAL,
    real_materials_cost REAL,
    real_benefit REAL,
    FOREIGN KEY(budget_id) REFERENCES budgets(id)
    ))");

    // defaults: price_base=10, increment_per_field=5, iva=21
    q.prepare("INSERT OR IGNORE INTO settings(key,value) VALUES(?,?)");
    q.addBindValue("price_base"); q.addBindValue("10"); q.exec();
    q.addBindValue("increment_per_field"); q.addBindValue("5"); q.exec();
    q.addBindValue("iva_pct"); q.addBindValue("21"); q.exec();
}
