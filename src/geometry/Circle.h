/**
 * Circle.h
 *
 * Circle class for pattern geometry
 */

#ifndef PATTERNCAD_CIRCLE_H
#define PATTERNCAD_CIRCLE_H

#include "GeometryObject.h"
#include <QPointF>

namespace PatternCAD {
namespace Geometry {

/**
 * Circle represents a circle in 2D space:
 * - Center point
 * - Radius
 * - Area and circumference calculations
 * - Point containment testing
 */
class Circle : public GeometryObject
{
    Q_OBJECT

public:
    explicit Circle(QObject* parent = nullptr);
    Circle(const QPointF& center, double radius, QObject* parent = nullptr);
    ~Circle();

    // Type identification
    ObjectType type() const override;
    QString typeName() const override;

    // Center
    QPointF center() const;
    void setCenter(const QPointF& center);

    // Radius
    double radius() const;
    void setRadius(double radius);

    // Circle properties
    double diameter() const;
    double area() const;
    double circumference() const;

    // Geometry calculations
    bool containsPoint(const QPointF& point) const;
    double distanceToPoint(const QPointF& point) const;

    // Geometry overrides
    QRectF boundingRect() const override;
    bool contains(const QPointF& point) const override;
    void translate(const QPointF& delta) override;
    void rotate(double angleDegrees, const QPointF& center) override;

    // Drawing
    void draw(QPainter* painter, const QColor& color = Qt::black) const override;

private:
    QPointF m_center;
    double m_radius;
    static constexpr double HIT_TOLERANCE = 5.0;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_CIRCLE_H
