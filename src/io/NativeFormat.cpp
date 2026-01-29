/**
 * NativeFormat.cpp
 *
 * Implementation of NativeFormat
 */

#include "NativeFormat.h"
#include "core/Document.h"
#include "core/Project.h"
#include "geometry/GeometryObject.h"
#include "geometry/Point2D.h"
#include "geometry/Line.h"
#include "geometry/Circle.h"
#include "geometry/Rectangle.h"
#include "geometry/CubicBezier.h"
#include "geometry/Polyline.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QColor>

namespace PatternCAD {
namespace IO {

NativeFormat::NativeFormat(QObject* parent)
    : FileFormat(parent)
{
}

NativeFormat::~NativeFormat()
{
}

QString NativeFormat::formatName() const
{
    return "PatternCAD Native";
}

QString NativeFormat::formatDescription() const
{
    return "PatternCAD Files";
}

QStringList NativeFormat::fileExtensions() const
{
    return QStringList() << "patterncad" << "pcad";
}

FormatType NativeFormat::formatType() const
{
    return FormatType::Native;
}

FormatCapability NativeFormat::capabilities() const
{
    return FormatCapability::ImportExport;
}

bool NativeFormat::importFile(const QString& filepath, Document* document)
{
    if (!document) {
        setError("Invalid document pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Read JSON from file
    QJsonObject json = readJsonFromFile(filepath);
    if (hasError()) {
        return false;
    }

    reportProgress(30);

    // Deserialize document
    bool success = deserializeDocument(json, document);
    reportProgress(100);

    return success;
}

bool NativeFormat::exportFile(const QString& filepath, const Document* document)
{
    if (!document) {
        setError("Invalid document pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Serialize document
    QJsonObject json = serializeDocument(document);
    reportProgress(50);

    // Write JSON to file
    bool success = writeJsonToFile(filepath, json);
    reportProgress(100);

    return success;
}

bool NativeFormat::importProject(const QString& filepath, Project* project)
{
    if (!project) {
        setError("Invalid project pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Read JSON from file
    QJsonObject json = readJsonFromFile(filepath);
    if (hasError()) {
        return false;
    }

    reportProgress(30);

    // Deserialize project
    bool success = deserializeProject(json, project);
    reportProgress(100);

    return success;
}

bool NativeFormat::exportProject(const QString& filepath, const Project* project)
{
    if (!project) {
        setError("Invalid project pointer");
        return false;
    }

    clearError();
    reportProgress(0);

    // Serialize project
    QJsonObject json = serializeProject(project);
    reportProgress(50);

    // Write JSON to file
    bool success = writeJsonToFile(filepath, json);
    reportProgress(100);

    return success;
}

QJsonObject NativeFormat::serializeDocument(const Document* document) const
{
    QJsonObject json;
    json["version"] = FILE_FORMAT_VERSION;
    json["type"] = "document";
    json["name"] = document->name();

    // Serialize layers with colors and visibility
    QJsonArray layersArray;
    for (const QString& layer : document->layers()) {
        QJsonObject layerObj;
        layerObj["name"] = layer;
        QColor color = document->layerColor(layer);
        layerObj["color"] = color.name(); // Store as hex string #RRGGBB
        layerObj["visible"] = document->isLayerVisible(layer);
        layersArray.append(layerObj);
    }
    json["layers"] = layersArray;
    json["activeLayer"] = document->activeLayer();

    // Serialize objects
    QJsonArray objectsArray;
    for (const auto* obj : document->objects()) {
        objectsArray.append(serializeGeometryObject(obj));
    }
    json["objects"] = objectsArray;

    return json;
}

bool NativeFormat::deserializeDocument(const QJsonObject& json, Document* document)
{
    // Check version
    int version = json["version"].toInt();
    if (version > FILE_FORMAT_VERSION) {
        setError(QString("File format version %1 is not supported").arg(version));
        return false;
    }

    // Load document properties
    document->setName(json["name"].toString("Untitled"));

    // Load layers with colors and visibility
    document->clear();
    QJsonArray layersArray = json["layers"].toArray();
    for (const QJsonValue& value : layersArray) {
        if (value.isObject()) {
            // New format with color and visibility
            QJsonObject layerObj = value.toObject();
            QString layerName = layerObj["name"].toString();
            QColor color(layerObj["color"].toString("#000000"));
            bool visible = layerObj["visible"].toBool(true);

            if (layerName != "Default") { // Default already exists
                document->addLayer(layerName, color);
            } else {
                document->setLayerColor(layerName, color);
            }
            document->setLayerVisible(layerName, visible);
        } else {
            // Legacy format (just layer name string)
            QString layerName = value.toString();
            if (layerName != "Default") {
                document->addLayer(layerName);
            }
        }
    }
    document->setActiveLayer(json["activeLayer"].toString("Default"));

    // Load objects
    QJsonArray objectsArray = json["objects"].toArray();
    for (const QJsonValue& value : objectsArray) {
        Geometry::GeometryObject* obj = deserializeGeometryObject(value.toObject());
        if (obj) {
            document->addObject(obj);
        }
    }

    document->setModified(false);
    return true;
}

QJsonObject NativeFormat::serializeProject(const Project* project) const
{
    QJsonObject json;
    json["version"] = FILE_FORMAT_VERSION;
    json["type"] = "project";
    json["name"] = project->name();

    // TODO: Serialize project-specific data
    // - Multiple documents
    // - Parameters
    // - Constraints

    return json;
}

bool NativeFormat::deserializeProject(const QJsonObject& json, Project* project)
{
    // Check version
    int version = json["version"].toInt();
    if (version > FILE_FORMAT_VERSION) {
        setError(QString("File format version %1 is not supported").arg(version));
        return false;
    }

    // Load project properties
    project->setName(json["name"].toString("Untitled"));

    // TODO: Deserialize project-specific data

    project->setModified(false);
    return true;
}

QJsonObject NativeFormat::serializeGeometryObject(const Geometry::GeometryObject* object) const
{
    QJsonObject json;
    json["id"] = object->id();
    json["name"] = object->name();
    json["type"] = object->typeName();
    json["layer"] = object->layer();
    json["visible"] = object->isVisible();
    json["locked"] = object->isLocked();

    // Serialize geometry-specific data based on type
    if (auto* point = dynamic_cast<const Geometry::Point2D*>(object)) {
        QJsonObject data;
        data["x"] = point->position().x();
        data["y"] = point->position().y();
        json["data"] = data;
    }
    else if (auto* line = dynamic_cast<const Geometry::Line*>(object)) {
        QJsonObject data;
        data["x1"] = line->start().x();
        data["y1"] = line->start().y();
        data["x2"] = line->end().x();
        data["y2"] = line->end().y();
        json["data"] = data;
    }
    else if (auto* circle = dynamic_cast<const Geometry::Circle*>(object)) {
        QJsonObject data;
        data["cx"] = circle->center().x();
        data["cy"] = circle->center().y();
        data["radius"] = circle->radius();
        json["data"] = data;
    }
    else if (auto* rect = dynamic_cast<const Geometry::Rectangle*>(object)) {
        QJsonObject data;
        data["x"] = rect->topLeft().x();
        data["y"] = rect->topLeft().y();
        data["width"] = rect->width();
        data["height"] = rect->height();
        json["data"] = data;
    }
    else if (auto* polyline = dynamic_cast<const Geometry::Polyline*>(object)) {
        QJsonObject data;
        QJsonArray verticesArray;
        for (const auto& vertex : polyline->vertices()) {
            QJsonObject vertexObj;
            vertexObj["x"] = vertex.position.x();
            vertexObj["y"] = vertex.position.y();
            vertexObj["type"] = (vertex.type == Geometry::VertexType::Sharp) ? "sharp" : "smooth";
            vertexObj["incomingTension"] = vertex.incomingTension;
            vertexObj["outgoingTension"] = vertex.outgoingTension;
            vertexObj["tangent_x"] = vertex.tangent.x();
            vertexObj["tangent_y"] = vertex.tangent.y();
            verticesArray.append(vertexObj);
        }
        data["vertices"] = verticesArray;
        data["closed"] = polyline->isClosed();
        json["data"] = data;
    }

    return json;
}

Geometry::GeometryObject* NativeFormat::deserializeGeometryObject(const QJsonObject& json)
{
    QString type = json["type"].toString();
    QJsonObject data = json["data"].toObject();

    Geometry::GeometryObject* object = nullptr;

    // Create appropriate geometry object based on type
    if (type == "Point") {
        double x = data["x"].toDouble();
        double y = data["y"].toDouble();
        object = new Geometry::Point2D(x, y);
    }
    else if (type == "Line") {
        double x1 = data["x1"].toDouble();
        double y1 = data["y1"].toDouble();
        double x2 = data["x2"].toDouble();
        double y2 = data["y2"].toDouble();
        object = new Geometry::Line(QPointF(x1, y1), QPointF(x2, y2));
    }
    else if (type == "Circle") {
        double cx = data["cx"].toDouble();
        double cy = data["cy"].toDouble();
        double radius = data["radius"].toDouble();
        object = new Geometry::Circle(QPointF(cx, cy), radius);
    }
    else if (type == "Rectangle") {
        double x = data["x"].toDouble();
        double y = data["y"].toDouble();
        double width = data["width"].toDouble();
        double height = data["height"].toDouble();
        object = new Geometry::Rectangle(QPointF(x, y), width, height);
    }
    else if (type == "Polyline") {
        QJsonArray verticesArray = data["vertices"].toArray();
        QVector<Geometry::PolylineVertex> vertices;

        for (const QJsonValue& value : verticesArray) {
            QJsonObject vertexObj = value.toObject();
            QPointF position(vertexObj["x"].toDouble(), vertexObj["y"].toDouble());
            QString typeStr = vertexObj["type"].toString("sharp");
            Geometry::VertexType vertexType = (typeStr == "smooth") ? Geometry::VertexType::Smooth : Geometry::VertexType::Sharp;

            // Support both old format (single "tension") and new format (two separate tensions)
            double incomingTension, outgoingTension;
            if (vertexObj.contains("incomingTension")) {
                incomingTension = vertexObj["incomingTension"].toDouble(0.5);
                outgoingTension = vertexObj["outgoingTension"].toDouble(0.5);
            } else {
                // Legacy format - use same tension for both
                double tension = vertexObj["tension"].toDouble(0.5);
                incomingTension = tension;
                outgoingTension = tension;
            }

            QPointF tangent(vertexObj["tangent_x"].toDouble(), vertexObj["tangent_y"].toDouble());

            vertices.append(Geometry::PolylineVertex(position, vertexType, incomingTension, outgoingTension, tangent));
        }

        auto* polyline = new Geometry::Polyline(vertices);
        polyline->setClosed(data["closed"].toBool(false));
        object = polyline;
    }

    // Set common properties
    if (object) {
        object->setId(json["id"].toString());
        object->setName(json["name"].toString());
        object->setLayer(json["layer"].toString("Default"));
        object->setVisible(json["visible"].toBool(true));
        object->setLocked(json["locked"].toBool(false));
    }

    return object;
}

bool NativeFormat::writeJsonToFile(const QString& filepath, const QJsonObject& json)
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(QString("Failed to open file for writing: %1").arg(file.errorString()));
        return false;
    }

    QJsonDocument doc(json);
    qint64 bytesWritten = file.write(doc.toJson(QJsonDocument::Indented));

    if (bytesWritten == -1) {
        setError(QString("Failed to write to file: %1").arg(file.errorString()));
        return false;
    }

    file.close();
    return true;
}

QJsonObject NativeFormat::readJsonFromFile(const QString& filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(QString("Failed to open file for reading: %1").arg(file.errorString()));
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        setError(QString("JSON parse error: %1").arg(parseError.errorString()));
        return QJsonObject();
    }

    if (!doc.isObject()) {
        setError("Invalid file format: expected JSON object");
        return QJsonObject();
    }

    return doc.object();
}

} // namespace IO
} // namespace PatternCAD
