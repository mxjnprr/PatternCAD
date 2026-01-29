/**
 * DXFFormat.cpp
 *
 * Implementation of DXFFormat
 */

#include "DXFFormat.h"
#include "core/Document.h"
#include "geometry/GeometryObject.h"
#include "geometry/Point2D.h"
#include "geometry/Line.h"
#include "geometry/Circle.h"
#include "geometry/Polyline.h"
#include "geometry/Rectangle.h"
#include "geometry/CubicBezier.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <cmath>

namespace PatternCAD {
namespace IO {

DXFFormat::DXFFormat(QObject* parent)
    : FileFormat(parent)
{
}

DXFFormat::~DXFFormat()
{
}

QString DXFFormat::formatName() const
{
    return "DXF";
}

QString DXFFormat::formatDescription() const
{
    return "AutoCAD Drawing Exchange Format";
}

QStringList DXFFormat::fileExtensions() const
{
    return QStringList() << "dxf";
}

FormatType DXFFormat::formatType() const
{
    return FormatType::DXF;
}

FormatCapability DXFFormat::capabilities() const
{
    return static_cast<FormatCapability>(
        static_cast<int>(FormatCapability::Import) |
        static_cast<int>(FormatCapability::Export)
    );
}

bool DXFFormat::importFile(const QString& filepath, Document* document)
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

    QTextStream stream(&file);
    reportProgress(30);

    // Parse DXF
    if (!parseDXF(stream, document)) {
        file.close();
        return false;
    }

