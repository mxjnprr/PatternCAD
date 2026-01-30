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
#include "geometry/Notch.h"
#include "geometry/MatchPoint.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDomDocument>
#include <QRegularExpression>
#include <cmath>

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
    return static_cast<FormatCapability>(
        static_cast<int>(FormatCapability::Import) |
        static_cast<int>(FormatCapability::Export)
    );
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

bool SVGFormat::importFile(const QString& filepath, Document* document)
{
    if (!document) {
        setError("Invalid document pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Read file
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(QString("Failed to open file for reading: %1").arg(file.errorString()));
        return false;
    }

    QString svgContent = file.readAll();
    file.close();

    reportProgress(50);

    // Parse SVG
    if (!parseSVG(svgContent, document)) {
        return false;
    }

    reportProgress(100);
    return true;
}

bool SVGFormat::parseSVG(const QString& svgContent, Document* document)
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;

    if (!doc.setContent(svgContent, &errorMsg, &errorLine, &errorColumn)) {
        setError(QString("Failed to parse SVG: %1 (line %2, column %3)")
                 .arg(errorMsg).arg(errorLine).arg(errorColumn));
        return false;
    }

    // Get root <svg> element
    QDomElement root = doc.documentElement();
    if (root.tagName() != "svg") {
        setError("Not a valid SVG file: root element is not <svg>");
        return false;
    }

    // Create an "Imported" layer
    QString layerName = "Imported";
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }

    // Parse all child elements
    QDomElement element = root.firstChildElement();
    while (!element.isNull()) {
        parseElement(element, document, layerName);
        element = element.nextSiblingElement();
    }

    return true;
}

void SVGFormat::parseElement(const QDomElement& element, Document* document, const QString& layerName)
{
    QString tagName = element.tagName();

    if (tagName == "g") {
        // Group - parse children recursively
        QString groupLayer = layerName;

        // Check for inkscape:label attribute
        if (element.hasAttribute("inkscape:label")) {
            groupLayer = element.attribute("inkscape:label");
            if (!document->layers().contains(groupLayer)) {
                document->addLayer(groupLayer);
            }
        }

        QDomElement child = element.firstChildElement();
        while (!child.isNull()) {
            parseElement(child, document, groupLayer);
            child = child.nextSiblingElement();
        }
    }
    else if (tagName == "path") {
        parsePath(element, document, layerName);
    }
    else if (tagName == "line") {
        parseLine(element, document, layerName);
    }
    else if (tagName == "circle") {
        parseCircle(element, document, layerName);
    }
    else if (tagName == "rect") {
        parseRect(element, document, layerName);
    }
    else if (tagName == "polyline") {
        parsePolyline(element, document, layerName);
    }
    else if (tagName == "polygon") {
        parsePolygon(element, document, layerName);
    }
    // Ignore: title, desc, defs, metadata, etc.
}

void SVGFormat::parsePath(const QDomElement& element, Document* document, const QString& layerName)
{
    if (!element.hasAttribute("d")) {
        return;
    }

    QString pathData = element.attribute("d");
    QVector<Geometry::PolylineVertex> vertices = parsePathData(pathData);

    if (vertices.isEmpty()) {
        return;
    }

    // Create polyline
    auto* polyline = new Geometry::Polyline(vertices);
    polyline->setLayer(layerName);
    document->addObjectDirect(polyline);
}

void SVGFormat::parseLine(const QDomElement& element, Document* document, const QString& layerName)
{
    if (!element.hasAttribute("x1") || !element.hasAttribute("y1") ||
        !element.hasAttribute("x2") || !element.hasAttribute("y2")) {
        return;
    }

    double x1 = element.attribute("x1").toDouble();
    double y1 = element.attribute("y1").toDouble();
    double x2 = element.attribute("x2").toDouble();
    double y2 = element.attribute("y2").toDouble();

    auto* line = new Geometry::Line(QPointF(x1, y1), QPointF(x2, y2));
    line->setLayer(layerName);
    document->addObjectDirect(line);
}

