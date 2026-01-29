/**
 * DXFFormat.h
 *
 * DXF import/export format handler
 */

#ifndef PATTERNCAD_DXFFORMAT_H
#define PATTERNCAD_DXFFORMAT_H

#include "FileFormat.h"
#include <QString>
#include <QPointF>
#include <QMap>
#include <QTextStream>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
}

namespace IO {

/**
 * DXFFormat handles import/export of DXF (Drawing Exchange Format)
 * - DXF R12 and later versions supported
 * - ASCII format only (not binary)
 * - Supported entities: LINE, CIRCLE, ARC, POLYLINE, LWPOLYLINE, POINT
 * - Layer support
 */
class DXFFormat : public FileFormat
{
    Q_OBJECT

public:
    explicit DXFFormat(QObject* parent = nullptr);
    ~DXFFormat();

    // Format identification
    QString formatName() const override;
    QString formatDescription() const override;
    QStringList fileExtensions() const override;
    FormatType formatType() const override;

    // Capabilities
    FormatCapability capabilities() const override;

    // Import operation
    bool importFile(const QString& filepath, Document* document) override;

    // Export operation (not yet implemented)
    bool exportFile(const QString& filepath, const Document* document) override;

private:
    // Import helpers
    struct DXFEntity {
        QString type;
        QString layer;
        QMultiMap<int, QString> attributes;
    };

    bool parseDXF(QTextStream& stream, Document* document);
    void processEntity(const DXFEntity& entity, Document* document);
    
    // Entity processors
    void processLine(const DXFEntity& entity, Document* document);
    void processCircle(const DXFEntity& entity, Document* document);
    void processArc(const DXFEntity& entity, Document* document);
    void processPolyline(const DXFEntity& polylineEntity, const QList<DXFEntity>& vertices, Document* document);
    void processLWPolyline(const DXFEntity& entity, Document* document);
    void processPoint(const DXFEntity& entity, Document* document);

    // Utility
    QString readPair(QTextStream& stream, int& code);
    double getDouble(const DXFEntity& entity, int code, double defaultValue = 0.0) const;
    QString getString(const DXFEntity& entity, int code, const QString& defaultValue = "") const;
};

} // namespace IO
} // namespace PatternCAD

#endif // PATTERNCAD_DXFFORMAT_H
