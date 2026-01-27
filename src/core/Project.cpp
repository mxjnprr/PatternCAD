/**
 * Project.cpp
 *
 * Implementation of Project class
 */

#include "Project.h"

namespace PatternCAD {

Project::Project(QObject* parent)
    : QObject(parent)
    , m_name("Untitled")
    , m_modified(false)
    , m_unit(Unit::Centimeters)
    , m_gridSpacing(10.0)
{
}

Project::~Project()
{
    // Cleanup will be implemented when we add layers and patterns
}

QString Project::name() const
{
    return m_name;
}

void Project::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
        setModified(true);
    }
}

QString Project::filepath() const
{
    return m_filepath;
}

void Project::setFilepath(const QString& filepath)
{
    m_filepath = filepath;
}

bool Project::isModified() const
{
    return m_modified;
}

void Project::setModified(bool modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(modified);
    }
}

Unit Project::unit() const
{
    return m_unit;
}

void Project::setUnit(Unit unit)
{
    if (m_unit != unit) {
        m_unit = unit;
        emit unitChanged(unit);
        setModified(true);
    }
}

double Project::gridSpacing() const
{
    return m_gridSpacing;
}

void Project::setGridSpacing(double spacing)
{
    if (m_gridSpacing != spacing) {
        m_gridSpacing = spacing;
        emit gridSpacingChanged(spacing);
        setModified(true);
    }
}

} // namespace PatternCAD