void SVGFormat::parseCircle(const QDomElement& element, Document* document, const QString& layerName)
{
    if (!element.hasAttribute("cx") || !element.hasAttribute("cy") || !element.hasAttribute("r")) {
        return;
    }

    double cx = element.attribute("cx").toDouble();
    double cy = element.attribute("cy").toDouble();
    double r = element.attribute("r").toDouble();

    auto* circle = new Geometry::Circle(QPointF(cx, cy), r);
    circle->setLayer(layerName);
    document->addObjectDirect(circle);
}

void SVGFormat::parseRect(const QDomElement& element, Document* document, const QString& layerName)
{
    if (!element.hasAttribute("x") || !element.hasAttribute("y") ||
        !element.hasAttribute("width") || !element.hasAttribute("height")) {
        return;
    }

    double x = element.attribute("x").toDouble();
    double y = element.attribute("y").toDouble();
    double width = element.attribute("width").toDouble();
    double height = element.attribute("height").toDouble();

    auto* rect = new Geometry::Rectangle(QPointF(x, y), width, height);
    rect->setLayer(layerName);
    document->addObjectDirect(rect);
}

void SVGFormat::parsePolyline(const QDomElement& element, Document* document, const QString& layerName)
{
    if (!element.hasAttribute("points")) {
        return;
    }

    QString pointsStr = element.attribute("points");
    QStringList coords = pointsStr.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);

    QVector<Geometry::PolylineVertex> vertices;
    for (int i = 0; i + 1 < coords.size(); i += 2) {
        double x = coords[i].toDouble();
        double y = coords[i + 1].toDouble();
        vertices.append(Geometry::PolylineVertex(QPointF(x, y), Geometry::VertexType::Sharp));
    }

    if (vertices.size() < 2) {
        return;
    }

    auto* polyline = new Geometry::Polyline(vertices);
    polyline->setClosed(false);
    polyline->setLayer(layerName);
    document->addObjectDirect(polyline);
}

void SVGFormat::parsePolygon(const QDomElement& element, Document* document, const QString& layerName)
{
    if (!element.hasAttribute("points")) {
        return;
    }

    QString pointsStr = element.attribute("points");
    QStringList coords = pointsStr.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);

    QVector<Geometry::PolylineVertex> vertices;
    for (int i = 0; i + 1 < coords.size(); i += 2) {
        double x = coords[i].toDouble();
        double y = coords[i + 1].toDouble();
        vertices.append(Geometry::PolylineVertex(QPointF(x, y), Geometry::VertexType::Sharp));
    }

    if (vertices.size() < 3) {
        return;
    }

    auto* polyline = new Geometry::Polyline(vertices);
    polyline->setClosed(true);
    polyline->setLayer(layerName);
    document->addObjectDirect(polyline);
}

