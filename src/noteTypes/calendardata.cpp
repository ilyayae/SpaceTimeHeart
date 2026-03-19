#include "include/noteTypes/calendardata.h"

#include <QFile>
#include <QIODevice>
#include <cmath>

QDataStream &operator<<(QDataStream &out, const MonthDefinition &m)
{
    out << m.name << static_cast<qint32>(m.dayCount);
    return out;
}
QDataStream &operator>>(QDataStream &in, MonthDefinition &m)
{
    qint32 dc;
    in >> m.name >> dc;
    m.dayCount = dc;
    return in;
}

QDataStream &operator<<(QDataStream &out, const MoonDefinition &m)
{
    out << m.name << m.cycleLengthDays << m.epochDayOffset << m.color;
    return out;
}
QDataStream &operator>>(QDataStream &in, MoonDefinition &m)
{
    in >> m.name >> m.cycleLengthDays >> m.epochDayOffset >> m.color;
    return in;
}

int CalendarConfigData::daysInYear() const
{
    int total = 0;
    for (const auto &m : months)
        total += m.dayCount;
    return total;
}
qint64 CalendarConfigData::absoluteDay(int year, int month, int day) const
{
    qint64 abs = static_cast<qint64>(year) * daysInYear();
    for (int i = 0; i < month && i < months.size(); ++i)
        abs += months[i].dayCount;
    abs += (day - 1);
    return abs;
}
int CalendarConfigData::weekdayOf(qint64 absoluteDay) const
{
    if (weekLength <= 0)
        return 0;

    int wd = static_cast<int>(absoluteDay % weekLength);
    if (wd < 0) wd += weekLength;
    return wd;
}

QDataStream &operator<<(QDataStream &out, const CalendarConfigData &c)
{
    out << c.calendarName
        << static_cast<qint32>(c.weekLength)
        << c.dayNames
        << static_cast<qint32>(c.months.size());

    for (const auto &m : c.months)
        out << m;

    out << static_cast<qint32>(c.moons.size());
    for (const auto &moon : c.moons)
        out << moon;

    return out;
}
QDataStream &operator>>(QDataStream &in, CalendarConfigData &c)
{
    qint32 wl, monthCount, moonCount;

    in >> c.calendarName >> wl >> c.dayNames >> monthCount;
    c.weekLength = wl;

    c.months.resize(monthCount);
    for (int i = 0; i < monthCount; ++i)
        in >> c.months[i];

    in >> moonCount;
    c.moons.resize(moonCount);
    for (int i = 0; i < moonCount; ++i)
        in >> c.moons[i];

    return in;
}

bool RecurringDateKey::operator<(const RecurringDateKey &o) const
{
    if (month != o.month) return month < o.month;
    return day < o.day;
}
bool RecurringDateKey::operator==(const RecurringDateKey &o) const
{
    return month == o.month && day == o.day;
}
QDataStream &operator<<(QDataStream &out, const RecurringDateKey &k)
{
    out << static_cast<qint32>(k.month) << static_cast<qint32>(k.day);
    return out;
}
QDataStream &operator>>(QDataStream &in, RecurringDateKey &k)
{
    qint32 m, d;
    in >> m >> d;
    k.month = m;
    k.day   = d;
    return in;
}

bool SpecificDateKey::operator<(const SpecificDateKey &o) const
{
    if (year  != o.year)  return year  < o.year;
    if (month != o.month) return month < o.month;
    return day < o.day;
}
bool SpecificDateKey::operator==(const SpecificDateKey &o) const
{
    return year == o.year && month == o.month && day == o.day;
}
QDataStream &operator<<(QDataStream &out, const SpecificDateKey &k)
{
    out << static_cast<qint32>(k.year)
    << static_cast<qint32>(k.month)
    << static_cast<qint32>(k.day);
    return out;
}
QDataStream &operator>>(QDataStream &in, SpecificDateKey &k)
{
    qint32 y, m, d;
    in >> y >> m >> d;
    k.year  = y;
    k.month = m;
    k.day   = d;
    return in;
}

QDataStream &operator<<(QDataStream &out, const DayLink &l)
{
    out << l.targetNoteId << l.displayLabel << l.colorHex;
    return out;
}
QDataStream &operator>>(QDataStream &in, DayLink &l)
{
    in >> l.targetNoteId >> l.displayLabel >> l.colorHex;
    return in;
}

