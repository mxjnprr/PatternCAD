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
    return FormatCapability::Import; // Export not yet implemented
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
    Q_UNUSED(filepath);
    Q_UNUSED(document);
    setError("DXF export not yet implemented");
    return false;
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
                // Process previous entity if any
                if (!currentEntityType.isEmpty()) {
                    currentEntity.type = currentEntityType;
                    processEntity(currentEntity, document);
                }
                // Start new entity
                currentEntityType = value;
                currentEntity = DXFEntity();
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
            if (code == 8) {
                currentEntity.layer = value;
            }
            currentEntity.attributes.insert(code, value);
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
        // They would need block expansion which is complex
        qDebug() << "DXF: Skipping INSERT entity (block references not yet supported)";
    }
    else {
        qDebug() << "DXF: Unsupported entity type:" << entity.type;
    }
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

} // namespace IO
} // namespace PatternCAD