QVector<Geometry::PolylineVertex> SVGFormat::parsePathData(const QString& pathData)
{
    QVector<Geometry::PolylineVertex> vertices;

    // Current position
    QPointF currentPos(0, 0);
    QPointF startPos(0, 0);
    QPointF lastControl(0, 0); // For smooth curves

    // Parse path commands
    QRegularExpression cmdRegex("([MmLlHhVvCcSsQqTtAaZz])");
    QRegularExpressionMatchIterator it = cmdRegex.globalMatch(pathData);

    int lastPos = 0;
    char lastCmd = 'M';

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int cmdPos = match.capturedStart();

        // Get parameters for previous command
        if (lastPos < cmdPos) {
            QString params = pathData.mid(lastPos, cmdPos - lastPos).trimmed();
            QStringList numbers = params.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);

            // Process command with parameters
            if (lastCmd == 'M' || lastCmd == 'm') {
                // MoveTo
                for (int i = 0; i + 1 < numbers.size(); i += 2) {
                    double x = numbers[i].toDouble();
                    double y = numbers[i + 1].toDouble();

                    if (lastCmd == 'm' && i > 0) {
                        // Relative (subsequent points are implicit linetos)
                        currentPos += QPointF(x, y);
                    } else if (lastCmd == 'm') {
                        // First point is relative
                        currentPos += QPointF(x, y);
                    } else {
                        // Absolute
                        currentPos = QPointF(x, y);
                    }

                    if (i == 0) {
                        startPos = currentPos;
                        vertices.append(Geometry::PolylineVertex(currentPos, Geometry::VertexType::Sharp));
                    } else {
                        // Implicit lineto
                        vertices.append(Geometry::PolylineVertex(currentPos, Geometry::VertexType::Sharp));
                    }
                }
            }
            else if (lastCmd == 'L' || lastCmd == 'l') {
                // LineTo
                for (int i = 0; i + 1 < numbers.size(); i += 2) {
                    double x = numbers[i].toDouble();
                    double y = numbers[i + 1].toDouble();

                    if (lastCmd == 'l') {
                        currentPos += QPointF(x, y);
                    } else {
                        currentPos = QPointF(x, y);
                    }

                    vertices.append(Geometry::PolylineVertex(currentPos, Geometry::VertexType::Sharp));
                }
            }
            else if (lastCmd == 'H' || lastCmd == 'h') {
                // Horizontal LineTo
                for (const QString& num : numbers) {
                    double x = num.toDouble();
                    if (lastCmd == 'h') {
                        currentPos.rx() += x;
                    } else {
                        currentPos.setX(x);
                    }
                    vertices.append(Geometry::PolylineVertex(currentPos, Geometry::VertexType::Sharp));
                }
            }
            else if (lastCmd == 'V' || lastCmd == 'v') {
                // Vertical LineTo
                for (const QString& num : numbers) {
                    double y = num.toDouble();
                    if (lastCmd == 'v') {
                        currentPos.ry() += y;
                    } else {
                        currentPos.setY(y);
                    }
                    vertices.append(Geometry::PolylineVertex(currentPos, Geometry::VertexType::Sharp));
                }
            }
            else if (lastCmd == 'C' || lastCmd == 'c') {
                // Cubic Bezier
                for (int i = 0; i + 5 < numbers.size(); i += 6) {
                    double x1 = numbers[i].toDouble();
                    double y1 = numbers[i + 1].toDouble();
                    double x2 = numbers[i + 2].toDouble();
                    double y2 = numbers[i + 3].toDouble();
                    double x = numbers[i + 4].toDouble();
                    double y = numbers[i + 5].toDouble();

                    QPointF c1, c2, endPt;
                    if (lastCmd == 'c') {
                        c1 = currentPos + QPointF(x1, y1);
                        c2 = currentPos + QPointF(x2, y2);
                        endPt = currentPos + QPointF(x, y);
                    } else {
                        c1 = QPointF(x1, y1);
                        c2 = QPointF(x2, y2);
                        endPt = QPointF(x, y);
                    }

                    // Calculate tangent from control points
                    QPointF tangent = (c2 - endPt);
                    double len = std::sqrt(tangent.x() * tangent.x() + tangent.y() * tangent.y());
                    if (len > 0.0001) {
                        tangent = QPointF(-tangent.x() / len, -tangent.y() / len);
                    }

                    currentPos = endPt;
                    lastControl = c2;

                    vertices.append(Geometry::PolylineVertex(currentPos, Geometry::VertexType::Smooth,
                                                            0.5, 0.5, tangent));
                }
            }
            else if (lastCmd == 'Z' || lastCmd == 'z') {
                // Close path - handled after loop
            }
        }

        lastCmd = match.captured(1)[0].toLatin1();
        lastPos = match.capturedEnd();
    }

    // Process final command parameters
    if (lastPos < pathData.length()) {
        QString params = pathData.mid(lastPos).trimmed();
        // Similar processing as above...
    }

    return vertices;
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

        // Export notches
        for (const Notch* notch : polyline->notches()) {
            out << notchToSVG(notch, indent);
        }

        // Export match points
        for (const MatchPoint* mp : polyline->matchPoints()) {
            out << matchPointToSVG(mp, indent);
        }
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

