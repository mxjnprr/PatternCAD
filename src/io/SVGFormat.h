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
#include <QDomElement>

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
    struct PolylineVertex;
}

class Notch;
class MatchPoint;

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

    // Import operation
    bool importFile(const QString& filepath, Document* document) override;

private:
    // Export helpers
    QString generateSVG(const Document* document) const;
    QString geometryToSVG(const Geometry::GeometryObject* object, int indent = 2) const;
    QString notchToSVG(const Notch* notch, int indent = 2) const;
    QString matchPointToSVG(const MatchPoint* mp, int indent = 2) const;
    QString formatIndent(int level) const;
    QRectF calculateBounds(const Document* document) const;

    // Import helpers
    bool parseSVG(const QString& svgContent, Document* document);
    void parseElement(const QDomElement& element, Document* document, const QString& layerName);
    void parsePath(const QDomElement& element, Document* document, const QString& layerName);
    void parseLine(const QDomElement& element, Document* document, const QString& layerName);
    void parseCircle(const QDomElement& element, Document* document, const QString& layerName);
    void parseRect(const QDomElement& element, Document* document, const QString& layerName);
    void parsePolyline(const QDomElement& element, Document* document, const QString& layerName);
    void parsePolygon(const QDomElement& element, Document* document, const QString& layerName);

    // Path parsing
    QVector<Geometry::PolylineVertex> parsePathData(const QString& pathData);
};

} // namespace IO
} // namespace PatternCAD

#endif // PATTERNCAD_SVGFORMAT_H
