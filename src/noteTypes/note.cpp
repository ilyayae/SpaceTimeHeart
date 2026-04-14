#include "include/noteTypes/note.h"
#include "qdebug.h"

Note::Note(QObject *parent)
    : QObject{parent}
{
    qWarning() << "Attempting to construct a Note object without a filePath";
}

Note::Note(QString filepath, QObject *parent)
    : QObject{parent}
{
    filePath = filepath;
    uuid = nullptr;
    load();
}

QUuid Note::getUuid()
{
    return *uuid;
}

QString Note::getPath()
{
    return filePath;
}

QString Note::getContent()
{
    return content;
}

QList<QUuid> Note::GetMyLinks()
{
    return myLinks;
}

void Note::setContent(QString c)
{
    content = c;
}

void Note::setLinkedUuids(QList<QUuid> list)
{
    myLinks = list;
}

void Note::load()
{
    if (filePath.isEmpty())
    {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open file: %1").arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QString line;
    bool firstContentLine = true;
    while (in.readLineInto(&line))
    {
        if(line.startsWith("#&$"))
        {
            QString data = line.mid(3);

            int sepIndex = data.indexOf(':');
            if(sepIndex == -1)
            {
                if (!firstContentLine) {
                    content.append('\n');
                }
                content.append(line);
                firstContentLine = false;
                continue;
            }

            QString key = data.left(sepIndex).trimmed().toUpper();
            QString value = data.mid(sepIndex + 1).trimmed();

            if(key == "UUID")
            {
                QUuid uuidCandidate = QUuid::fromString(value);
                if(!uuidCandidate.isNull())
                {
                    delete uuid;
                    uuid = new QUuid(uuidCandidate);
                }
            }
            else if(key == "LINKEDUUIDS")
            {
                myLinks.clear();
                for (const QString &part : value.split(',', Qt::SkipEmptyParts))
                {
                    QUuid linkCandidate = QUuid::fromString(part);
                    if (!linkCandidate.isNull())
                        myLinks << linkCandidate;
                }
            }
        }
        else
        {
            if (!firstContentLine) {
                content.append('\n');
            }
            content.append(line);
            firstContentLine = false;
        }
    }

    file.close();
}

void Note::save()
{
    if (uuid == nullptr)
        uuid = new QUuid(QUuid::createUuid());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot save file: %1").arg(file.errorString()));
        return;
    }

    QStringList linkParts;
    for (const QUuid &link : myLinks)
        linkParts << link.toString(QUuid::WithoutBraces);

    QTextStream out(&file);
    out << "#&$UUID:" << uuid->toString(QUuid::WithoutBraces) << "\n";
    out << "#&$LINKEDUUIDS:" << linkParts.join(',') << "\n";
    out << "#&$NOTE:Please do not touch this or above lines of text, these hold information for proper work of the SpaceTimeHeart application with this file \n";
    out << content;
    file.close();
}

void Note::saveAs(QString path)
{
    filePath = path;
    save();
}
