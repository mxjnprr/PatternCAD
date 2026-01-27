/**
 * Project.h
 *
 * Project class representing a complete pattern design project
 */

#ifndef PATTERNCAD_PROJECT_H
#define PATTERNCAD_PROJECT_H

#include <QObject>
#include <QString>
#include <QMap>
#include <vector>

namespace PatternCAD {

// Forward declarations
class Pattern;
class Layer;

/**
 * Units for measurements
 */
enum class Unit {
    Millimeters,
    Centimeters,
    Inches
};

/**
 * Project contains all data for a pattern design:
 * - Patterns and geometry
 * - Layers and organization
 * - Parameters and constraints
 * - Settings and metadata
 */
class Project : public QObject
{
    Q_OBJECT

public:
    explicit Project(QObject* parent = nullptr);
    ~Project();

    // Project metadata
    QString name() const;
    void setName(const QString& name);

    QString filepath() const;
    void setFilepath(const QString& filepath);

    bool isModified() const;
    void setModified(bool modified);

    // Units
    Unit unit() const;
    void setUnit(Unit unit);

    // Grid settings
    double gridSpacing() const;
    void setGridSpacing(double spacing);

    // Layers (to be implemented)
    // std::vector<Layer*> layers() const;

    // Patterns (to be implemented)
    // std::vector<Pattern*> patterns() const;

    // Parameters (to be implemented)
    // QMap<QString, double> parameters() const;

signals:
    void nameChanged(const QString& name);
    void modifiedChanged(bool modified);
    void unitChanged(Unit unit);
    void gridSpacingChanged(double spacing);

private:
    QString m_name;
    QString m_filepath;
    bool m_modified;
    Unit m_unit;
    double m_gridSpacing;

    // To be implemented:
    // std::vector<Layer*> m_layers;
    // std::vector<Pattern*> m_patterns;
    // QMap<QString, double> m_parameters;
};

} // namespace PatternCAD

#endif // PATTERNCAD_PROJECT_H
