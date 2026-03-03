#ifndef SEARCHHIGHLIGHTER_H
#define SEARCHHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QUuid>

class SearchHighlighter : public QSyntaxHighlighter
{
public:
    SearchHighlighter(QTextDocument *parent);
    void setSearchString(const QString &str);

protected:
    void highlightBlock(const QString &text) override;

private:
    QString searchString;
    QRegularExpression uuidFindRegex;
    QRegularExpression linkFindRegex;
};

#endif // SEARCHHIGHLIGHTER_H
