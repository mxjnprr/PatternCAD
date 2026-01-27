/**
 * FileFormat.cpp
 *
 * Implementation of FileFormat base class
 */

#include "FileFormat.h"
#include "core/Document.h"
#include "core/Project.h"

namespace PatternCAD {
namespace IO {

FileFormat::FileFormat(QObject* parent)
    : QObject(parent)
{
}

FileFormat::~FileFormat()
{
}

bool FileFormat::canImport() const
{
    int caps = static_cast<int>(capabilities());
    return (caps & static_cast<int>(FormatCapability::Import)) != 0;
}

bool FileFormat::canExport() const
{
    int caps = static_cast<int>(capabilities());
    return (caps & static_cast<int>(FormatCapability::Export)) != 0;
}

QString FileFormat::fileFilter() const
{
    QStringList extensions = fileExtensions();
    QString filter = formatDescription() + " (";

    for (int i = 0; i < extensions.size(); ++i) {
        if (i > 0) {
            filter += " ";
        }
        filter += "*." + extensions[i];
    }

    filter += ")";
    return filter;
}

bool FileFormat::importFile(const QString& filepath, Document* document)
{
    Q_UNUSED(filepath);
    Q_UNUSED(document);
    setError("Import not implemented for this format");
    return false;
}

bool FileFormat::exportFile(const QString& filepath, const Document* document)
{
    Q_UNUSED(filepath);
    Q_UNUSED(document);
    setError("Export not implemented for this format");
    return false;
}

bool FileFormat::importProject(const QString& filepath, Project* project)
{
    Q_UNUSED(filepath);
    Q_UNUSED(project);
    setError("Project import not implemented for this format");
    return false;
}

bool FileFormat::exportProject(const QString& filepath, const Project* project)
{
    Q_UNUSED(filepath);
    Q_UNUSED(project);
    setError("Project export not implemented for this format");
    return false;
}

QString FileFormat::lastError() const
{
    return m_lastError;
}

bool FileFormat::hasError() const
{
    return !m_lastError.isEmpty();
}

void FileFormat::setError(const QString& error)
{
    m_lastError = error;
}

void FileFormat::clearError()
{
    m_lastError.clear();
}

void FileFormat::reportProgress(int percentage)
{
    emit progressChanged(percentage);
}

} // namespace IO
} // namespace PatternCAD
