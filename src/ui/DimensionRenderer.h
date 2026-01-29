/**
 * DimensionRenderer.h
 *
 * Renders dimensions and measurements for geometry objects
 */

#ifndef PATTERNCAD_DIMENSIONRENDERER_H
#define PATTERNCAD_DIMENSIONRENDERER_H

#include <QObject>
#include <QPainter>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    class Line;
    class Circle;
    class Rectangle;
    class Polyline;
}

namespace UI {

/**
 * DimensionRenderer draws dimension annotations for geometry objects
 */
class DimensionRenderer
{
public:
    DimensionRenderer();

    // Render dimensions for an object
    void renderDimensions(QPainter* painter, Geometry::GeometryObject* object);

    // Settings
    void setShowDimensions(bool show) { m_showDimensions = show; }
    bool showDimensions() const { return m_showDimensions; }

private:
    void drawLineDimension(QPainter* painter, Geometry::Line* line);
    void drawCircleDimension(QPainter* painter, Geometry::Circle* circle);
    void drawRectangleDimension(QPainter* painter, Geometry::Rectangle* rect);
    void drawPolylineDimension(QPainter* painter, Geometry::Polyline* polyline);

    void drawDimensionLine(QPainter* painter, const QPointF& start, const QPointF& end,
                          const QString& text, double offset = 15.0);
    void drawDimensionText(QPainter* painter, const QPointF& position, const QString& text);

    bool m_showDimensions;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_DIMENSIONRENDERER_H
