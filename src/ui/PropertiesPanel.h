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
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QList>

namespace PatternCAD {

class Document;

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

    // Document
    void setDocument(Document* document);

    // Selection
    void setSelectedObjects(const QList<Geometry::GeometryObject*>& objects);
    void clearSelection();

signals:
    void propertyChanged(const QString& propertyName, const QVariant& value);

private slots:
    void onPropertyEdited();
    void onColorButtonClicked();

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
    Document* m_document;
    QList<Geometry::GeometryObject*> m_selectedObjects;

    // Property widgets
    QLineEdit* m_nameEdit;
    QComboBox* m_layerCombo;
    QDoubleSpinBox* m_widthEdit;   // Bounding box width (read-only)
    QDoubleSpinBox* m_heightEdit;  // Bounding box height (read-only)
    QPushButton* m_lineColorButton;
    QComboBox* m_lineStyleCombo;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_PROPERTIESPANEL_H
