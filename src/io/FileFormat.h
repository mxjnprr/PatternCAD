/**
 * FileFormat.h
 *
 * Base class for file format importers and exporters
 */

#ifndef PATTERNCAD_FILEFORMAT_H
#define PATTERNCAD_FILEFORMAT_H

#include <QObject>
#include <QString>
#include <QStringList>

namespace PatternCAD {

// Forward declarations
class Document;
class Project;

namespace IO {

/**
 * FileFormat type enumeration
 */
enum class FormatType {
    Native,     // PatternCAD native format
    DXF,        // AutoCAD DXF
    SVG,        // Scalable Vector Graphics
    PDF,        // Portable Document Format
    JSON        // JSON export
};

/**
 * FileFormat capabilities
 */
enum class FormatCapability {
    Import = 0x01,
    Export = 0x02,
    ImportExport = 0x03
};

/**
 * Base class for file format handlers providing:
 * - File type identification
 * - Import/export capabilities
 * - Document serialization
 * - Error handling
 */
class FileFormat : public QObject
{
    Q_OBJECT

public:
    explicit FileFormat(QObject* parent = nullptr);
    virtual ~FileFormat();

    // Format identification
    virtual QString formatName() const = 0;
    virtual QString formatDescription() const = 0;
    virtual QStringList fileExtensions() const = 0;
    virtual FormatType formatType() const = 0;

    // Capabilities
    virtual FormatCapability capabilities() const = 0;
    bool canImport() const;
    bool canExport() const;

    // File filter for dialogs
    QString fileFilter() const;

    // Import/Export operations
    virtual bool importFile(const QString& filepath, Document* document);
    virtual bool exportFile(const QString& filepath, const Document* document);

    // Project-level operations (for formats that support multiple documents)
    virtual bool importProject(const QString& filepath, Project* project);
    virtual bool exportProject(const QString& filepath, const Project* project);

    // Error handling
    QString lastError() const;
    bool hasError() const;

signals:
    void progressChanged(int percentage);
    void statusMessage(const QString& message);

protected:
    void setError(const QString& error);
    void clearError();
    void reportProgress(int percentage);

    QString m_lastError;
};

} // namespace IO
} // namespace PatternCAD

#endif // PATTERNCAD_FILEFORMAT_H