    file.close();
    reportProgress(100);
    return true;
}

bool DXFFormat::exportFile(const QString& filepath, const Document* document)
{
    if (!document) {
        setError("Invalid document pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Open file for writing
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(QString("Failed to open file for writing: %1").arg(file.errorString()));
        return false;
    }

    QTextStream stream(&file);
    reportProgress(10);

    // Write DXF sections
    writeHeader(stream);
    reportProgress(30);

    writeTables(stream, document);
    reportProgress(50);

    writeEntities(stream, document);
    reportProgress(90);

    // Write EOF
    writePair(stream, 0, "EOF");

    file.close();
    reportProgress(100);
    return true;
}

QString DXFFormat::readPair(QTextStream& stream, int& code)
{
    QString codeLine = stream.readLine().trimmed();
    QString valueLine = stream.readLine().trimmed();
    
    code = codeLine.toInt();
    return valueLine;
}

bool DXFFormat::parseDXF(QTextStream& stream, Document* document)
{
    bool inEntitiesSection = false;
    bool inBlocksSection = false;
    QString currentEntityType;
    DXFEntity currentEntity;
    QMap<QString, QList<DXFEntity>> blocks; // Store blocks for INSERT expansion
    QString currentBlockName;

    // POLYLINE handling
    bool inPolyline = false;
    DXFEntity polylineEntity;
    QList<DXFEntity> vertexEntities;

    qDebug() << "DXF: Starting parse";

    while (!stream.atEnd()) {
        int code;
        QString value = readPair(stream, code);

        if (code == 0) {
            // Code 0 marks entity boundaries
            if (value == "SECTION") {
                // Start of section
                int nextCode;
                QString sectionName = readPair(stream, nextCode);
                qDebug() << "DXF: Found SECTION" << sectionName;
                if (nextCode == 2 && sectionName == "ENTITIES") {
                    inEntitiesSection = true;
                    qDebug() << "DXF: Entering ENTITIES section";
                }
                else if (nextCode == 2 && sectionName == "BLOCKS") {
                    inBlocksSection = true;
                    qDebug() << "DXF: Entering BLOCKS section";
                }
            }
            else if (value == "ENDSEC") {
                // End of section - process last entity if any
                qDebug() << "DXF: Found ENDSEC";
                if (inEntitiesSection && !currentEntityType.isEmpty()) {
                    currentEntity.type = currentEntityType;
                    processEntity(currentEntity, document);
                    currentEntityType.clear();
                    currentEntity = DXFEntity();
                }
                if (inBlocksSection && !currentEntityType.isEmpty()) {
                    currentEntity.type = currentEntityType;
                    if (!currentBlockName.isEmpty()) {
                        blocks[currentBlockName].append(currentEntity);
                        qDebug() << "DXF: Added" << currentEntityType << "to block" << currentBlockName;
                    }
                    currentEntityType.clear();
                    currentEntity = DXFEntity();
                }
                inEntitiesSection = false;
                inBlocksSection = false;
            }
            else if (value == "EOF") {
                // End of file
                qDebug() << "DXF: Found EOF";
                break;
            }
            else if (value == "BLOCK") {
                // Start of block definition
                if (inBlocksSection) {
                    // Read block name from next attributes
                    currentEntityType = "BLOCK";
                    currentEntity = DXFEntity();
                }
            }
            else if (value == "ENDBLK") {
                // End of block definition
                if (inBlocksSection) {
                    currentBlockName.clear();
                }
            }
            else if (inEntitiesSection) {
                // Special handling for POLYLINE/VERTEX/SEQEND sequence
                if (value == "POLYLINE") {
                    // Process previous entity if any
                    if (!currentEntityType.isEmpty() && !inPolyline) {
                        currentEntity.type = currentEntityType;
                        processEntity(currentEntity, document);
                    }
                    // Start POLYLINE collection
                    inPolyline = true;
                    polylineEntity = DXFEntity();
                    vertexEntities.clear();
                    currentEntityType = value;
                    currentEntity = DXFEntity();
                }
                else if (value == "VERTEX" && inPolyline) {
                    // Save previous vertex if any
                    if (currentEntityType == "VERTEX") {
                        currentEntity.type = "VERTEX";
                        vertexEntities.append(currentEntity);
                        qDebug() << "DXF: Stored VERTEX" << vertexEntities.size();
                    }
                    // Start new vertex
                    currentEntityType = value;
                    currentEntity = DXFEntity();
                }
                else if (value == "SEQEND" && inPolyline) {
                    // Save last vertex if any
                    if (currentEntityType == "VERTEX") {
                        currentEntity.type = "VERTEX";
                        vertexEntities.append(currentEntity);
                        qDebug() << "DXF: Stored last VERTEX" << vertexEntities.size();
                    }
                    // End of POLYLINE - process it
                    qDebug() << "DXF: Processing POLYLINE with" << vertexEntities.size() << "vertices";
                    processPolyline(polylineEntity, vertexEntities, document);
                    inPolyline = false;
                    polylineEntity = DXFEntity();
                    vertexEntities.clear();
                    currentEntityType.clear();
                    currentEntity = DXFEntity();
                }
                else if (!inPolyline) {
                    // Process previous entity if any
                    if (!currentEntityType.isEmpty()) {
                        currentEntity.type = currentEntityType;
                        processEntity(currentEntity, document);
                    }
                    // Start new entity
                    currentEntityType = value;
                    currentEntity = DXFEntity();
                }
            }
            else if (inBlocksSection && !currentEntityType.isEmpty()) {
                // Process previous entity in block
                if (currentEntityType != "BLOCK") {
                    currentEntity.type = currentEntityType;
                    if (!currentBlockName.isEmpty()) {
                        blocks[currentBlockName].append(currentEntity);
                        qDebug() << "DXF: Added" << currentEntityType << "to block" << currentBlockName;
                    }
                }
                // Start new entity in block
                currentEntityType = value;
                currentEntity = DXFEntity();
            }
        }
        else if (inEntitiesSection && !currentEntityType.isEmpty()) {
            // Accumulate entity attributes
            if (inPolyline) {
                if (currentEntityType == "POLYLINE") {
                    // Accumulate POLYLINE header attributes
                    if (code == 8) {
                        polylineEntity.layer = value;
                    }
                    polylineEntity.attributes.insert(code, value);
                }
                else if (currentEntityType == "VERTEX") {
                    // Accumulate VERTEX attributes
                    if (code == 8) {
                        currentEntity.layer = value;
                    }
                    currentEntity.attributes.insert(code, value);

                    // When we hit code 0 next, this vertex is complete
                    // But we're still reading, so we need to check in next iteration
                }
            }
            else {
                // Normal entity attribute accumulation
                if (code == 8) {
                    currentEntity.layer = value;
                }
                currentEntity.attributes.insert(code, value);
            }
        }
        else if (inBlocksSection) {
            // Accumulate block data
            if (code == 2 && currentEntityType == "BLOCK") {
                // Block name
                currentBlockName = value;
                qDebug() << "DXF: Starting block definition:" << currentBlockName;
            }
            else if (!currentEntityType.isEmpty() && currentEntityType != "BLOCK") {
                // Entity attribute in block
                if (code == 8) {
                    currentEntity.layer = value;
                }
                currentEntity.attributes.insert(code, value);
            }
        }
    }

    // Process last entity
    if (inEntitiesSection && !currentEntityType.isEmpty()) {
        currentEntity.type = currentEntityType;
        processEntity(currentEntity, document);
    }

    qDebug() << "DXF: Parse complete. Document has" << document->objects().size() << "objects";
    qDebug() << "DXF: Layers:" << document->layers();

    return true;
}

void DXFFormat::processEntity(const DXFEntity& entity, Document* document)
{
    qDebug() << "DXF: Processing entity type:" << entity.type << "layer:" << entity.layer;

    if (entity.type == "LINE") {
        processLine(entity, document);
    }
    else if (entity.type == "CIRCLE") {
        processCircle(entity, document);
    }
    else if (entity.type == "ARC") {
        processArc(entity, document);
    }
    else if (entity.type == "POINT") {
        processPoint(entity, document);
    }
    else if (entity.type == "LWPOLYLINE") {
        processLWPolyline(entity, document);
    }
    else if (entity.type == "INSERT") {
        // INSERT entities reference blocks - we skip them for now
        qDebug() << "DXF: Skipping INSERT entity (block references not yet supported)";
    }
    else if (entity.type == "POLYLINE") {
        // POLYLINE handled separately in parseDXF
        qDebug() << "DXF: POLYLINE should be handled with vertices, not here";
    }
    else if (entity.type == "VERTEX" || entity.type == "SEQEND") {
        // These are handled as part of POLYLINE
    }
    else {
        qDebug() << "DXF: Unsupported entity type:" << entity.type;
    }
}

void DXFFormat::processPolyline(const DXFEntity& polylineEntity, const QList<DXFEntity>& vertices, Document* document)
{
    if (vertices.isEmpty()) {
        qDebug() << "DXF: POLYLINE has no vertices, skipping";
        return;
    }

    QVector<Geometry::PolylineVertex> polylineVertices;

    // Extract coordinates from VERTEX entities
    for (const DXFEntity& vertex : vertices) {
        double x = getDouble(vertex, 10);
        double y = getDouble(vertex, 20);
        polylineVertices.append(Geometry::PolylineVertex(QPointF(x, y), Geometry::VertexType::Sharp));
    }

    if (polylineVertices.size() < 2) {
        qDebug() << "DXF: POLYLINE has less than 2 valid vertices, skipping";
        return;
    }

    // Code 70: polyline flags (1 = closed)
    QList<QString> flagsList = polylineEntity.attributes.values(70);
    int flags = flagsList.isEmpty() ? 0 : flagsList.first().toInt();
    bool closed = (flags & 1) != 0;

    auto* polyline = new Geometry::Polyline(polylineVertices);
    polyline->setClosed(closed);

    QString layerName = polylineEntity.layer.isEmpty() ? "Imported" : polylineEntity.layer;
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }
    polyline->setLayer(layerName);

    document->addObjectDirect(polyline);
    qDebug() << "DXF: POLYLINE added with" << polylineVertices.size() << "vertices, closed:" << closed
             << "layer:" << layerName << "document now has" << document->objects().size() << "objects";
}

void DXFFormat::processLine(const DXFEntity& entity, Document* document)
{
    double x1 = getDouble(entity, 10);
    double y1 = getDouble(entity, 20);
    double x2 = getDouble(entity, 11);
    double y2 = getDouble(entity, 21);

    qDebug() << "DXF: Creating LINE from (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ") on layer" << entity.layer;

