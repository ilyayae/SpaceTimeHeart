#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include "include/noteTypes/uuidregistry.h"

UuidRegistry::UuidRegistry(const QString &dbPath, QObject *parent)
    : QObject(parent)
{
    connectionName = "UuidRegistry_" + QString::number(reinterpret_cast<quintptr>(this));

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(dbPath);

        if(!db.open())
        {
            qCritical() << "Failed to open database: " << db.lastError().text();
            return;
        }

        initDatabase();
    }
}

UuidRegistry::~UuidRegistry()
{
    {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if (db.isOpen()) {
            db.close();
        }
    }

    QSqlDatabase::removeDatabase(connectionName);
}

void UuidRegistry::initDatabase()
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QSqlQuery query(db);
    query.exec(
        "CREATE TABLE IF NOT EXISTS uuid_registry ("
        "   uuid TEXT PRIMARY KEY,"
        "   filepath TEXT NOT NULL UNIQUE"
        ")"
        );

    // Index for reverse lookups by filepath
    query.exec("CREATE INDEX IF NOT EXISTS idx_filepath ON uuid_registry(filepath)");
}

bool UuidRegistry::writeEntry(const QUuid &uuid, const QString &filePath)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO uuid_registry (uuid, filepath) VALUES (:uuid, :path)");
    query.bindValue(":uuid", uuid.toString(QUuid::WithoutBraces));
    query.bindValue(":path", filePath);

    if (!query.exec()) {
        qWarning() << "Failed to add entry:" << query.lastError().text();
        return false;
    }
    return true;
}

bool UuidRegistry::removeEntry(const QUuid &uuid)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("DELETE FROM uuid_registry WHERE uuid = :uuid");
    query.bindValue(":uuid", uuid.toString(QUuid::WithoutBraces));

    if (!query.exec()) {
        qWarning() << "Failed to remove entry:" << query.lastError().text();
        return false;
    }
    return true;
}

QString UuidRegistry::getPath(const QUuid &uuid) const
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT filepath FROM uuid_registry WHERE uuid = :uuid");
    query.bindValue(":uuid", uuid.toString(QUuid::WithoutBraces));

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

QUuid UuidRegistry::getUuid(const QString &filePath) const
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT uuid FROM uuid_registry WHERE filepath = :path");
    query.bindValue(":path", filePath);

    if (query.exec() && query.next()) {
        return QUuid::fromString(query.value(0).toString());
    }
    return QUuid();
}

bool UuidRegistry::contains(const QUuid &uuid) const
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM uuid_registry WHERE uuid = :uuid");
    query.bindValue(":uuid", uuid.toString(QUuid::WithoutBraces));

    return query.exec() && query.next();
}
