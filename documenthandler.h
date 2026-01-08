#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H

#include <QWidget>
#include <QObject>
#include <QTextEdit>
#include <QSettings>

class DocumentHandler : public QWidget
{
    Q_OBJECT
public:
    explicit DocumentHandler(QWidget *parent = nullptr,  QSettings *settings = nullptr);
    void loadFile(QTextEdit *editor, QString fileName);
    void saveFile(QTextEdit *editor);
private:
    QString filePath;
    bool saveable = true;
    QSettings *setts;
signals:
};

#endif // DOCUMENTHANDLER_H
