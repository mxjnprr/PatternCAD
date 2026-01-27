/**
 * NativeFormat.h
 *
 * Native .patterncad file format handler
 */

#ifndef PATTERNCAD_NATIVEFORMAT_H
#define PATTERNCAD_NATIVEFORMAT_H

#include "FileFormat.h"
#include <QJsonObject>
#include <QJsonArray>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
}

namespace IO {

/**
 * NativeFormat handles the PatternCAD native file format:
 * - JSON-based format with .patterncad extension
 * - Stores complete project/document data
 * - Preserves all properties and metadata
 * - Human-readable structure
 * - Version tracking for backward compatibility
 */
class NativeFormat : public FileFormat
{
    Q_OBJECT

public:
    explicit NativeFormat(QObject* parent = nullptr);
    ~NativeFormat();

    // Format identification
    QString formatName() const override;
    QString formatDescription() const override;
    QStringList fileExtensions() const override;
    FormatType formatType() const override;

    // Capabilities
    FormatCapability capabilities() const override;

    // Import/Export operations
    bool importFile(const QString& filepath, Document* document) override;
    bool exportFile(const QString& filepath, const Document* document) override;
    bool importProject(const QString& filepath, Project* project) override;
    bool exportProject(const QString& filepath, const Project* project) override;

private:
    static constexpr int FILE_FORMAT_VERSION = 1;

    // Serialization helpers
    QJsonObject serializeDocument(const Document* document) const;
    bool deserializeDocument(const QJsonObject& json, Document* document);

    QJsonObject serializeProject(const Project* project) const;
    bool deserializeProject(const QJsonObject& json, Project* project);

    QJsonObject serializeGeometryObject(const Geometry::GeometryObject* object) const;
    Geometry::GeometryObject* deserializeGeometryObject(const QJsonObject& json);

    // File I/O helpers
    bool writeJsonToFile(const QString& filepath, const QJsonObject& json);
    QJsonObject readJsonFromFile(const QString& filepath);
};

} // namespace IO
} // namespace PatternCAD

#endif // PATTERNCAD_NATIVEFORMAT_H
