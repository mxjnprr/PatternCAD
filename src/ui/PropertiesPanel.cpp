/**
 * PropertiesPanel.cpp
 *
 * Implementation of PropertiesPanel widget
 */

#include "PropertiesPanel.h"
#include "geometry/GeometryObject.h"
#include <QScrollArea>

namespace PatternCAD {
namespace UI {

PropertiesPanel::PropertiesPanel(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_formLayout(nullptr)
    , m_formWidget(nullptr)
    , m_titleLabel(nullptr)
    , m_nameEdit(nullptr)
    , m_layerCombo(nullptr)
    , m_xPositionEdit(nullptr)
    , m_yPositionEdit(nullptr)
{
    setupUi();
}

PropertiesPanel::~PropertiesPanel()
{
    // Qt handles cleanup of child widgets
}

void PropertiesPanel::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(8);

    // Title label
    m_titleLabel = new QLabel("Properties", this);
    m_titleLabel->setStyleSheet("font-weight: bold;");
    m_mainLayout->addWidget(m_titleLabel);

    // Create scrollable form area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    m_formWidget = new QWidget();
    m_formLayout = new QFormLayout(m_formWidget);
    m_formLayout->setContentsMargins(4, 4, 4, 4);
    m_formLayout->setSpacing(4);

    scrollArea->setWidget(m_formWidget);
    m_mainLayout->addWidget(scrollArea);

    // Create default property widgets
    createCommonProperties();

    // Initially disabled (no selection)
    m_formWidget->setEnabled(false);
}

void PropertiesPanel::createCommonProperties()
{
    // Name property
    m_nameEdit = new QLineEdit(m_formWidget);
    m_nameEdit->setPlaceholderText("Object name");
    connect(m_nameEdit, &QLineEdit::editingFinished,
            this, &PropertiesPanel::onPropertyEdited);
    m_formLayout->addRow("Name:", m_nameEdit);

    // Layer property
    m_layerCombo = new QComboBox(m_formWidget);
    m_layerCombo->addItem("Default");
    // TODO: Populate from document layers
    connect(m_layerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onPropertyEdited);
    m_formLayout->addRow("Layer:", m_layerCombo);

    // Position properties
    m_xPositionEdit = new QLineEdit(m_formWidget);
    m_xPositionEdit->setPlaceholderText("0.0");
    connect(m_xPositionEdit, &QLineEdit::editingFinished,
            this, &PropertiesPanel::onPropertyEdited);
    m_formLayout->addRow("X:", m_xPositionEdit);

    m_yPositionEdit = new QLineEdit(m_formWidget);
    m_yPositionEdit->setPlaceholderText("0.0");
    connect(m_yPositionEdit, &QLineEdit::editingFinished,
            this, &PropertiesPanel::onPropertyEdited);
    m_formLayout->addRow("Y:", m_yPositionEdit);

    // TODO: Add more common properties (color, line style, etc.)
}

void PropertiesPanel::createGeometryProperties()
{
    // TODO: Create geometry-specific property widgets based on object type
    // - Line: start point, end point, length, angle
    // - Circle: center, radius
    // - Rectangle: position, width, height
    // - Bezier: control points
}

void PropertiesPanel::setSelectedObjects(const QList<Geometry::GeometryObject*>& objects)
{
    m_selectedObjects = objects;
    updateProperties();
}

void PropertiesPanel::clearSelection()
{
    m_selectedObjects.clear();
    updateProperties();
}

void PropertiesPanel::updateProperties()
{
    if (m_selectedObjects.isEmpty()) {
        // No selection - disable panel
        m_formWidget->setEnabled(false);
        m_titleLabel->setText("Properties");

        // Clear property values
        m_nameEdit->clear();
        m_xPositionEdit->clear();
        m_yPositionEdit->clear();
    } else if (m_selectedObjects.size() == 1) {
        // Single selection - show properties
        m_formWidget->setEnabled(true);
        m_titleLabel->setText("Properties (1 object)");

        // TODO: Load properties from selected object
        // Geometry::GeometryObject* obj = m_selectedObjects.first();
        // m_nameEdit->setText(obj->name());
        // m_xPositionEdit->setText(QString::number(obj->position().x()));
        // m_yPositionEdit->setText(QString::number(obj->position().y()));

        // TODO: Create geometry-specific properties
    } else {
        // Multiple selection - show common properties
        m_formWidget->setEnabled(true);
        m_titleLabel->setText(QString("Properties (%1 objects)").arg(m_selectedObjects.size()));

        // TODO: Show only common properties for multiple objects
        m_nameEdit->clear();
        m_nameEdit->setPlaceholderText("[Multiple objects]");
    }
}

void PropertiesPanel::onPropertyEdited()
{
    if (m_selectedObjects.isEmpty()) {
        return;
    }

    // TODO: Apply property changes to selected objects
    // Determine which property was edited
    QWidget* sender = qobject_cast<QWidget*>(QObject::sender());

    if (sender == m_nameEdit) {
        emit propertyChanged("name", m_nameEdit->text());
    } else if (sender == m_xPositionEdit) {
        bool ok;
        double value = m_xPositionEdit->text().toDouble(&ok);
        if (ok) {
            emit propertyChanged("x", value);
        }
    } else if (sender == m_yPositionEdit) {
        bool ok;
        double value = m_yPositionEdit->text().toDouble(&ok);
        if (ok) {
            emit propertyChanged("y", value);
        }
    }
}

} // namespace UI
} // namespace PatternCAD
