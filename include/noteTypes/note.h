#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QUuid>
#include <QFile>
#include <QMessageBox>
#include <include/noteTypes/uuidregistry.h>

class Note : public QObject
{
    Q_OBJECT
public:
    explicit Note(QObject *parent = nullptr);
    explicit Note(QString filePath, QObject *parent = nullptr);
    QUuid getUuid();
    QString getPath();
    QString getContent();
    QList<QUuid> GetMyLinks();
    void setContent(QString c);
    void setLinkedUuids(QList<QUuid> list);
    void load();
    void save();
    void saveAs(QString path);

    QList<QUuid> myLinks;

private:
    QUuid *uuid;
    QString filePath;
    QString content;
};

#endif // NOTE_H