QVector<DayLink> CalendarData::linksForDay(int year, int month, int day) const
{
    QVector<DayLink> result;

    RecurringDateKey rk{month, day};
    if (recurringEvents.contains(rk))
        result.append(recurringEvents.value(rk));

    SpecificDateKey sk{year, month, day};
    if (specificEvents.contains(sk))
        result.append(specificEvents.value(sk));

    return result;
}
void CalendarData::addRecurringLink(int month, int day, const DayLink &link)
{
    RecurringDateKey key{month, day};
    recurringEvents[key].append(link);
}
void CalendarData::addSpecificLink(int year, int month, int day, const DayLink &link)
{
    SpecificDateKey key{year, month, day};
    specificEvents[key].append(link);
}
void CalendarData::removeLinksToNote(const QString &noteId)
{
    for (auto it = recurringEvents.begin(); it != recurringEvents.end(); ) {
        auto &vec = it.value();
        vec.erase(std::remove_if(vec.begin(), vec.end(),
                                 [&](const DayLink &l){ return l.targetNoteId == noteId; }),
                  vec.end());
        if (vec.isEmpty())
            it = recurringEvents.erase(it);
        else
            ++it;
    }

    for (auto it = specificEvents.begin(); it != specificEvents.end(); ) {
        auto &vec = it.value();
        vec.erase(std::remove_if(vec.begin(), vec.end(),
                                 [&](const DayLink &l){ return l.targetNoteId == noteId; }),
                  vec.end());
        if (vec.isEmpty())
            it = specificEvents.erase(it);
        else
            ++it;
    }
}
bool CalendarData::save(const QString &path, CalendarData &data)
{
    if (data.myUuid.isNull())
        data.myUuid = QUuid::createUuid();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    out << CCAL_MAGIC << CCAL_VERSION;

    out << data.myUuid;
    out << static_cast<qint32>(data.lastViewedYear) << static_cast<qint32>(data.lastViewedMonth);
    out << data;

    data.myPath = path;

    return out.status() == QDataStream::Ok;
}
bool CalendarData::load(const QString &path, CalendarData &data)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);

    uint32_t magic;
    uint16_t version;
    in >> magic >> version;

    if (magic != CCAL_MAGIC)
        return false;

    if (version > CCAL_VERSION)
        return false;

    if (version >= 2) {
        in >> data.myUuid;
    } else {
        data.myUuid = QUuid::createUuid();
    }
    if (version >= 3) {
        qint32 y, m;
        in >> y >> m;
        data.lastViewedYear  = y;
        data.lastViewedMonth = m;
    } else {
        data.lastViewedYear  = 1;
        data.lastViewedMonth = 0;
    }

    in >> data;

    data.myPath = path;

    return in.status() == QDataStream::Ok;
}

QUuid CalendarData::GetUuid()
{
    return myUuid;
}
QString CalendarData::GetPath()
{
    return myPath;
}

QDataStream &operator<<(QDataStream &out, const CalendarData &d)
{
    out << d.config;

    out << static_cast<qint32>(d.recurringEvents.size());
    for (auto it = d.recurringEvents.cbegin(); it != d.recurringEvents.cend(); ++it) {
        out << it.key() << static_cast<qint32>(it.value().size());
        for (const auto &link : it.value())
            out << link;
    }

    out << static_cast<qint32>(d.specificEvents.size());
    for (auto it = d.specificEvents.cbegin(); it != d.specificEvents.cend(); ++it) {
        out << it.key() << static_cast<qint32>(it.value().size());
        for (const auto &link : it.value())
            out << link;
    }

    return out;
}
QDataStream &operator>>(QDataStream &in, CalendarData &d)
{
    in >> d.config;

    qint32 recurringCount;
    in >> recurringCount;
    d.recurringEvents.clear();
    for (int i = 0; i < recurringCount; ++i) {
        RecurringDateKey key;
        qint32 linkCount;
        in >> key >> linkCount;
        QVector<DayLink> links(linkCount);
        for (int j = 0; j < linkCount; ++j)
            in >> links[j];
        d.recurringEvents.insert(key, links);
    }

    qint32 specificCount;
    in >> specificCount;
    d.specificEvents.clear();
    for (int i = 0; i < specificCount; ++i) {
        SpecificDateKey key;
        qint32 linkCount;
        in >> key >> linkCount;
        QVector<DayLink> links(linkCount);
        for (int j = 0; j < linkCount; ++j)
            in >> links[j];
        d.specificEvents.insert(key, links);
    }

    return in;
}