QString SVGFormat::notchToSVG(const Notch* notch, int indent) const
{
    if (!notch) return "";

    QString svg;
    QTextStream out(&svg);
    QString indentStr = formatIndent(indent);

    QPointF pos = notch->getLocation();
    QPointF normal = notch->getNormal();
    double depth = notch->depth();
    NotchStyle style = notch->style();

    QString notchStyle = "fill:none;stroke:#0000FF;stroke-width:0.5";  // Blue for notches

    switch (style) {
        case NotchStyle::VNotch: {
            // V-notch: two lines forming a V
            QPointF perpendicular(-normal.y(), normal.x());
            QPointF tip = pos + normal * depth;
            QPointF left = pos - perpendicular * (depth * 0.5);
            QPointF right = pos + perpendicular * (depth * 0.5);
            
            out << indentStr << QString("<path d=\"M %1,%2 L %3,%4 L %5,%6\" style=\"%7\" />\n")
                   .arg(left.x()).arg(left.y())
                   .arg(tip.x()).arg(tip.y())
                   .arg(right.x()).arg(right.y())
                   .arg(notchStyle);
            break;
        }
        case NotchStyle::Slit: {
            // Slit: single line perpendicular to edge
            QPointF end = pos + normal * depth;
            out << indentStr << QString("<line x1=\"%1\" y1=\"%2\" x2=\"%3\" y2=\"%4\" style=\"%5\" />\n")
                   .arg(pos.x()).arg(pos.y())
                   .arg(end.x()).arg(end.y())
                   .arg(notchStyle);
            break;
        }
        case NotchStyle::Dot: {
            // Dot: small circle
            double radius = depth * 0.3;
            out << indentStr << QString("<circle cx=\"%1\" cy=\"%2\" r=\"%3\" style=\"fill:#0000FF;stroke:none\" />\n")
                   .arg(pos.x()).arg(pos.y())
                   .arg(radius);
            break;
        }
    }

    return svg;
}

QString SVGFormat::matchPointToSVG(const MatchPoint* mp, int indent) const
{
    if (!mp) return "";

    QString svg;
    QTextStream out(&svg);
    QString indentStr = formatIndent(indent);

    QPointF pos = mp->position();
    QString label = mp->label();
    double crossSize = 3.0;  // Size of the cross marker

    QString mpStyle = "fill:none;stroke:#FF00FF;stroke-width:0.5";  // Magenta for match points

    // Draw cross
    out << indentStr << QString("<line x1=\"%1\" y1=\"%2\" x2=\"%3\" y2=\"%4\" style=\"%5\" />\n")
           .arg(pos.x() - crossSize).arg(pos.y())
           .arg(pos.x() + crossSize).arg(pos.y())
           .arg(mpStyle);
    out << indentStr << QString("<line x1=\"%1\" y1=\"%2\" x2=\"%3\" y2=\"%4\" style=\"%5\" />\n")
           .arg(pos.x()).arg(pos.y() - crossSize)
           .arg(pos.x()).arg(pos.y() + crossSize)
           .arg(mpStyle);

    // Draw label
    if (!label.isEmpty()) {
        out << indentStr << QString("<text x=\"%1\" y=\"%2\" font-size=\"4\" fill=\"#FF00FF\">%3</text>\n")
               .arg(pos.x() + crossSize + 1)
               .arg(pos.y() + 1.5)
               .arg(label);
    }

    return svg;
}

} // namespace IO
} // namespace PatternCAD

