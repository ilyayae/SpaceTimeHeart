#ifndef NOTE_H
#define NOTE_H

#include <QString>
#include <QUuid>
#include <QFile>
#include <QMessageBox>

class Note : public QObject
{
    Q_OBJECT
public:
    explicit Note(QObject *parent = nullptr);
    explicit Note(QString filePath, QObject *parent = nullptr);
    QUuid getUuid();
    QString getPath();
    QString getContent();
    void setContent(QString c);
    void load();
    void save();
    void saveAs(QString path);

private:
    QUuid *uuid;
    QString filePath;
    QString content;
    QList<QString> linkedUuids;
};

#endif // NOTE_H
