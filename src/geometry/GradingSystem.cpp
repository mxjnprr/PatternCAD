/**
 * GradingSystem.cpp
 *
 * Implementation of GradingSystem
 */

#include "GradingSystem.h"
#include "Polyline.h"
#include <algorithm>

namespace PatternCAD {

GradingSystem::GradingSystem(QObject* parent)
    : QObject(parent)
    , m_baseSizeIndex(0)
{
}

GradingSystem::~GradingSystem()
{
}

// Size management

void GradingSystem::setSizes(const QVector<SizeInfo>& sizes)
{
    m_sizes = sizes;
    // Ensure base size index is valid
    if (m_baseSizeIndex >= m_sizes.size()) {
        m_baseSizeIndex = m_sizes.isEmpty() ? 0 : m_sizes.size() / 2;
    }
    emit sizesChanged();
    emit changed();
}

void GradingSystem::addSize(const QString& name, double offset)
{
    m_sizes.append(SizeInfo(name, offset));
    emit sizesChanged();
    emit changed();
}

void GradingSystem::removeSize(int index)
{
    if (index >= 0 && index < m_sizes.size()) {
        m_sizes.removeAt(index);
        if (m_baseSizeIndex >= m_sizes.size()) {
            m_baseSizeIndex = m_sizes.isEmpty() ? 0 : m_sizes.size() - 1;
        }
        emit sizesChanged();
        emit changed();
    }
}

void GradingSystem::clearSizes()
{
    m_sizes.clear();
    m_baseSizeIndex = 0;
    emit sizesChanged();
    emit changed();
}

SizeInfo GradingSystem::sizeAt(int index) const
{
    if (index >= 0 && index < m_sizes.size()) {
        return m_sizes[index];
    }
    return SizeInfo();
}

void GradingSystem::setBaseSizeIndex(int index)
{
    if (index >= 0 && index < m_sizes.size() && index != m_baseSizeIndex) {
        m_baseSizeIndex = index;
        emit sizesChanged();
        emit changed();
    }
}

QString GradingSystem::baseSizeName() const
{
    if (m_baseSizeIndex >= 0 && m_baseSizeIndex < m_sizes.size()) {
        return m_sizes[m_baseSizeIndex].name;
    }
    return QString();
}

// Grading rules

void GradingSystem::setRules(const QVector<GradeRule>& rules)
{
    m_rules = rules;
    emit rulesChanged();
    emit changed();
}

void GradingSystem::addRule(const GradeRule& rule)
{
    m_rules.append(rule);
    emit rulesChanged();
    emit changed();
}

void GradingSystem::updateRule(int index, const GradeRule& rule)
{
    if (index >= 0 && index < m_rules.size()) {
        m_rules[index] = rule;
        emit rulesChanged();
        emit changed();
    }
}

void GradingSystem::removeRule(int index)
{
    if (index >= 0 && index < m_rules.size()) {
        m_rules.removeAt(index);
        emit rulesChanged();
        emit changed();
    }
}

void GradingSystem::clearRules()
{
    m_rules.clear();
    emit rulesChanged();
    emit changed();
}

GradeRule GradingSystem::ruleAt(int index) const
{
    if (index >= 0 && index < m_rules.size()) {
        return m_rules[index];
    }
    return GradeRule();
}

GradeRule* GradingSystem::findRuleForVertex(int vertexIndex)
{
    for (int i = 0; i < m_rules.size(); ++i) {
        if (m_rules[i].vertexIndex == vertexIndex) {
            return &m_rules[i];
        }
    }
    return nullptr;
}

const GradeRule* GradingSystem::findRuleForVertex(int vertexIndex) const
{
    for (int i = 0; i < m_rules.size(); ++i) {
        if (m_rules[i].vertexIndex == vertexIndex) {
            return &m_rules[i];
        }
    }
    return nullptr;
}

// Apply grading

double GradingSystem::getOffsetForSize(int sizeIndex) const
{
    if (sizeIndex >= 0 && sizeIndex < m_sizes.size()) {
        return m_sizes[sizeIndex].offset;
    }
    return 0.0;
}

double GradingSystem::getOffsetForSize(const QString& sizeName) const
{
    for (const SizeInfo& size : m_sizes) {
        if (size.name == sizeName) {
            return size.offset;
        }
    }
    return 0.0;
}

Geometry::Polyline* GradingSystem::applyToSize(const Geometry::Polyline* base, int sizeIndex) const
{
    if (!base || sizeIndex < 0 || sizeIndex >= m_sizes.size()) {
        return nullptr;
    }
    
    double offset = m_sizes[sizeIndex].offset;
    
    // Clone the base polyline
    Geometry::Polyline* graded = base->clone();
    if (!graded) {
        return nullptr;
    }
    
    // Apply grading rules to vertices
    QVector<Geometry::PolylineVertex> vertices = graded->vertices();
    
    for (const GradeRule& rule : m_rules) {
        if (rule.vertexIndex >= 0 && rule.vertexIndex < vertices.size()) {
            // Apply offset: position += increment * offset
            QPointF delta = rule.incrementPerSize * offset;
            vertices[rule.vertexIndex].position += delta;
        }
    }
    
    graded->setVertices(vertices);
    graded->setName(base->name() + " - " + m_sizes[sizeIndex].name);
    
    return graded;
}

Geometry::Polyline* GradingSystem::applyToSize(const Geometry::Polyline* base, const QString& sizeName) const
{
    for (int i = 0; i < m_sizes.size(); ++i) {
        if (m_sizes[i].name == sizeName) {
            return applyToSize(base, i);
        }
    }
    return nullptr;
}

// Preset size ranges

void GradingSystem::setStandardSizes()
{
    m_sizes.clear();
    m_sizes.append(SizeInfo("XS", -2.0));
    m_sizes.append(SizeInfo("S", -1.0));
    m_sizes.append(SizeInfo("M", 0.0));
    m_sizes.append(SizeInfo("L", 1.0));
    m_sizes.append(SizeInfo("XL", 2.0));
    m_baseSizeIndex = 2;  // M is the base
    emit sizesChanged();
    emit changed();
}

void GradingSystem::setNumericSizes(int start, int end, int step)
{
    m_sizes.clear();
    
    if (step <= 0) step = 2;
    
    int mid = (start + end) / 2;
    // Round mid to nearest valid size
    mid = start + ((mid - start) / step) * step;
    
    for (int size = start; size <= end; size += step) {
        double offset = static_cast<double>(size - mid) / step;
        m_sizes.append(SizeInfo(QString::number(size), offset));
    }
    
    // Set base size to middle
    m_baseSizeIndex = m_sizes.size() / 2;
    
    emit sizesChanged();
    emit changed();
}

// Validation

bool GradingSystem::isValid() const
{
    if (m_sizes.isEmpty()) {
        return false;
    }
    
    if (m_baseSizeIndex < 0 || m_baseSizeIndex >= m_sizes.size()) {
        return false;
    }
    
    // Check for duplicate size names
    QSet<QString> names;
    for (const SizeInfo& size : m_sizes) {
        if (names.contains(size.name)) {
            return false;
        }
        names.insert(size.name);
    }
    
    return true;
}

QString GradingSystem::validationError() const
{
    if (m_sizes.isEmpty()) {
        return tr("No sizes defined");
    }
    
    if (m_baseSizeIndex < 0 || m_baseSizeIndex >= m_sizes.size()) {
        return tr("Invalid base size index");
    }
    
    QSet<QString> names;
    for (const SizeInfo& size : m_sizes) {
        if (names.contains(size.name)) {
            return tr("Duplicate size name: %1").arg(size.name);
        }
        names.insert(size.name);
    }
    
    return QString();
}

// Clone

GradingSystem* GradingSystem::clone(QObject* parent) const
{
    GradingSystem* copy = new GradingSystem(parent);
    copy->m_sizes = m_sizes;
    copy->m_baseSizeIndex = m_baseSizeIndex;
    copy->m_rules = m_rules;
    return copy;
}

// Serialization

QJsonObject GradingSystem::toJson() const
{
    QJsonObject obj;
    
    // Sizes
    QJsonArray sizesArray;
    for (const SizeInfo& size : m_sizes) {
        sizesArray.append(size.toJson());
    }
    obj["sizes"] = sizesArray;
    obj["baseSizeIndex"] = m_baseSizeIndex;
    
    // Rules
    QJsonArray rulesArray;
    for (const GradeRule& rule : m_rules) {
        rulesArray.append(rule.toJson());
    }
    obj["rules"] = rulesArray;
    
    return obj;
}

GradingSystem* GradingSystem::fromJson(const QJsonObject& json, QObject* parent)
{
    GradingSystem* system = new GradingSystem(parent);
    
    // Sizes
    QJsonArray sizesArray = json["sizes"].toArray();
    for (const QJsonValue& val : sizesArray) {
        system->m_sizes.append(SizeInfo::fromJson(val.toObject()));
    }
    
    system->m_baseSizeIndex = json["baseSizeIndex"].toInt(0);
    
    // Rules
    QJsonArray rulesArray = json["rules"].toArray();
    for (const QJsonValue& val : rulesArray) {
        system->m_rules.append(GradeRule::fromJson(val.toObject()));
    }
    
    return system;
}

} // namespace PatternCAD
