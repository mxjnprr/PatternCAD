/**
 * PDFFormat.h
 *
 * PDF export format handler
 */

#ifndef PATTERNCAD_PDFFORMAT_H
#define PATTERNCAD_PDFFORMAT_H

#include "FileFormat.h"
#include <QString>
#include <QPageSize>
#include <QPageLayout>

class QPainter;

namespace PatternCAD {

namespace Geometry {
    class GeometryObject;
}

namespace IO {

/**
 * PDFFormat handles export to PDF (Portable Document Format)
 * - Vector-based output (not rasterized)
 * - Standard page sizes
 * - Portrait/landscape orientation
 * - Fit to page scaling
 * - PDF metadata
 */
class PDFFormat : public FileFormat
{
    Q_OBJECT

public:
    explicit PDFFormat(QObject* parent = nullptr);
    ~PDFFormat();

    // Format identification
    QString formatName() const override;
    QString formatDescription() const override;
    QStringList fileExtensions() const override;
    FormatType formatType() const override;

    // Capabilities
    FormatCapability capabilities() const override;

    // Export operation
    bool exportFile(const QString& filepath, const Document* document) override;

    // Configuration
    void setPageSize(QPageSize::PageSizeId pageSize) { m_pageSize = pageSize; }
    void setOrientation(QPageLayout::Orientation orientation) { m_orientation = orientation; }

private:
    void renderGeometry(QPainter* painter, const Geometry::GeometryObject* object);
    QRectF calculateBounds(const Document* document) const;

    QPageSize::PageSizeId m_pageSize;
    QPageLayout::Orientation m_orientation;
};

} // namespace IO
} // namespace PatternCAD

#endif // PATTERNCAD_PDFFORMAT_H
