/**
 * Line.h
 *
 * Line segment class for pattern geometry
 */

#ifndef PATTERNCAD_LINE_H
#define PATTERNCAD_LINE_H

#include "GeometryObject.h"
#include <QPointF>
#include <QLineF>

namespace PatternCAD {
namespace Geometry {

/**
 * Line represents a line segment in 2D space:
 * - Start and end points
 * - Length and angle calculations
 * - Intersection testing
 * - Perpendicular and parallel operations
 */
class Line : public GeometryObject
{
    Q_OBJECT

public:
    explicit Line(QObject* parent = nullptr);
    Line(const QPointF& start, const QPointF& end, QObject* parent = nullptr);
    ~Line();

    // Type identification
    ObjectType type() const override;
    QString typeName() const override;

    // Endpoints
    QPointF start() const;
    void setStart(const QPointF& start);

    QPointF end() const;
    void setEnd(const QPointF& end);

    void setPoints(const QPointF& start, const QPointF& end);

    // Line properties
    double length() const;
    double angle() const;
    QPointF midpoint() const;

    // Geometry calculations
    double distanceToPoint(const QPointF& point) const;
    QPointF closestPointOnLine(const QPointF& point) const;

    // Geometry overrides
    QRectF boundingRect() const override;
    bool contains(const QPointF& point) const override;
    void translate(const QPointF& delta) override;
    void rotate(double angleDegrees, const QPointF& center) override;
    void mirror(const QPointF& axisPoint1, const QPointF& axisPoint2) override;

    // Drawing
    void draw(QPainter* painter, const QColor& color = Qt::black) const override;

private:
    QPointF m_start;
    QPointF m_end;
    static constexpr double HIT_TOLERANCE = 5.0;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_LINE_H
