/**
 * DimensionRenderer.cpp
 *
 * Implementation of DimensionRenderer
 */

#include "DimensionRenderer.h"
#include "geometry/GeometryObject.h"
#include "geometry/Line.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "geometry/Polyline.h"
#include "core/Units.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <cmath>

namespace PatternCAD {
namespace UI {

DimensionRenderer::DimensionRenderer()
    : m_showDimensions(true)
{
}

void DimensionRenderer::renderDimensions(QPainter* painter, Geometry::GeometryObject* object)
{
    if (!m_showDimensions || !object) {
        return;
    }

    // Only show dimensions for selected objects
    if (!object->isSelected()) {
        return;
    }

    if (auto* line = dynamic_cast<Geometry::Line*>(object)) {
        drawLineDimension(painter, line);
    } else if (auto* circle = dynamic_cast<Geometry::Circle*>(object)) {
        drawCircleDimension(painter, circle);
    } else if (auto* rect = dynamic_cast<Geometry::Rectangle*>(object)) {
        drawRectangleDimension(painter, rect);
    } else if (auto* polyline = dynamic_cast<Geometry::Polyline*>(object)) {
        drawPolylineDimension(painter, polyline);
    }
}

void DimensionRenderer::drawLineDimension(QPainter* painter, Geometry::Line* line)
{
    double lengthMm = line->length();
    QString lengthText = Units::formatLength(lengthMm, 1);

    drawDimensionLine(painter, line->start(), line->end(), lengthText, 20.0);
}

void DimensionRenderer::drawCircleDimension(QPainter* painter, Geometry::Circle* circle)
{
    painter->save();

    double radiusMm = circle->radius();
    QString radiusText = "R " + Units::formatLength(radiusMm, 1);
    QString diameterText = "Ø " + Units::formatLength(radiusMm * 2, 1);

    QPointF center = circle->center();

    // Draw radius line to the right
    QPointF radiusEnd = center + QPointF(radiusMm, 0);

    QPen dimPen(QColor(200, 100, 0), 1, Qt::DashLine);
    painter->setPen(dimPen);
    painter->drawLine(center, radiusEnd);

    // Draw radius text
    QPointF textPos = center + QPointF(radiusMm / 2, -10);
    drawDimensionText(painter, textPos, radiusText);

    // Draw diameter text at top
    QPointF diameterPos = center + QPointF(0, -radiusMm - 15);
    drawDimensionText(painter, diameterPos, diameterText);

    painter->restore();
}

void DimensionRenderer::drawRectangleDimension(QPainter* painter, Geometry::Rectangle* rect)
{
    QPointF topLeft = rect->topLeft();
    double width = rect->width();
    double height = rect->height();

    QString widthText = Units::formatLength(std::abs(width), 1);
    QString heightText = Units::formatLength(std::abs(height), 1);

    // Draw width dimension at bottom
    QPointF bottomLeft = topLeft + QPointF(0, height);
    QPointF bottomRight = bottomLeft + QPointF(width, 0);
    drawDimensionLine(painter, bottomLeft, bottomRight, widthText, 25.0);

    // Draw height dimension at right
    QPointF topRight = topLeft + QPointF(width, 0);
    drawDimensionLine(painter, topRight, bottomRight, heightText, 25.0);
}

void DimensionRenderer::drawPolylineDimension(QPainter* painter, Geometry::Polyline* polyline)
{
    painter->save();

    auto vertices = polyline->vertices();
    if (vertices.size() < 2) {
        painter->restore();
        return;
    }

    // Calculate total perimeter
    double totalLength = 0.0;
    for (int i = 0; i < vertices.size(); ++i) {
        int nextIdx = (i + 1) % vertices.size();
        if (!polyline->isClosed() && nextIdx == 0) {
            break;
        }

        QPointF p1 = vertices[i].position;
        QPointF p2 = vertices[nextIdx].position;
        QPointF delta = p2 - p1;
        double segmentLength = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());
        totalLength += segmentLength;
    }

    // Draw total perimeter at centroid
    QPointF centroid(0, 0);
    for (const auto& vertex : vertices) {
        centroid += vertex.position;
    }
    centroid /= vertices.size();

    QString perimeterText = "Perimeter: " + Units::formatLength(totalLength, 1);
    drawDimensionText(painter, centroid + QPointF(0, -15), perimeterText);