    auto* line = new Geometry::Line(QPointF(x1, y1), QPointF(x2, y2));

    QString layerName = entity.layer.isEmpty() ? "Imported" : entity.layer;
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }
    line->setLayer(layerName);

    document->addObjectDirect(line);
    qDebug() << "DXF: LINE added, document now has" << document->objects().size() << "objects";
}

void DXFFormat::processCircle(const DXFEntity& entity, Document* document)
{
    double cx = getDouble(entity, 10);
    double cy = getDouble(entity, 20);
    double radius = getDouble(entity, 40);
    
    auto* circle = new Geometry::Circle(QPointF(cx, cy), radius);
    
    QString layerName = entity.layer.isEmpty() ? "Imported" : entity.layer;
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }
    circle->setLayer(layerName);
    
    document->addObjectDirect(circle);
}

void DXFFormat::processArc(const DXFEntity& entity, Document* document)
{
    double cx = getDouble(entity, 10);
    double cy = getDouble(entity, 20);
    double radius = getDouble(entity, 40);
    double startAngle = getDouble(entity, 50); // degrees
    double endAngle = getDouble(entity, 51);   // degrees
    
    // Convert arc to polyline approximation
    QVector<Geometry::PolylineVertex> vertices;
    
    // Number of segments based on arc length
    double arcLength = std::abs(endAngle - startAngle);
    int numSegments = std::max(8, static_cast<int>(arcLength / 10.0));
    
    for (int i = 0; i <= numSegments; ++i) {
        double t = static_cast<double>(i) / numSegments;
        double angle = startAngle + t * (endAngle - startAngle);
        double radians = angle * M_PI / 180.0;
        
        double x = cx + radius * std::cos(radians);
        double y = cy + radius * std::sin(radians);
        
        vertices.append(Geometry::PolylineVertex(QPointF(x, y), Geometry::VertexType::Sharp));
    }
    
    auto* polyline = new Geometry::Polyline(vertices);
    polyline->setClosed(false);
    
    QString layerName = entity.layer.isEmpty() ? "Imported" : entity.layer;
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }
    polyline->setLayer(layerName);
    
    document->addObjectDirect(polyline);
}

