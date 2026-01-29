/**
 * SVGFormat.h
 *
 * SVG export format handler
 */

#ifndef PATTERNCAD_SVGFORMAT_H
#define PATTERNCAD_SVGFORMAT_H

#include "FileFormat.h"
#include <QString>
#include <QRectF>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
}

namespace IO {

/**
 * SVGFormat handles export to SVG (Scalable Vector Graphics) format
 * - SVG 1.1 standard
 * - Clean, readable output
 * - Layers as groups
 * - All geometry types supported
 */
class SVGFormat : public FileFormat
{
    Q_OBJECT

public:
    explicit SVGFormat(QObject* parent = nullptr);
    ~SVGFormat();

    // Format identification
    QString formatName() const override;
    QString formatDescription() const override;
    QStringList fileExtensions() const override;
    FormatType formatType() const override;

    // Capabilities
    FormatCapability capabilities() const override;

    // Export operation
    bool exportFile(const QString& filepath, const Document* document) override;

private:
    QString generateSVG(const Document* document) const;
    QString geometryToSVG(const Geometry::GeometryObject* object, int indent = 2) const;
    QString formatIndent(int level) const;
    QRectF calculateBounds(const Document* document) const;
};

} // namespace IO
} // namespace PatternCAD

#endif // PATTERNCAD_SVGFORMAT_H
