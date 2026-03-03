#ifndef HYPERLINKTEXTBROWSER_H
#define HYPERLINKTEXTBROWSER_H

#include <QObject>
#include <QTextBrowser>
#include <QUuid>
#include <QRegularExpression>
#include "include/subclasses/searchhighlighter.h"

class HyperlinkTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    HyperlinkTextBrowser(QWidget *parent);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    QString getClickedHyperlink(const QPoint &pos);
    void highlightText(bool b, QString find);
private:
    QRegularExpression uuidFindRegex;
    QRegularExpression linkFindRegex;
    SearchHighlighter *highlighter = nullptr;
signals:
    void uuidClicked(QUuid uuid);
};

#endif // HYPERLINKTEXTBROWSER_H
