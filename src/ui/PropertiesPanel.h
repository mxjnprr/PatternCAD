/**
 * PropertiesPanel.h
 *
 * QWidget for displaying and editing object properties
 */

#ifndef PATTERNCAD_PROPERTIESPANEL_H
#define PATTERNCAD_PROPERTIESPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QList>

namespace PatternCAD {

// Forward declarations
namespace Geometry {
    class GeometryObject;
}

namespace UI {

/**
 * PropertiesPanel displays and allows editing of:
 * - Selected object properties
 * - Geometric parameters (position, size, angles)
 * - Visual properties (color, line style, fill)
 * - Constraints and parameters
 * - Layer assignment
 */
class PropertiesPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PropertiesPanel(QWidget* parent = nullptr);
    ~PropertiesPanel();

    // Selection
    void setSelectedObjects(const QList<Geometry::GeometryObject*>& objects);
    void clearSelection();

signals:
    void propertyChanged(const QString& propertyName, const QVariant& value);

private slots:
    void onPropertyEdited();

private:
    // UI setup
    void setupUi();
    void updateProperties();
    void createCommonProperties();
    void createGeometryProperties();

    // Private members
    QVBoxLayout* m_mainLayout;
    QFormLayout* m_formLayout;
    QWidget* m_formWidget;
    QLabel* m_titleLabel;
    QList<Geometry::GeometryObject*> m_selectedObjects;

    // Property widgets (examples)
    QLineEdit* m_nameEdit;
    QComboBox* m_layerCombo;
    QLineEdit* m_xPositionEdit;
    QLineEdit* m_yPositionEdit;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_PROPERTIESPANEL_H
