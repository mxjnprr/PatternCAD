/**
 * Point2D.h
 *
 * 2D point class for pattern geometry
 */

#ifndef PATTERNCAD_POINT2D_H
#define PATTERNCAD_POINT2D_H

#include "GeometryObject.h"
#include <QPointF>

namespace PatternCAD {
namespace Geometry {

/**
 * Point2D represents a single point in 2D space:
 * - Position (x, y)
 * - Visual representation
 * - Named control points
 * - Snap targets
 */
class Point2D : public GeometryObject
{
    Q_OBJECT

public:
    explicit Point2D(QObject* parent = nullptr);
    Point2D(const QPointF& position, QObject* parent = nullptr);
    Point2D(double x, double y, QObject* parent = nullptr);
    ~Point2D();

    // Type identification
    ObjectType type() const override;
    QString typeName() const override;

    // Position
    QPointF position() const;
    void setPosition(const QPointF& position);
    void setPosition(double x, double y);

    double x() const;
    void setX(double x);

    double y() const;
    void setY(double y);

    // Distance
    double distanceTo(const QPointF& other) const;
    double distanceTo(const Point2D* other) const;

    // Geometry overrides
    QRectF boundingRect() const override;
    bool contains(const QPointF& point) const override;
    void translate(const QPointF& delta) override;
    void rotate(double angleDegrees, const QPointF& center) override;
    void mirror(const QPointF& axisPoint1, const QPointF& axisPoint2) override;

    // Drawing
    void draw(QPainter* painter, const QColor& color = Qt::black) const override;

private:
    QPointF m_position;
    static constexpr double POINT_SIZE = 6.0;
    static constexpr double HIT_TOLERANCE = 10.0;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_POINT2D_H
