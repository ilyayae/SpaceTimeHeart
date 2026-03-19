#ifndef CALENDARDATA_H
#define CALENDARDATA_H
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QMap>
#include <cstdint>
#include <QUuid>
constexpr uint32_t CCAL_MAGIC   = 0x4343414C; // "CCAL" in ASCII
constexpr uint16_t CCAL_VERSION = 3;
inline const QString CCAL_EXTENSION = QStringLiteral(".ccal");
struct MonthDefinition {
    QString name;
    int dayCount = 30;
};
QDataStream &operator<<(QDataStream &out, const MonthDefinition &m);
QDataStream &operator>>(QDataStream &in,  MonthDefinition &m);
struct MoonDefinition {
    QString name;
    QString color;
    double cycleLengthDays = 29.53;
    double epochDayOffset  = 0.0;
};
QDataStream &operator<<(QDataStream &out, const MoonDefinition &m);
QDataStream &operator>>(QDataStream &in,  MoonDefinition &m);
struct CalendarConfigData {
    QString calendarName;
    int weekLength = 7;
    QVector<QString> dayNames;
    QVector<MonthDefinition> months;
    QVector<MoonDefinition> moons;
    int monthCount() const { return months.size(); }
    int daysInYear() const;
    qint64 absoluteDay(int year, int month, int day) const;
    int weekdayOf(qint64 absoluteDay) const;
};
QDataStream &operator<<(QDataStream &out, const CalendarConfigData &c);
QDataStream &operator>>(QDataStream &in,  CalendarConfigData &c);
struct RecurringDateKey {
    int month;
    int day;
    bool operator<(const RecurringDateKey &o) const;
    bool operator==(const RecurringDateKey &o) const;
};
QDataStream &operator<<(QDataStream &out, const RecurringDateKey &k);
QDataStream &operator>>(QDataStream &in,  RecurringDateKey &k);
struct SpecificDateKey {
    int year;
    int month;
    int day;
    bool operator<(const SpecificDateKey &o) const;
    bool operator==(const SpecificDateKey &o) const;
};
QDataStream &operator<<(QDataStream &out, const SpecificDateKey &k);
QDataStream &operator>>(QDataStream &in,  SpecificDateKey &k);
struct DayLink {
    QString targetNoteId;
    QString displayLabel;
    QString colorHex;
};
QDataStream &operator<<(QDataStream &out, const DayLink &l);
QDataStream &operator>>(QDataStream &in,  DayLink &l);
struct CalendarData {
    CalendarConfigData config;
    QMap<RecurringDateKey, QVector<DayLink>> recurringEvents;
    QMap<SpecificDateKey, QVector<DayLink>> specificEvents;
    QUuid myUuid;
    QString myPath;
    int lastViewedYear = 1;
    int lastViewedMonth = 0;
    QVector<DayLink> linksForDay(int year, int month, int day) const;
    void addRecurringLink(int month, int day, const DayLink &link);
    void addSpecificLink(int year, int month, int day, const DayLink &link);
    void removeLinksToNote(const QString &noteId);
    bool save(const QString &path, CalendarData &data);
    bool load(const QString &path, CalendarData &data);
    QUuid GetUuid();
    QString GetPath();
};
QDataStream &operator<<(QDataStream &out, const CalendarData &d);
QDataStream &operator>>(QDataStream &in,  CalendarData &d);
#endif // CALENDARDATA_H
