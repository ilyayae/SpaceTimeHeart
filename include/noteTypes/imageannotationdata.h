#ifndef IMAGEANNOTATIONDATA_H
#define IMAGEANNOTATIONDATA_H
#include <QString>
#include <QList>
#include <QDataStream>
#include <QMap>
#include <cstdint>
#include <QUuid>

constexpr uint32_t IMAN_MAGIC   = 0x494D414E; // "IMAN" in ASCII
constexpr uint16_t IMAN_VERSION = 1;
inline const QString IMAN_EXTENSION = QStringLiteral(".iman");


QDataStream &operator<<(QDataStream &out, const QPair<double, double> &p);
QDataStream &operator>>(QDataStream &in, QPair<double, double> &p);

struct MarkerData
{
    double X;
    double Y;
    int size = 16;
    QUuid Link;
    QString Color;
    QString Label;
    QString IconId;
    bool operator==(const MarkerData &o) const;
};
QDataStream &operator<<(QDataStream &out, const MarkerData &m);
QDataStream &operator>>(QDataStream &in,  MarkerData &m);

struct LineStyle
{
    QString lineColor;
    QString linePatternId;
    int width;
    bool operator==(const LineStyle &o) const;
};
QDataStream &operator<<(QDataStream &out, const LineStyle &m);
QDataStream &operator>>(QDataStream &in,  LineStyle &m);

struct FillStyle
{
    QString fillColor;
    QString fillPatternId;
    bool operator==(const FillStyle &o) const;
};
QDataStream &operator<<(QDataStream &out, const FillStyle &m);
QDataStream &operator>>(QDataStream &in,  FillStyle &m);

struct ShapeData
{
    QList<QPair<double, double>> XYPoints;
    bool Closed; //Is shape closed creating a polygon or is ti just a jagged line
    LineStyle StyleOfLine;
    FillStyle StyleOfFill;
    double rounding;
    bool operator==(const ShapeData &o) const;
};
QDataStream &operator<<(QDataStream &out, const ShapeData &m);
QDataStream &operator>>(QDataStream &in,  ShapeData &m);

struct ImageAnnotationData
{
    QList<MarkerData> markers;
    QList<ShapeData> shapes;
    QByteArray imageData;
    QString imageFormat;
    QUuid myUuid;
    QString myPath;
    QList<QUuid> myLinks;

    QUuid GetUuid();
    QString GetPath();
    QList<QUuid> GetMyLinks();

    void addMarker(double x, double y, int size, QUuid link, QString color, QString label, QString iconId);
    void addShape(QList<QPair<double, double>> xyPoints, bool closed, QString lineColor, QString fillColor, int width, QString linePatternId, QString fillPatternId, double rounding);
    void removeMarker(MarkerData marker);
    void removeShape(ShapeData shape);
    bool save(const QString &path, ImageAnnotationData &data);
    bool load(const QString &path, ImageAnnotationData &data);
};
QDataStream &operator<<(QDataStream &out, const ImageAnnotationData &m);
QDataStream &operator>>(QDataStream &in,  ImageAnnotationData &m);

#endif // IMAGEANNOTATIONDATA_H
