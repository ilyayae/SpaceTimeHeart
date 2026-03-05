#ifndef CUSTOMTEXTBROWSER_H
#define CUSTOMTEXTBROWSER_H

#include "include/subclasses/searchhighlighter.h"

#include <QObject>
#include <QTextBrowser>
#include <QRegularExpression>
#include <QUuid>

class CustomTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    CustomTextBrowser(QWidget *parent);
    void setMarkdown(const QString &markdown);
    void applyUnderline();
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

#endif // CUSTOMTEXTBROWSER_H
