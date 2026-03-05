#ifndef FINDREPLACEWIDGET_H
#define FINDREPLACEWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QScrollBar>
#include <QTimer>

#include "ui_findreplacewidget.h"
#include "include/subclasses/customtextbrowser.h"

namespace Ui {
class FindReplaceWidget;
}

class FindReplaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindReplaceWidget(QWidget *parent = nullptr, QTextBrowser *browser = nullptr);
    ~FindReplaceWidget();
    QList<int> findText();
    void selectText();
    void show(bool b);
private slots:
    void on_FindButton_clicked();

    void on_FindPrevButton_clicked();

    void on_FindNextButton_clicked();

    void on_ReplaceButton_clicked();

signals:
    void findReplaceChanged();

private:
    Ui::FindReplaceWidget *ui;
    QList<int> textFoundPositions;
    int currentFoundPosition;
    int currentFoundWordLength;
    bool isActivelySearchingWords;
    QTextBrowser *myTextEdit;
};

#endif // FINDREPLACEWIDGET_H