void DXFFormat::processLWPolyline(const DXFEntity& entity, Document* document)
{
    QVector<Geometry::PolylineVertex> vertices;

    // Code 70: polyline flags (1 = closed)
    QList<QString> flagsList = entity.attributes.values(70);
    int flags = flagsList.isEmpty() ? 0 : flagsList.first().toInt();
    bool closed = (flags & 1) != 0;

    // Collect all X and Y coordinates using QMultiMap::values()
    QList<QString> xValues = entity.attributes.values(10);
    QList<QString> yValues = entity.attributes.values(20);

    // Create vertices
    int count = std::min(xValues.size(), yValues.size());
    for (int i = 0; i < count; ++i) {
        double x = xValues[i].toDouble();
        double y = yValues[i].toDouble();
        vertices.append(Geometry::PolylineVertex(QPointF(x, y),
                                                  Geometry::VertexType::Sharp));
    }

    if (vertices.size() < 2) {
        return;
    }

    auto* polyline = new Geometry::Polyline(vertices);
    polyline->setClosed(closed);

    QString layerName = entity.layer.isEmpty() ? "Imported" : entity.layer;
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }
    polyline->setLayer(layerName);

    document->addObjectDirect(polyline);
}

void DXFFormat::processPoint(const DXFEntity& entity, Document* document)
{
    double x = getDouble(entity, 10);
    double y = getDouble(entity, 20);
    
    auto* point = new Geometry::Point2D(x, y);
    
    QString layerName = entity.layer.isEmpty() ? "Imported" : entity.layer;
    if (!document->layers().contains(layerName)) {
        document->addLayer(layerName);
    }
    point->setLayer(layerName);
    
    document->addObjectDirect(point);
}

