/**
 * CubicBezier.h
 *
 * Cubic Bezier curve class for pattern geometry
 */

#ifndef PATTERNCAD_CUBICBEZIER_H
#define PATTERNCAD_CUBICBEZIER_H

#include "GeometryObject.h"
#include <QPointF>
#include <QPainterPath>
#include <QVector>

namespace PatternCAD {
namespace Geometry {

/**
 * CubicBezier represents a cubic Bezier curve:
 * - Start and end points
 * - Two control points
 * - Point evaluation along curve (0.0 to 1.0)
 * - Length approximation
 * - Tangent calculation
 */
class CubicBezier : public GeometryObject
{
    Q_OBJECT

public:
    explicit CubicBezier(QObject* parent = nullptr);
    CubicBezier(const QPointF& p0, const QPointF& p1,
                const QPointF& p2, const QPointF& p3,
                QObject* parent = nullptr);
    ~CubicBezier();

    // Type identification
    ObjectType type() const override;
    QString typeName() const override;

    // Control points
    QPointF p0() const; // Start point
    void setP0(const QPointF& p0);

    QPointF p1() const; // First control point
    void setP1(const QPointF& p1);

    QPointF p2() const; // Second control point
    void setP2(const QPointF& p2);

    QPointF p3() const; // End point
    void setP3(const QPointF& p3);

    void setPoints(const QPointF& p0, const QPointF& p1,
                   const QPointF& p2, const QPointF& p3);

    // Curve evaluation
    QPointF pointAt(double t) const;
    QPointF tangentAt(double t) const;
    double length() const;

    // Geometry overrides
    QRectF boundingRect() const override;
    bool contains(const QPointF& point) const override;
    void translate(const QPointF& delta) override;
    void rotate(double angleDegrees, const QPointF& center) override;
    void mirror(const QPointF& axisPoint1, const QPointF& axisPoint2) override;

    // Drawing
    void draw(QPainter* painter, const QColor& color = Qt::black) const override;

private:
    QPointF m_p0; // Start point
    QPointF m_p1; // First control point
    QPointF m_p2; // Second control point
    QPointF m_p3; // End point

    static constexpr double HIT_TOLERANCE = 5.0;
    static constexpr int SAMPLE_COUNT = 20; // For length calculation

    // Helper methods
    QPainterPath createPath() const;
    double approximateLength() const;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_CUBICBEZIER_H
