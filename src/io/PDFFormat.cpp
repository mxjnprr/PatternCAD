/**
 * PDFFormat.cpp
 *
 * Implementation of PDFFormat
 */

#include "PDFFormat.h"
#include "core/Document.h"
#include "geometry/GeometryObject.h"
#include "geometry/Point2D.h"
#include "geometry/Line.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "geometry/Polyline.h"
#include "geometry/CubicBezier.h"
#include <QPdfWriter>
#include <QPainter>
#include <QPageLayout>
#include <QDateTime>
#include <cmath>

namespace PatternCAD {
namespace IO {

PDFFormat::PDFFormat(QObject* parent)
    : FileFormat(parent)
    , m_pageSize(QPageSize::A4)
    , m_orientation(QPageLayout::Portrait)
{
}

PDFFormat::~PDFFormat()
{
}

QString PDFFormat::formatName() const
{
    return "PDF";
}

QString PDFFormat::formatDescription() const
{
    return "Portable Document Format";
}

QStringList PDFFormat::fileExtensions() const
{
    return QStringList() << "pdf";
}

FormatType PDFFormat::formatType() const
{
    return FormatType::PDF;
}

FormatCapability PDFFormat::capabilities() const
{
    return FormatCapability::Export;
}

bool PDFFormat::exportFile(const QString& filepath, const Document* document)
{
    if (!document) {
        setError("Invalid document pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Create PDF writer
    QPdfWriter pdfWriter(filepath);
    pdfWriter.setPageSize(QPageSize(m_pageSize));
    pdfWriter.setPageOrientation(m_orientation);
    pdfWriter.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    // Set PDF metadata
    pdfWriter.setTitle(document->name());
    pdfWriter.setCreator("PatternCAD");

    reportProgress(20);

    // Calculate pattern bounds
    QRectF bounds = calculateBounds(document);
    if (bounds.isNull() || bounds.isEmpty()) {
        bounds = QRectF(0, 0, 100, 100);
    }

    // Add margins
    double margin = 10.0;
    bounds = bounds.adjusted(-margin, -margin, margin, margin);

    // Create painter
    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);

    reportProgress(40);

    // Get page rectangle (in device coordinates)
    QRectF pageRect = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());

    // Calculate scaling to fit pattern on page
    double scaleX = pageRect.width() / bounds.width();
    double scaleY = pageRect.height() / bounds.height();
    double scale = std::min(scaleX, scaleY);

    // Apply transformation: translate to center and scale
    painter.translate(pageRect.center());
    painter.scale(scale, scale);
    painter.translate(-bounds.center());

    reportProgress(60);

    // Set pen style
    QPen pen(Qt::black);
    pen.setWidthF(0.5); // 0.5mm width
    pen.setCosmetic(false); // Scale with transformation
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    // Render all visible objects
    for (const auto* obj : document->objects()) {
        if (obj->isVisible()) {
            renderGeometry(&painter, obj);
        }
    }

    reportProgress(90);

    // Finish
    painter.end();

    reportProgress(100);
    return true;
}

void PDFFormat::renderGeometry(QPainter* painter, const Geometry::GeometryObject* object)
{
    if (auto* point = dynamic_cast<const Geometry::Point2D*>(object)) {
        // Point as small filled circle
        QPointF pos = point->position();
        painter->setBrush(Qt::black);
        painter->drawEllipse(pos, 1.5, 1.5);
        painter->setBrush(Qt::NoBrush);
    }
    else if (auto* line = dynamic_cast<const Geometry::Line*>(object)) {
        // Line
        painter->drawLine(line->start(), line->end());
    }
    else if (auto* circle = dynamic_cast<const Geometry::Circle*>(object)) {
        // Circle
        QPointF center = circle->center();
        double radius = circle->radius();
        painter->drawEllipse(center, radius, radius);
    }
    else if (auto* rect = dynamic_cast<const Geometry::Rectangle*>(object)) {
        // Rectangle
        painter->drawRect(QRectF(rect->topLeft(), QSizeF(rect->width(), rect->height())));
    }
    else if (auto* polyline = dynamic_cast<const Geometry::Polyline*>(object)) {
        // Polyline with bezier curves
        const QVector<Geometry::PolylineVertex>& vertices = polyline->vertices();
        if (vertices.isEmpty()) {
            return;
        }

        QPainterPath path;
        const Geometry::PolylineVertex& first = vertices.first();
        path.moveTo(first.position);

        for (int i = 1; i < vertices.size(); ++i) {
            const Geometry::PolylineVertex& prev = vertices[i - 1];
            const Geometry::PolylineVertex& curr = vertices[i];

            if (prev.type == Geometry::VertexType::Smooth || curr.type == Geometry::VertexType::Smooth) {
                // Cubic bezier
                QPointF p0 = prev.position;
                QPointF p3 = curr.position;

                // Calculate control points
                QPointF direction = p3 - p0;
                double distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

                // Normalize tangents
                QPointF prevTangent = prev.tangent;
                double prevLen = std::sqrt(prevTangent.x() * prevTangent.x() + prevTangent.y() * prevTangent.y());
                if (prevLen > 0.0001) {
                    prevTangent = QPointF(prevTangent.x() / prevLen, prevTangent.y() / prevLen);
                }

                QPointF currTangent = curr.tangent;
                double currLen = std::sqrt(currTangent.x() * currTangent.x() + currTangent.y() * currTangent.y());
                if (currLen > 0.0001) {
                    currTangent = QPointF(currTangent.x() / currLen, currTangent.y() / currLen);
                }

                QPointF p1 = p0 + prevTangent * distance * prev.outgoingTension;
                QPointF p2 = p3 - currTangent * distance * curr.incomingTension;

                path.cubicTo(p1, p2, p3);
            } else {
                // Straight line
                path.lineTo(curr.position);
            }
        }

        if (polyline->isClosed()) {
            path.closeSubpath();
        }

        painter->drawPath(path);
    }
    else if (auto* bezier = dynamic_cast<const Geometry::CubicBezier*>(object)) {
        // Cubic bezier
        QPainterPath path;
        path.moveTo(bezier->p0());
        path.cubicTo(bezier->p1(), bezier->p2(), bezier->p3());
        painter->drawPath(path);
    }
}

QRectF PDFFormat::calculateBounds(const Document* document) const
{
    QRectF bounds;

    for (const auto* obj : document->objects()) {
        if (!obj->isVisible()) {
            continue;
        }

        QRectF objBounds = obj->boundingRect();
        if (bounds.isNull()) {
            bounds = objBounds;
        } else {
            bounds = bounds.united(objBounds);
        }
    }

    return bounds;
}

} // namespace IO
} // namespace PatternCAD