double DXFFormat::getDouble(const DXFEntity& entity, int code, double defaultValue) const
{
    QList<QString> values = entity.attributes.values(code);
    if (values.isEmpty()) {
        return defaultValue;
    }
    return values.first().toDouble();
}

QString DXFFormat::getString(const DXFEntity& entity, int code, const QString& defaultValue) const
{
    QList<QString> values = entity.attributes.values(code);
    if (values.isEmpty()) {
        return defaultValue;
    }
    return values.first();
}

// ============================================================================
// Export helper methods
// ============================================================================

void DXFFormat::writePair(QTextStream& stream, int code, const QString& value) const
{
    stream << code << "\n" << value << "\n";
}

void DXFFormat::writePair(QTextStream& stream, int code, int value) const
{
    stream << code << "\n" << value << "\n";
}

void DXFFormat::writePair(QTextStream& stream, int code, double value) const
{
    stream << code << "\n" << value << "\n";
}

void DXFFormat::writeHeader(QTextStream& stream) const
{
    writePair(stream, 0, "SECTION");
    writePair(stream, 2, "HEADER");

    // Set units to millimeters (DXF code 4 = mm)
    writePair(stream, 9, "$INSUNITS");
    writePair(stream, 70, 4);

    // Set measurement system to metric
    writePair(stream, 9, "$MEASUREMENT");
    writePair(stream, 70, 1);

    writePair(stream, 0, "ENDSEC");
}

void DXFFormat::writeTables(QTextStream& stream, const Document* document) const
{
    writePair(stream, 0, "SECTION");
    writePair(stream, 2, "TABLES");

    // Write layer table
    writePair(stream, 0, "TABLE");
    writePair(stream, 2, "LAYER");

    // Get all unique layers from objects
    QSet<QString> layers;
    for (const auto* obj : document->objects()) {
        layers.insert(obj->layer());
    }

    // Write each layer definition
    for (const QString& layerName : layers) {
        writePair(stream, 0, "LAYER");
        writePair(stream, 2, layerName);
        writePair(stream, 70, 0);  // Standard flags

        // Get layer color from document
        QColor layerColor = document->layerColor(layerName);
        // Convert RGB to DXF color index (simplified - use color index 7 for white/default)
        int colorIndex = 7;  // Default white
        if (layerColor.isValid()) {
            // Simple color mapping (can be improved)
            if (layerColor == Qt::red) colorIndex = 1;
            else if (layerColor == Qt::yellow) colorIndex = 2;
            else if (layerColor == Qt::green) colorIndex = 3;
            else if (layerColor == Qt::cyan) colorIndex = 4;
            else if (layerColor == Qt::blue) colorIndex = 5;
            else if (layerColor == Qt::magenta) colorIndex = 6;
        }
        writePair(stream, 62, colorIndex);

        writePair(stream, 6, "CONTINUOUS");  // Linetype
    }

    writePair(stream, 0, "ENDTAB");
    writePair(stream, 0, "ENDSEC");
}

void DXFFormat::writeEntities(QTextStream& stream, const Document* document) const
{
    writePair(stream, 0, "SECTION");
    writePair(stream, 2, "ENTITIES");

    // Write each object based on its type
    for (const auto* obj : document->objects()) {
        if (!obj->isVisible()) {
            continue;  // Skip invisible objects
        }

        using namespace Geometry;
        switch (obj->type()) {
            case ObjectType::Line:
                writeLine(stream, obj);
                break;
            case ObjectType::Circle:
                writeCircle(stream, obj);
                break;
            case ObjectType::Polyline:
            case ObjectType::Polygon:
                writePolyline(stream, obj);
                break;
            case ObjectType::Rectangle:
                writeRectangle(stream, obj);
                break;
            case ObjectType::CubicBezier:
                writeCubicBezier(stream, obj);
                break;
            case ObjectType::Point:
                writePoint(stream, obj);
                break;
            case ObjectType::Arc:
                // Arc not yet implemented in geometry classes
                qWarning() << "DXFFormat::writeEntities - Arc export not yet supported";
                break;
            default:
                qWarning() << "DXFFormat::writeEntities - Unknown object type";
                break;
        }
    }

    writePair(stream, 0, "ENDSEC");
}

