#include "include/subclasses/customtextbrowser.h"
#include <QMouseEvent>
#include <QDesktopServices>

CustomTextBrowser::CustomTextBrowser(QWidget *parent)
    : QTextBrowser(parent)
{
    setUndoRedoEnabled(true);
    setOpenExternalLinks(true);
    uuidFindRegex.setPattern("\\[\\[([a-fA-F0-9\\-]{36})\\]\\]");
    linkFindRegex.setPattern("(http|ftp|https):\/\/([a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF_-]+(?:(?:\.[a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF_-]+)+))([a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF.,@?^=%&:\/~+#-]*[a-zA-Z0-9\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF@?^=%&\/~+#-])");

    highlighter = new SearchHighlighter(document());
}

void CustomTextBrowser::setMarkdown(const QString &markdown)
{
    QTextBrowser::setMarkdown(markdown);
    applyUnderline();
}

void CustomTextBrowser::applyUnderline()
{
    QTextDocument *doc = document();
    QTextCursor cursor(doc);

    while (true) {
        cursor = doc->find("++", cursor);
        if (cursor.isNull())
            break;
        int startPos = cursor.selectionStart();
        cursor.removeSelectedText();
        QTextCursor closeCursor = doc->find("++", cursor);
        if (closeCursor.isNull())
            break;
        int endPos = closeCursor.selectionStart();
        closeCursor.removeSelectedText();
        cursor.setPosition(startPos);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        QTextCharFormat fmt = cursor.charFormat();
        fmt.setFontUnderline(true);
        cursor.mergeCharFormat(fmt);
        cursor.setPosition(startPos + (endPos - startPos));
    }
}



void CustomTextBrowser::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        QString clicked = getClickedHyperlink(event->pos());
        if(!clicked.isEmpty())
        {
            if(clicked.size() == 40)
            {
                QString uuidString = clicked.mid(2, clicked.length() - 4);
                QUuid uuidCandidate = QUuid(uuidString);
                if(!uuidCandidate.isNull())
                {
                    emit uuidClicked(uuidCandidate);
                    return;
                }
            }
            QUrl url = QUrl(clicked);
            QDesktopServices::openUrl(url);
        }
    }
    QTextBrowser::mousePressEvent(event);
}

void CustomTextBrowser::mouseMoveEvent(QMouseEvent *event)
{
    QString hovered = getClickedHyperlink(event->pos());
    if (!hovered.isEmpty() && (event->modifiers() & Qt::ControlModifier)) {
        viewport()->setCursor(Qt::PointingHandCursor);
    } else {
        viewport()->setCursor(Qt::IBeamCursor);
    }
    QTextBrowser::mouseMoveEvent(event);
}

QString CustomTextBrowser::getClickedHyperlink(const QPoint &pos)
{
    QTextCursor cursor = cursorForPosition(pos);
    int cursorPos = cursor.position();
    QString text = toPlainText();
    QRegularExpressionMatchIterator one = uuidFindRegex.globalMatch(text);
    while (one.hasNext())
    {
        QRegularExpressionMatch match = one.next();
        int start = match.capturedStart();
        int end = match.capturedEnd();
        if (cursorPos >= start && cursorPos <= end)
        {
            return text.mid(start, end - start);
        }
    }
    QRegularExpressionMatchIterator two = linkFindRegex.globalMatch(text);
    while (two.hasNext())
    {
        QRegularExpressionMatch match = two.next();
        int start = match.capturedStart();
        int end = match.capturedEnd();
        if (cursorPos >= start && cursorPos <= end)
        {
            return text.mid(start, end - start);
        }
    }
    return "";
}


void CustomTextBrowser::highlightText(bool b, QString find)
{
    QTextCursor cursor = textCursor();
    if(b)
        highlighter->setSearchString(find);
    else
        highlighter->setSearchString("");
}
