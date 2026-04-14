#include "include/noteTypes/imageannotationdata.h"

#include <QFile>
#include <QIODevice>
#include <cmath>

bool MarkerData::operator==(const MarkerData &o) const
{
    return qFuzzyCompare(X, o.X) &&
        qFuzzyCompare(Y, o.Y) &&
        Link == o.Link &&
        Color == o.Color &&
        Label == o.Label &&
        IconId == o.IconId &&
        size == o.size;
}

bool LineStyle::operator==(const LineStyle &o) const
{
    return lineColor == o.lineColor &&
           linePatternId == o.linePatternId &&
           width == o.width;
}

bool FillStyle::operator==(const FillStyle &o) const
{
    return fillColor == o.fillColor &&
           fillPatternId == o.fillPatternId;
}

bool ShapeData::operator==(const ShapeData &o) const
{
    if (Closed != o.Closed)
        return false;
    if (!(StyleOfLine == o.StyleOfLine))
        return false;
    if (!(StyleOfFill == o.StyleOfFill))
        return false;
    if (XYPoints.size() != o.XYPoints.size())
        return false;
    if(rounding != o.rounding)
        return false;
    for (int i = 0; i < XYPoints.size(); i++)
    {
        if (!qFuzzyCompare(XYPoints[i].first, o.XYPoints[i].first) ||
            !qFuzzyCompare(XYPoints[i].second, o.XYPoints[i].second))
            return false;
    }
    return true;
}

QDataStream &operator<<(QDataStream &out, const QPair<double, double> &p)
{
    qint64 xBits, yBits;
    memcpy(&xBits, &p.first, sizeof(double));
    memcpy(&yBits, &p.second, sizeof(double));
    out << xBits << yBits;
    return out;
}

QDataStream &operator>>(QDataStream &in, QPair<double, double> &p)
{
    qint64 xBits, yBits;
    in >> xBits >> yBits;
    memcpy(&p.first, &xBits, sizeof(double));
    memcpy(&p.second, &yBits, sizeof(double));
    return in;
}

QDataStream &operator<<(QDataStream &out, const MarkerData &m)
{
    qint64 xBits, yBits;
    memcpy(&xBits, &m.X, sizeof(double));
    memcpy(&yBits, &m.Y, sizeof(double));
    out << m.Color << m.IconId << m.Label << m.Link << xBits << yBits << m.size;
    return out;
}
QDataStream &operator>>(QDataStream &in, MarkerData &m)
{
    qint64 xBits, yBits;
    in >> m.Color >> m.IconId >> m.Label >> m.Link >> xBits >> yBits >> m.size;
    memcpy(&m.X, &xBits, sizeof(double));
    memcpy(&m.Y, &yBits, sizeof(double));
    return in;
}

QDataStream &operator<<(QDataStream &out, const LineStyle &m)
{
    out << m.lineColor << m.linePatternId << m.width;
    return out;
}
QDataStream &operator>>(QDataStream &in,  LineStyle &m)
{
    in >> m.lineColor >> m.linePatternId >> m.width;
    return in;
}

QDataStream &operator<<(QDataStream &out, const FillStyle &m)
{
    out << m.fillColor << m.fillPatternId;
    return out;
}
QDataStream &operator>>(QDataStream &in, FillStyle &m)
{
    in >> m.fillColor >> m.fillPatternId;
    return in;
}

QDataStream &operator<<(QDataStream &out, const ShapeData &m)
{
    out << m.Closed << m.StyleOfFill << m.StyleOfLine << m.XYPoints << m.rounding;
    return out;
}
QDataStream &operator>>(QDataStream &in,  ShapeData &m)
{
    in >> m.Closed >> m.StyleOfFill >> m.StyleOfLine >> m.XYPoints >> m.rounding;
    return in;
}

QUuid ImageAnnotationData::GetUuid()
{
    return myUuid;
}
QString ImageAnnotationData::GetPath()
{
    return myPath;
}
QList<QUuid> ImageAnnotationData::GetMyLinks()
{
    myLinks.clear();

    for(const MarkerData &marker : markers)
    {
        if(!marker.Link.isNull())
            myLinks.append(marker.Link);
    }

    return myLinks;
}

void ImageAnnotationData::addMarker(double x, double y, int size, QUuid link, QString color, QString label, QString iconId)
{
    MarkerData mark = {x,y,size,link,color,label,iconId};
    markers.append(mark);
}
void ImageAnnotationData::addShape(QList<QPair<double, double>> xyPoints, bool closed, QString lineColor, QString fillColor, int width, QString linePatternId, QString fillPatternId, double rounding)
{
    LineStyle lstyle = {lineColor, linePatternId, width};
    FillStyle fstyle = {fillColor, fillPatternId};
    ShapeData shape = {xyPoints, closed, lstyle, fstyle, rounding};
    shapes.append(shape);
}
void ImageAnnotationData::removeMarker(MarkerData marker)
{
    for(int i = markers.count()-1; i >= 0; i--)
    {
        if(markers[i] == marker) markers.remove(i);
    }
}
void ImageAnnotationData::removeShape(ShapeData shape)
{
    for(int i = shapes.count()-1; i >= 0; i--)
    {
        if(shapes[i] == shape) shapes.remove(i);
    }
}
bool ImageAnnotationData::save(const QString &path, ImageAnnotationData &data)
{
    if (data.myUuid.isNull())
        data.myUuid = QUuid::createUuid();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    out << data;
    data.myPath = path;

    return out.status() == QDataStream::Ok;
}
bool ImageAnnotationData::load(const QString &path, ImageAnnotationData &data)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    in >> data;

    data.myPath = path;

    return in.status() == QDataStream::Ok;
}

QDataStream &operator<<(QDataStream &out, const ImageAnnotationData &m)
{
    out << IMAN_MAGIC;
    out << IMAN_VERSION;
    out << m.myUuid;
    out << m.myLinks;
    out << m.imageFormat;
    out << m.imageData;
    out << m.markers;
    out << m.shapes;
    return out;
}

QDataStream &operator>>(QDataStream &in, ImageAnnotationData &m)
{
    uint32_t magic;
    uint16_t version;
    in >> magic;
    in >> version;
    if (magic != IMAN_MAGIC)
    {
        in.setStatus(QDataStream::ReadCorruptData);
        return in;
    }
    if (version > IMAN_VERSION)
    {
        in.setStatus(QDataStream::ReadCorruptData);
        return in;
    }
    in >> m.myUuid;
    in >> m.myLinks;
    in >> m.imageFormat;
    in >> m.imageData;
    in >> m.markers;
    in >> m.shapes;
    return in;
}