void DXFFormat::writeLine(QTextStream& stream, const Geometry::GeometryObject* obj) const
{
    const auto* line = dynamic_cast<const Geometry::Line*>(obj);
    if (!line) return;

    QPointF start = line->start();
    QPointF end = line->end();

    writePair(stream, 0, "LINE");
    writePair(stream, 8, obj->layer());  // Layer name
    writePair(stream, 10, start.x());    // Start X
    writePair(stream, 20, start.y());    // Start Y
    writePair(stream, 30, 0.0);          // Start Z
    writePair(stream, 11, end.x());      // End X
    writePair(stream, 21, end.y());      // End Y
    writePair(stream, 31, 0.0);          // End Z
}

void DXFFormat::writeCircle(QTextStream& stream, const Geometry::GeometryObject* obj) const
{
    const auto* circle = dynamic_cast<const Geometry::Circle*>(obj);
    if (!circle) return;

    QPointF center = circle->center();
    double radius = circle->radius();

    writePair(stream, 0, "CIRCLE");
    writePair(stream, 8, obj->layer());
    writePair(stream, 10, center.x());   // Center X
    writePair(stream, 20, center.y());   // Center Y
    writePair(stream, 30, 0.0);          // Center Z
    writePair(stream, 40, radius);       // Radius
}

void DXFFormat::writePolyline(QTextStream& stream, const Geometry::GeometryObject* obj) const
{
    const auto* polyline = dynamic_cast<const Geometry::Polyline*>(obj);
    if (!polyline) return;

    QVector<Geometry::PolylineVertex> vertices = polyline->vertices();
    if (vertices.isEmpty()) return;

    // Collect all points including curve approximations
    QList<QPointF> allPoints;
    int n = vertices.size();

    for (int i = 0; i < n; ++i) {
        int nextIdx = (i + 1) % n;

        // If we're at the last vertex and not closed, don't process to first
        if (!polyline->isClosed() && i == n - 1) {
            allPoints.append(vertices[i].position);
            break;
        }

        const Geometry::PolylineVertex& current = vertices[i];
        const Geometry::PolylineVertex& next = vertices[nextIdx];

        // Add current vertex
        allPoints.append(current.position);

        // Check if segment is curved
        bool needsCurve = (current.type == Geometry::VertexType::Smooth) ||
                          (next.type == Geometry::VertexType::Smooth);

        if (needsCurve) {
            QPointF p1 = current.position;
            QPointF p2 = next.position;

            // Distance between points for scaling control points
            QPointF segment = p2 - p1;
            double dist = std::sqrt(segment.x() * segment.x() + segment.y() * segment.y());
            double controlDistance = dist / 3.0;

            // Calculate control points for cubic Bezier
            QPointF c1, c2;

            // Determine control point c1 (outgoing from current)
            if (current.type == Geometry::VertexType::Smooth && current.tangent != QPointF()) {
                c1 = p1 + current.tangent * controlDistance * current.outgoingTension;
            } else if (current.type == Geometry::VertexType::Smooth) {
                int prevIdx = (i - 1 + n) % n;
                QPointF p0 = vertices[prevIdx].position;
                if (!polyline->isClosed() && i == 0) p0 = p1;
                c1 = p1 + (p2 - p0) * (current.outgoingTension / 3.0);
            } else {
                c1 = p1 + (p2 - p1) * 0.01;
            }

            // Determine control point c2 (incoming to next)
            if (next.type == Geometry::VertexType::Smooth && next.tangent != QPointF()) {
                c2 = p2 - next.tangent * controlDistance * next.incomingTension;
            } else if (next.type == Geometry::VertexType::Smooth) {
                int nextNextIdx = (i + 2) % n;
                QPointF p3 = vertices[nextNextIdx].position;
                if (!polyline->isClosed() && nextIdx == n - 1) p3 = p2;
                c2 = p2 - (p3 - p1) * (next.incomingTension / 3.0);
            } else {
                c2 = p2 - (p2 - p1) * 0.01;
            }

            // Approximate cubic Bezier with line segments
            const int segments = 10;
            for (int j = 1; j <= segments; ++j) {
                double t = static_cast<double>(j) / segments;
                double t2 = t * t;
                double t3 = t2 * t;
                double mt = 1.0 - t;
                double mt2 = mt * mt;
                double mt3 = mt2 * mt;

                QPointF point = p1 * mt3 + c1 * 3.0 * mt2 * t + c2 * 3.0 * mt * t2 + p2 * t3;

                // Don't add the last point as it will be added as the next vertex
                if (j < segments) {
                    allPoints.append(point);
                }
            }
        }
    }

    // Write as LWPOLYLINE
    writePair(stream, 0, "LWPOLYLINE");
    writePair(stream, 8, obj->layer());
    writePair(stream, 90, static_cast<int>(allPoints.size()));
    writePair(stream, 70, polyline->isClosed() ? 1 : 0);

    for (const QPointF& point : allPoints) {
        writePair(stream, 10, point.x());
        writePair(stream, 20, point.y());
    }
}

