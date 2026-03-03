#ifndef UUIDREGISTRY_H
#define UUIDREGISTRY_H
#include <QObject>
#include <QUuid>
#include <QString>
#include <QtSql/QSqlDatabase>

class UuidRegistry : public QObject
{
    Q_OBJECT
public:
    explicit UuidRegistry(const QString &dbPath, QObject *parent = nullptr);
    ~UuidRegistry();

    bool writeEntry(const QUuid &uuid, const QString &filePath);
    bool removeEntry(const QUuid &uuid);
    QString getPath(const QUuid &uuid) const;
    QUuid getUuid(const QString &filePath) const;
    bool contains(const QUuid &uuid) const;

private:
    void initDatabase();
    QString connectionName;
};

#endif // UUIDREGISTRY_H
