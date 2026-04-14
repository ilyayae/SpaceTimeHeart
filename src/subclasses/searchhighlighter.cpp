#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QUuid>

#include "include/subclasses/searchhighlighter.h"

SearchHighlighter::SearchHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    uuidFindRegex.setPattern("\\[\\[([a-fA-F0-9\\-]{36})\\]\\]");
    linkFindRegex.setPattern("(http|ftp|https):\/\/([a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF_-]+(?:(?:\.[a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF_-]+)+))([a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF.,@?^=%&:\/~+#-]*[a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF@?^=%&\/~+#-])");
}

void SearchHighlighter::setSearchString(const QString &str) {
    searchString = str;
    rehighlight();
}

void SearchHighlighter::highlightBlock(const QString &text) {
    //Hyperlink-recognition functionality
    QRegularExpressionMatchIterator it = uuidFindRegex.globalMatch(text);
    while (it.hasNext())
    {
        QRegularExpressionMatch match = it.next();
        QUuid uuid = QUuid::fromString(match.captured(1));
        if (!uuid.isNull())
        {
            QTextCharFormat format;
            format.setForeground(QColor("blue"));
            format.setFontUnderline(true);
            setFormat(match.capturedStart(), match.capturedLength(), format);
        }
    }
    //"note://" + uuid.toString(QUuid::WithoutBraces)

    QRegularExpressionMatchIterator two = linkFindRegex.globalMatch(text);
    while (two.hasNext())
    {
        QRegularExpressionMatch match = two.next();
        QTextCharFormat format;
        format.setForeground(QColor("blue"));
        format.setFontUnderline(true);
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }

    //Search-highlight functionality
    if (searchString.isEmpty()) return;
    QTextCharFormat format;
    format.setBackground(Qt::yellow);

    int index = 0;
    while ((index = text.indexOf(searchString, index, Qt::CaseInsensitive)) != -1) {
        setFormat(index, searchString.length(), format);
        index += searchString.length();
    }
}