void DXFFormat::writeRectangle(QTextStream& stream, const Geometry::GeometryObject* obj) const
{
    const auto* rect = dynamic_cast<const Geometry::Rectangle*>(obj);
    if (!rect) return;

    // Get rectangle corners
    QPointF topLeft = rect->topLeft();
    QPointF bottomRight = rect->bottomRight();

    // Write as closed LWPOLYLINE
    writePair(stream, 0, "LWPOLYLINE");
    writePair(stream, 8, obj->layer());
    writePair(stream, 90, 4);  // 4 vertices
    writePair(stream, 70, 1);  // Closed

    // Top-left
    writePair(stream, 10, topLeft.x());
    writePair(stream, 20, topLeft.y());

    // Top-right
    writePair(stream, 10, bottomRight.x());
    writePair(stream, 20, topLeft.y());

    // Bottom-right
    writePair(stream, 10, bottomRight.x());
    writePair(stream, 20, bottomRight.y());

    // Bottom-left
    writePair(stream, 10, topLeft.x());
    writePair(stream, 20, bottomRight.y());
}

void DXFFormat::writeCubicBezier(QTextStream& stream, const Geometry::GeometryObject* obj) const
{
    const auto* bezier = dynamic_cast<const Geometry::CubicBezier*>(obj);
    if (!bezier) return;

    // For now, approximate Bezier curve with line segments
    // A proper implementation would use DXF SPLINE entity
    QList<QPointF> points;
    const int segments = 20;
    for (int i = 0; i <= segments; ++i) {
        double t = static_cast<double>(i) / segments;
        points.append(bezier->pointAt(t));
    }

    // Write as LWPOLYLINE
    writePair(stream, 0, "LWPOLYLINE");
    writePair(stream, 8, obj->layer());
    writePair(stream, 90, static_cast<int>(points.size()));
    writePair(stream, 70, 0);  // Not closed

    for (const QPointF& point : points) {
        writePair(stream, 10, point.x());
        writePair(stream, 20, point.y());
    }
}

void DXFFormat::writePoint(QTextStream& stream, const Geometry::GeometryObject* obj) const
{
    const auto* point = dynamic_cast<const Geometry::Point2D*>(obj);
    if (!point) return;

    QPointF pos = point->position();

    writePair(stream, 0, "POINT");
    writePair(stream, 8, obj->layer());
    writePair(stream, 10, pos.x());
    writePair(stream, 20, pos.y());
    writePair(stream, 30, 0.0);
}

} // namespace IO
} // namespace PatternCAD
