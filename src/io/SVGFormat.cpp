/**
 * SVGFormat.cpp
 *
 * Implementation of SVGFormat
 */

#include "SVGFormat.h"
#include "core/Document.h"
#include "geometry/GeometryObject.h"
#include "geometry/Point2D.h"
#include "geometry/Line.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "geometry/Polyline.h"
#include "geometry/CubicBezier.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

namespace PatternCAD {
namespace IO {

SVGFormat::SVGFormat(QObject* parent)
    : FileFormat(parent)
{
}

SVGFormat::~SVGFormat()
{
}

QString SVGFormat::formatName() const
{
    return "SVG";
}

QString SVGFormat::formatDescription() const
{
    return "Scalable Vector Graphics";
}

QStringList SVGFormat::fileExtensions() const
{
    return QStringList() << "svg";
}

FormatType SVGFormat::formatType() const
{
    return FormatType::SVG;
}

FormatCapability SVGFormat::capabilities() const
{
    return FormatCapability::Export;
}

bool SVGFormat::exportFile(const QString& filepath, const Document* document)
{
    if (!document) {
        setError("Invalid document pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Generate SVG content
    QString svgContent = generateSVG(document);
    reportProgress(50);

    // Write to file
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(QString("Failed to open file for writing: %1").arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << svgContent;
    file.close();

    reportProgress(100);
    return true;
}

QString SVGFormat::generateSVG(const Document* document) const
{
    QString svg;
    QTextStream out(&svg);

    // Calculate bounds
    QRectF bounds = calculateBounds(document);
    if (bounds.isNull() || bounds.isEmpty()) {
        bounds = QRectF(0, 0, 100, 100); // Default size
    }

    // Add margins
    double margin = 10.0;
    bounds = bounds.adjusted(-margin, -margin, margin, margin);

    // SVG header
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    out << "<!-- Created with PatternCAD on " << QDateTime::currentDateTime().toString(Qt::ISODate) << " -->\n";
    out << "<svg\n";
    out << "   xmlns=\"http://www.w3.org/2000/svg\"\n";
    out << "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n";
    out << "   version=\"1.1\"\n";
    out << QString("   width=\"%1mm\"\n").arg(bounds.width());
    out << QString("   height=\"%1mm\"\n").arg(bounds.height());
    out << QString("   viewBox=\"%1 %2 %3 %4\">\n\n")
           .arg(bounds.x())
           .arg(bounds.y())
           .arg(bounds.width())
           .arg(bounds.height());

    // Title and description
    out << formatIndent(1) << "<title>" << document->name() << "</title>\n";
    out << formatIndent(1) << "<desc>PatternCAD document exported to SVG</desc>\n\n";

    // Export layers as groups
    QStringList layers = document->layers();
    for (const QString& layerName : layers) {
        bool layerVisible = document->isLayerVisible(layerName);

        // Start layer group
        out << formatIndent(1) << "<g\n";
        out << formatIndent(2) << QString("id=\"layer_%1\"\n").arg(layerName.toLower().replace(' ', '_'));
        out << formatIndent(2) << QString("inkscape:label=\"%1\"\n").arg(layerName);
        out << formatIndent(2) << QString("inkscape:groupmode=\"layer\"\n");
        if (!layerVisible) {
            out << formatIndent(2) << "style=\"display:none\"\n";
        }
        out << formatIndent(2) << ">\n";

        // Export objects in this layer
        for (const auto* obj : document->objects()) {
            if (obj->layer() == layerName && obj->isVisible()) {
                out << geometryToSVG(obj, 2);
            }
        }

        // Close layer group
        out << formatIndent(1) << "</g>\n\n";
    }

    // SVG footer
    out << "</svg>\n";

    return svg;
}

QString SVGFormat::geometryToSVG(const Geometry::GeometryObject* object, int indent) const
{
    QString svg;
    QTextStream out(&svg);
    QString indentStr = formatIndent(indent);

    // Common style
    QString style = QString("fill:none;stroke:#000000;stroke-width:0.5");

    if (auto* point = dynamic_cast<const Geometry::Point2D*>(object)) {
        // Point as small circle
        QPointF pos = point->position();
        out << indentStr << QString("<circle cx=\"%1\" cy=\"%2\" r=\"1.5\" style=\"fill:#000000;stroke:none\" />\n")
               .arg(pos.x()).arg(pos.y());
    }
    else if (auto* line = dynamic_cast<const Geometry::Line*>(object)) {
        // Line element
        QPointF start = line->start();
        QPointF end = line->end();
        out << indentStr << QString("<line x1=\"%1\" y1=\"%2\" x2=\"%3\" y2=\"%4\" style=\"%5\" />\n")
               .arg(start.x()).arg(start.y())
               .arg(end.x()).arg(end.y())
               .arg(style);
    }
    else if (auto* circle = dynamic_cast<const Geometry::Circle*>(object)) {
        // Circle element
        QPointF center = circle->center();
        double radius = circle->radius();
        out << indentStr << QString("<circle cx=\"%1\" cy=\"%2\" r=\"%3\" style=\"%4\" />\n")
               .arg(center.x()).arg(center.y())
               .arg(radius)
               .arg(style);
    }
    else if (auto* rect = dynamic_cast<const Geometry::Rectangle*>(object)) {
        // Rectangle element
        QPointF topLeft = rect->topLeft();
        double width = rect->width();
        double height = rect->height();
        out << indentStr << QString("<rect x=\"%1\" y=\"%2\" width=\"%3\" height=\"%4\" style=\"%5\" />\n")
               .arg(topLeft.x()).arg(topLeft.y())
               .arg(width).arg(height)
               .arg(style);
    }
    else if (auto* polyline = dynamic_cast<const Geometry::Polyline*>(object)) {
        // Polyline as path
        const QVector<Geometry::PolylineVertex>& vertices = polyline->vertices();
        if (vertices.isEmpty()) {
            return "";
        }

        QString pathData;
        QTextStream pathStream(&pathData);

        // Start point
        const Geometry::PolylineVertex& first = vertices.first();
        pathStream << QString("M %1,%2").arg(first.position.x()).arg(first.position.y());

        // Build path with cubic bezier curves
        for (int i = 1; i < vertices.size(); ++i) {
            const Geometry::PolylineVertex& prev = vertices[i - 1];
            const Geometry::PolylineVertex& curr = vertices[i];

            if (prev.type == Geometry::VertexType::Smooth || curr.type == Geometry::VertexType::Smooth) {
                // Use cubic bezier - same algorithm as Polyline::createPath()
                QPointF p1 = prev.position;
                QPointF p2 = curr.position;

                // Calculate control points
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

                pathStream << QString(" C %1,%2 %3,%4 %5,%6")
                              .arg(c1.x()).arg(c1.y())
                              .arg(c2.x()).arg(c2.y())
                              .arg(p2.x()).arg(p2.y());
            } else {
                // Straight line
                pathStream << QString(" L %1,%2").arg(curr.position.x()).arg(curr.position.y());
            }
        }

        // Close path if needed
        if (polyline->isClosed()) {
            pathStream << " Z";
        }

        out << indentStr << QString("<path d=\"%1\" style=\"%2\" />\n")
               .arg(pathData)
               .arg(style);
    }
    else if (auto* bezier = dynamic_cast<const Geometry::CubicBezier*>(object)) {
        // Cubic bezier as path
        QPointF p0 = bezier->p0();
        QPointF p1 = bezier->p1();
        QPointF p2 = bezier->p2();
        QPointF p3 = bezier->p3();

        QString pathData = QString("M %1,%2 C %3,%4 %5,%6 %7,%8")
                              .arg(p0.x()).arg(p0.y())
                              .arg(p1.x()).arg(p1.y())
                              .arg(p2.x()).arg(p2.y())
                              .arg(p3.x()).arg(p3.y());

        out << indentStr << QString("<path d=\"%1\" style=\"%2\" />\n")
               .arg(pathData)
               .arg(style);
    }

    return svg;
}

QString SVGFormat::formatIndent(int level) const
{
    return QString(level * 2, ' ');
}

QRectF SVGFormat::calculateBounds(const Document* document) const
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
