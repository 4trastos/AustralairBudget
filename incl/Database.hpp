#ifndef DATABASE_HPP
# define DATABASE_HPP

# pragma once
# include <QtSql>
# include <QString>
# include <QFile>
# include <QDebug>

class Database {
    public:
        static bool init(const QString &path = "australair.db");
        static QSqlDatabase instance();
        // helpers
        static void ensureSchema();
};

#endif