    // Draw individual segment lengths (only for short polylines)
    if (vertices.size() <= 6) {
        QPen dimPen(QColor(200, 100, 0), 1);
        painter->setPen(dimPen);
        QFont font = painter->font();
        font.setPointSize(9);
        painter->setFont(font);

        for (int i = 0; i < vertices.size(); ++i) {
            int nextIdx = (i + 1) % vertices.size();
            if (!polyline->isClosed() && nextIdx == 0) {
                break;
            }

            QPointF p1 = vertices[i].position;
            QPointF p2 = vertices[nextIdx].position;
            QPointF delta = p2 - p1;
            double segmentLength = std::sqrt(delta.x() * delta.x() + delta.y() * delta.y());

            // Calculate midpoint and normal for text placement
            QPointF midpoint = (p1 + p2) / 2.0;
            QPointF normal(-delta.y(), delta.x());
            double normalLen = std::sqrt(normal.x() * normal.x() + normal.y() * normal.y());
            if (normalLen > 0.001) {
                normal /= normalLen;
            }

            QString lengthText = Units::formatLength(segmentLength, 1);
            drawDimensionText(painter, midpoint + normal * 10.0, lengthText);
        }
    }

    painter->restore();
}

void DimensionRenderer::drawDimensionLine(QPainter* painter, const QPointF& start,
                                          const QPointF& end, const QString& text, double offset)
{
    painter->save();

    // Calculate perpendicular offset direction
    QPointF delta = end - start;
    QPointF perpendicular(-delta.y(), delta.x());
    double length = std::sqrt(perpendicular.x() * perpendicular.x() +
                             perpendicular.y() * perpendicular.y());

    if (length < 0.001) {
        painter->restore();
        return;
    }

    perpendicular /= length;
    perpendicular *= offset;

    QPointF dimStart = start + perpendicular;
    QPointF dimEnd = end + perpendicular;

    // Draw dimension line
    QPen dimPen(QColor(200, 100, 0), 1);
    painter->setPen(dimPen);
    painter->drawLine(dimStart, dimEnd);

    // Draw extension lines
    QPen extPen(QColor(200, 100, 0), 1, Qt::DashLine);
    painter->setPen(extPen);
    painter->drawLine(start, dimStart + perpendicular * 0.3);
    painter->drawLine(end, dimEnd + perpendicular * 0.3);

    // Draw arrows
    double arrowSize = 8.0;
    QPointF direction = (dimEnd - dimStart);
    double dirLength = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (dirLength > 0.001) {
        direction /= dirLength;
    }

    QPointF arrowPerpendicular(-direction.y(), direction.x());

    // Start arrow
    painter->setBrush(QColor(200, 100, 0));
    QPolygonF startArrow;
    startArrow << dimStart
               << dimStart + direction * arrowSize - arrowPerpendicular * (arrowSize / 3)
               << dimStart + direction * arrowSize + arrowPerpendicular * (arrowSize / 3);
    painter->drawPolygon(startArrow);

    // End arrow
    QPolygonF endArrow;
    endArrow << dimEnd
             << dimEnd - direction * arrowSize - arrowPerpendicular * (arrowSize / 3)
             << dimEnd - direction * arrowSize + arrowPerpendicular * (arrowSize / 3);
    painter->drawPolygon(endArrow);

    // Draw text in the middle
    QPointF textPos = (dimStart + dimEnd) / 2.0;
    drawDimensionText(painter, textPos, text);

    painter->restore();
}

void DimensionRenderer::drawDimensionText(QPainter* painter, const QPointF& position,
                                          const QString& text)
{
    painter->save();

    // Set font
    QFont font = painter->font();
    font.setPointSize(10);
    font.setBold(true);
    painter->setFont(font);

    // Calculate text bounding box
    QFontMetrics metrics(font);
    QRect textRect = metrics.boundingRect(text);
    textRect.moveCenter(position.toPoint());

    // Draw background
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 220));
    painter->drawRect(textRect.adjusted(-3, -2, 3, 2));

    // Draw text
    painter->setPen(QColor(200, 100, 0));
    painter->drawText(textRect, Qt::AlignCenter, text);

    painter->restore();
}

} // namespace UI
} // namespace PatternCAD
