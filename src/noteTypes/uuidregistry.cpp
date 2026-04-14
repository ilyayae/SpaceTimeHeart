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
        "   uuid            TEXT PRIMARY KEY,"
        "   filepath        TEXT NOT NULL UNIQUE,"
        "   connected_uuids TEXT NOT NULL DEFAULT ''"
        ")"
        );

    query.exec("CREATE INDEX IF NOT EXISTS idx_filepath ON uuid_registry(filepath)");
}

bool UuidRegistry::writeEntry(const QUuid &uuid, const QString &filePath, const QList<QUuid> &links)
{
    QStringList parts;
    for (const QUuid &link : links)
        parts << link.toString(QUuid::WithoutBraces);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare(
        "INSERT OR REPLACE INTO uuid_registry (uuid, filepath, connected_uuids) "
        "VALUES (:uuid, :path, :links)"
        );
    query.bindValue(":uuid",  uuid.toString(QUuid::WithoutBraces));
    QSettings settings("zhopets", "SpaceTimeHeart");
    QDir workDir(settings.value("general/WorkDirectory", "/home").toString());
    query.bindValue(":path",  workDir.relativeFilePath(filePath));
    query.bindValue(":links", parts.join(','));

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
        QSettings settings("zhopets", "SpaceTimeHeart");
        QDir workDir(settings.value("general/WorkDirectory", "/home").toString());
        return workDir.absoluteFilePath(query.value(0).toString());
    }
    return QString();
}

QUuid UuidRegistry::getUuid(const QString &filePath) const
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT uuid FROM uuid_registry WHERE filepath = :path");
    QSettings settings("zhopets", "SpaceTimeHeart");
    QDir workDir(settings.value("general/WorkDirectory", "/home").toString());
    query.bindValue(":path", workDir.relativeFilePath(filePath));

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

QList<QPair<QUuid, QString>> UuidRegistry::getAllUuids() const
{
    QList<QPair<QUuid, QString>> result;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT uuid, filepath FROM uuid_registry");
    if (query.exec()) {
        while(query.next())
        {
            result.append(*new QPair<QUuid, QString>(query.value(0).toUuid(), query.value(1).toString()));
        }
    }
    return result;
}

QList<QUuid> UuidRegistry::getConnectedUuids(const QUuid &uuid) const
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT connected_uuids FROM uuid_registry WHERE uuid = :uuid");
    query.bindValue(":uuid", uuid.toString(QUuid::WithoutBraces));

    QList<QUuid> result;
    if (query.exec() && query.next()) {
        const QString raw = query.value(0).toString();
        if (!raw.isEmpty()) {
            for (const QString &part : raw.split(',', Qt::SkipEmptyParts))
                result << QUuid::fromString(part);
        }
    }
    return result;
}

void UuidRegistry::validateEntries()
{
    QString connName = connectionName;
    QSqlDatabase mainDb = QSqlDatabase::database(connName);
    QString dbPath = mainDb.databaseName();

    auto future = QtConcurrent::run([connName, dbPath]()
                      {
                          QSettings settings("zhopets", "SpaceTimeHeart");
                          QDir workDir(settings.value("general/WorkDirectory", "/home").toString());

                          QString threadConnName = connName + "_validator";
                          {
                              QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", threadConnName);
                              db.setDatabaseName(dbPath);
                              if (!db.open())
                              {
                                  QSqlDatabase::removeDatabase(threadConnName);
                                  return;
                              }
                              QStringList invalidUuids;
                              {
                                  QSqlQuery selectQuery(db);
                                  selectQuery.setForwardOnly(true);
                                  selectQuery.exec("SELECT uuid, filepath FROM uuid_registry");
                                  while (selectQuery.next())
                                  {
                                      QString uuid = selectQuery.value(0).toString();
                                      QString relativePath = selectQuery.value(1).toString();
                                      QString absolutePath = workDir.absoluteFilePath(relativePath);
                                      if (!QFile::exists(absolutePath))
                                          invalidUuids.append(uuid);
                                  }
                              }
                              if (!invalidUuids.isEmpty())
                              {
                                  db.transaction();
                                  QSqlQuery deleteQuery(db);
                                  deleteQuery.prepare("DELETE FROM uuid_registry WHERE uuid = :uuid");
                                  for (const QString &uuid : invalidUuids)
                                  {
                                      deleteQuery.bindValue(":uuid", uuid);
                                      qDebug() << "Killing UUID:" + uuid.toStdString();;
                                      deleteQuery.exec();
                                  }
                                  db.commit();
                              }

                              db.close();
                          }
                          QSqlDatabase::removeDatabase(threadConnName);
                      });
    Q_UNUSED(future);
}
