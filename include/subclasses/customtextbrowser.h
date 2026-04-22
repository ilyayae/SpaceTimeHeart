#ifndef CUSTOMTEXTBROWSER_H
#define CUSTOMTEXTBROWSER_H

#include "include/subclasses/searchhighlighter.h"
#include "include/noteTypes/uuidregistry.h"
#include <QObject>
#include <QTextBrowser>
#include <QRegularExpression>
#include <QUuid>
#include <QPainter>

class CustomTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    CustomTextBrowser(QWidget *parent);
    void setMarkdown(const QString &markdown);
    void setTextWithProcessing(const QString &text);
    QString preprocessMarkdown(const QString &text);
    void applyUnderline();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    QString getClickedHyperlink(const QPoint &pos);
    void highlightText(bool b, QString find);
    void keyPressEvent(QKeyEvent *event) override;
    QList<QUuid> getUuids();
    UuidRegistry *registry;
    QString originalText;
private:
    QList<QPair<QUuid, QPair<int, int>>> locationsOfLinks;
    QRegularExpression uuidFindRegex;
    QRegularExpression linkFindRegex;
    SearchHighlighter *highlighter = nullptr;
    QString originalMarkdown;
signals:
    void uuidClicked(QUuid uuid);
    void mdCheckboxChanged(QString content);
};

#endif // CUSTOMTEXTBROWSER_H
