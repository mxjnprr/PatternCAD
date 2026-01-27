/**
 * Rectangle.h
 *
 * Rectangle class for pattern geometry
 */

#ifndef PATTERNCAD_RECTANGLE_H
#define PATTERNCAD_RECTANGLE_H

#include "GeometryObject.h"
#include <QPointF>
#include <QRectF>

namespace PatternCAD {
namespace Geometry {

/**
 * Rectangle represents a rectangle in 2D space:
 * - Position (top-left corner)
 * - Width and height
 * - Corner points
 * - Area calculations
 */
class Rectangle : public GeometryObject
{
    Q_OBJECT

public:
    explicit Rectangle(QObject* parent = nullptr);
    Rectangle(const QPointF& topLeft, double width, double height, QObject* parent = nullptr);
    Rectangle(const QRectF& rect, QObject* parent = nullptr);
    ~Rectangle();

    // Type identification
    ObjectType type() const override;
    QString typeName() const override;

    // Position
    QPointF topLeft() const;
    void setTopLeft(const QPointF& topLeft);

    QPointF topRight() const;
    QPointF bottomLeft() const;
    QPointF bottomRight() const;
    QPointF center() const;

    // Dimensions
    double width() const;
    void setWidth(double width);

    double height() const;
    void setHeight(double height);

    void setSize(double width, double height);

    // Rectangle properties
    double area() const;
    double perimeter() const;

    // QRectF conversion
    QRectF rect() const;
    void setRect(const QRectF& rect);

    // Geometry overrides
    QRectF boundingRect() const override;
    bool contains(const QPointF& point) const override;
    void translate(const QPointF& delta) override;

    // Drawing
    void draw(QPainter* painter) const override;

private:
    QPointF m_topLeft;
    double m_width;
    double m_height;
    static constexpr double HIT_TOLERANCE = 5.0;
};

} // namespace Geometry
} // namespace PatternCAD

#endif // PATTERNCAD_RECTANGLE_H
