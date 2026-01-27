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
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    // Serialize layers
    QJsonArray layersArray;
    for (const QString& layer : document->layers()) {
        layersArray.append(layer);
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

    // Load layers
    document->clear();
    QJsonArray layersArray = json["layers"].toArray();
    for (const QJsonValue& value : layersArray) {
        QString layerName = value.toString();
        if (layerName != "Default") { // Default already exists
            document->addLayer(layerName);
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
    json["visible"] = object->isVisible();
    json["locked"] = object->isLocked();

    // TODO: Serialize geometry-specific data based on type
    // This would require dynamic casting to specific geometry types

    return json;
}

Geometry::GeometryObject* NativeFormat::deserializeGeometryObject(const QJsonObject& json)
{
    QString type = json["type"].toString();

    // TODO: Create appropriate geometry object based on type
    // and deserialize its properties
    Q_UNUSED(type);

    return nullptr;
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
