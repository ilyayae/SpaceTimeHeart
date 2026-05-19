#ifndef CALENDARDATA_H
#define CALENDARDATA_H
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QMap>
#include <cstdint>
#include <QUuid>
#include <QSet>
constexpr uint32_t CCAL_MAGIC   = 0x4343414C; // "CCAL" in ASCII
constexpr uint16_t CCAL_VERSION = 4;
inline const QString CCAL_EXTENSION = QStringLiteral(".ccal");

struct DayLink {
    QString targetNoteId;
    QString displayLabel;
    QString colorHex;
};
QDataStream &operator<<(QDataStream &out, const DayLink &l);
QDataStream &operator>>(QDataStream &in,  DayLink &l);

struct LeapDayException
{
    int priority; // the exception to the leapday with priority P
    int everyNYears; // occurs every N years
    int offsetYears; // with an offset of Y years
    bool happens; // to cause it to happen or not to happen
    // The highest priority exception is used to determine if the leapday happens,
    // if no LeapDayException can be applied to the LeapDayDifinition, it happens.
    // otherwise the leap day occurs only if the happens == true;
};
QDataStream &operator<<(QDataStream &out, const LeapDayException &m);
QDataStream &operator>>(QDataStream &in,  LeapDayException &m);

struct LeapDayDefinition {
    QVector<DayLink> uniqueToMeLinks;
    int targetsMonth;
    int happensEveryNYears = 4;
    int offsetYears = 0;
    QVector<LeapDayException> exceptions;
    int daysAdded = 1;
    bool affectsWeekDays = true;
    bool pickRandomDay = false;
    bool pickRandomMonth = false;
};
QDataStream &operator<<(QDataStream &out, const LeapDayDefinition &m);
QDataStream &operator>>(QDataStream &in,  LeapDayDefinition &m);

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
    int weekOffset = 0;
    QVector<QString> dayNames;
    QVector<MonthDefinition> months;
    QVector<MoonDefinition> moons;
    QVector<LeapDayDefinition> leapDays;
    int monthCount() const { return months.size(); }
    int daysInYear() const;
    int daysInYear(int year) const;
    qint64 absoluteDay(int year, int month, int day) const;
    int weekdayOf(qint64 absoluteDay) const;
    int weekdayOf(int year, int month, int day) const;
    bool isLeapYear(int year, const LeapDayDefinition &leapDef) const;
    int daysInMonth(int year, int month) const;
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

struct CalendarData {
    CalendarConfigData config;
    QMap<RecurringDateKey, QVector<DayLink>> recurringEvents;
    QMap<SpecificDateKey, QVector<DayLink>> specificEvents;
    QUuid myUuid;
    QString myPath;
    int lastViewedYear = 1;
    int lastViewedMonth = 0;
    QVector<DayLink> linksForDay(int year, int month, int day, const LeapDayDefinition* leapDef = nullptr) const;
    void addRecurringLink(int month, int day, const DayLink &link);
    void addSpecificLink(int year, int month, int day, const DayLink &link);
    void removeLinksToNote(const QString &noteId);
    bool save(const QString &path, CalendarData &data);
    bool load(const QString &path, CalendarData &data);
    QUuid GetUuid();
    QString GetPath();
    QList<QUuid> GetMyLinks();
};
QDataStream &operator<<(QDataStream &out, const CalendarData &d);
QDataStream &operator>>(QDataStream &in,  CalendarData &d);
#endif // CALENDARDATA_H
