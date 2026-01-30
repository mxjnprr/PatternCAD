/**
 * GradingSystem.h
 *
 * Grading system for pattern sizing - defines size ranges and grading rules
 */

#ifndef PATTERNCAD_GRADINGSYSTEM_H
#define PATTERNCAD_GRADINGSYSTEM_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>

namespace PatternCAD {

namespace Geometry {
    class Polyline;
}

/**
 * Size definition with name and offset from base size
 */
struct SizeInfo {
    QString name;      // Size name (e.g., "XS", "S", "M", "L", "XL" or "34", "36", "38")
    double offset;     // Offset from base size in size steps (-2, -1, 0, +1, +2)
    
    SizeInfo(const QString& n = "", double o = 0.0) : name(n), offset(o) {}
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["offset"] = offset;
        return obj;
    }
    
    static SizeInfo fromJson(const QJsonObject& obj) {
        return SizeInfo(obj["name"].toString(), obj["offset"].toDouble());
    }
};

/**
 * A grading rule that specifies how a vertex moves per size step
 */
struct GradeRule {
    QString name;              // Rule name (e.g., "Bust", "Waist", "Hip")
    int vertexIndex;           // Which vertex to affect
    QPointF incrementPerSize;  // X,Y movement per size step (in mm)
    
    GradeRule(const QString& n = "", int idx = 0, const QPointF& inc = QPointF())
        : name(n), vertexIndex(idx), incrementPerSize(inc) {}
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["vertexIndex"] = vertexIndex;
        obj["incrementX"] = incrementPerSize.x();
        obj["incrementY"] = incrementPerSize.y();
        return obj;
    }
    
    static GradeRule fromJson(const QJsonObject& obj) {
        return GradeRule(
            obj["name"].toString(),
            obj["vertexIndex"].toInt(),
            QPointF(obj["incrementX"].toDouble(), obj["incrementY"].toDouble())
        );
    }
};

/**
 * GradingSystem manages size definitions and grading rules for a pattern.
 * It can apply rules to generate graded versions of a base pattern.
 */
class GradingSystem : public QObject
{
    Q_OBJECT

public:
    explicit GradingSystem(QObject* parent = nullptr);
    ~GradingSystem();

    // Size management
    QVector<SizeInfo> sizes() const { return m_sizes; }
    void setSizes(const QVector<SizeInfo>& sizes);
    void addSize(const QString& name, double offset);
    void removeSize(int index);
    void clearSizes();
    int sizeCount() const { return m_sizes.size(); }
    SizeInfo sizeAt(int index) const;
    
    // Base size
    int baseSizeIndex() const { return m_baseSizeIndex; }
    void setBaseSizeIndex(int index);
    QString baseSizeName() const;
    
    // Grading rules
    QVector<GradeRule> rules() const { return m_rules; }
    void setRules(const QVector<GradeRule>& rules);
    void addRule(const GradeRule& rule);
    void updateRule(int index, const GradeRule& rule);
    void removeRule(int index);
    void clearRules();
    int ruleCount() const { return m_rules.size(); }
    GradeRule ruleAt(int index) const;
    
    // Find rule for a specific vertex
    GradeRule* findRuleForVertex(int vertexIndex);
    const GradeRule* findRuleForVertex(int vertexIndex) const;
    
    // Apply grading to create a sized version
    Geometry::Polyline* applyToSize(const Geometry::Polyline* base, int sizeIndex) const;
    Geometry::Polyline* applyToSize(const Geometry::Polyline* base, const QString& sizeName) const;
    
    // Get offset multiplier for a size
    double getOffsetForSize(int sizeIndex) const;
    double getOffsetForSize(const QString& sizeName) const;
    
    // Preset size ranges
    void setStandardSizes();        // XS, S, M, L, XL
    void setNumericSizes(int start, int end, int step);  // 34, 36, 38, ...
    
    // Validation
    bool isValid() const;
    QString validationError() const;
    
    // Clone
    GradingSystem* clone(QObject* parent = nullptr) const;
    
    // Serialization
    QJsonObject toJson() const;
    static GradingSystem* fromJson(const QJsonObject& json, QObject* parent = nullptr);

signals:
    void changed();
    void sizesChanged();
    void rulesChanged();

private:
    QVector<SizeInfo> m_sizes;
    int m_baseSizeIndex;
    QVector<GradeRule> m_rules;
};

} // namespace PatternCAD

#endif // PATTERNCAD_GRADINGSYSTEM_H
