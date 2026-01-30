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
#include "geometry/Notch.h"
#include "geometry/MatchPoint.h"
#include "geometry/SeamAllowance.h"
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
                QPointF p1 = prev.position;
                QPointF p2 = curr.position;

                // Calculate control points - same algorithm as Polyline::createPath()
                QPointF segment = p2 - p1;
                double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
                double controlDistance = dist / 3.0;

                QPointF c1, c2;

                // Control point c1 (outgoing from prev)
                if (prev.type == Geometry::VertexType::Smooth && prev.tangent != QPointF()) {
                    c1 = p1 + prev.tangent * controlDistance * prev.outgoingTension;
                } else {
                    // Sharp: place control point very close to p1
                    c1 = p1 + (p2 - p1) * 0.01;
                }

                // Control point c2 (incoming to curr)
                if (curr.type == Geometry::VertexType::Smooth && curr.tangent != QPointF()) {
                    c2 = p2 - curr.tangent * controlDistance * curr.incomingTension;
                } else {
                    // Sharp: place control point very close to p2
                    c2 = p2 - (p2 - p1) * 0.01;
                }

                path.cubicTo(c1, c2, p2);
            } else {
                // Straight line
                path.lineTo(curr.position);
            }
        }

        if (polyline->isClosed()) {
            path.closeSubpath();
        }

        painter->drawPath(path);
        
        // Save current pen for restoration
        QPen savedPen = painter->pen();
        
        // Render notches
        for (const Notch* notch : polyline->notches()) {
            QPointF pos = notch->getLocation();
            QPointF normal = notch->getNormal();
            double depth = notch->depth();
            
            // Calculate perpendicular for V-notch (tangent direction)
            QPointF perp(-normal.y(), normal.x());
            double halfWidth = depth * 0.5;
            
            switch (notch->style()) {
                case NotchStyle::VNotch: {
                    QPointF tip = pos + normal * depth;
                    QPointF left = pos + perp * halfWidth;
                    QPointF right = pos - perp * halfWidth;
                    painter->drawLine(left, tip);
                    painter->drawLine(tip, right);
                    break;
                }
                case NotchStyle::Slit: {
                    painter->drawLine(pos, pos + normal * depth);
                    break;
                }
                case NotchStyle::Dot: {
                    painter->setBrush(Qt::black);
                    painter->drawEllipse(pos, depth * 0.3, depth * 0.3);
                    painter->setBrush(Qt::NoBrush);
                    break;
                }
            }
        }
        
        // Render match points
        for (const MatchPoint* mp : polyline->matchPoints()) {
            QPointF pos = mp->position();
            double size = 2.0;  // Cross size in mm
            
            // Draw cross
            painter->drawLine(pos - QPointF(size, 0), pos + QPointF(size, 0));
            painter->drawLine(pos - QPointF(0, size), pos + QPointF(0, size));
            
            // Draw label
            QString label = mp->label();
            if (!label.isEmpty()) {
                QFont font = painter->font();
                font.setPointSizeF(8);
                painter->setFont(font);
                painter->drawText(pos + QPointF(size + 1, -1), label);
            }
        }
        
        // Render seam allowance
        SeamAllowance* seam = polyline->seamAllowance();
        if (seam && seam->isEnabled()) {
            QPen seamPen(Qt::red);
            seamPen.setWidthF(0.3);
            seamPen.setStyle(Qt::DashLine);
            painter->setPen(seamPen);
            
            QVector<QVector<QPointF>> allOffsets = seam->computeAllOffsets();
            for (const QVector<QPointF>& offsetPoints : allOffsets) {
                if (offsetPoints.size() < 2) continue;
                
                QPainterPath seamPath;
                seamPath.moveTo(offsetPoints[0]);
                for (int i = 1; i < offsetPoints.size(); ++i) {
                    seamPath.lineTo(offsetPoints[i]);
                }
                seamPath.closeSubpath();
                painter->drawPath(seamPath);
            }
        }
        
        // Restore pen
        painter->setPen(savedPen);
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
