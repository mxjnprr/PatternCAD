/**
 * PropertiesPanel.cpp
 *
 * Implementation of PropertiesPanel widget
 */

#include "PropertiesPanel.h"
#include "geometry/GeometryObject.h"
#include "geometry/Rectangle.h"
#include "geometry/Circle.h"
#include "core/Application.h"
#include "core/Project.h"
#include "core/Document.h"
#include "core/Commands.h"
#include <QScrollArea>
#include <QColorDialog>

namespace PatternCAD {
namespace UI {

PropertiesPanel::PropertiesPanel(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_formLayout(nullptr)
    , m_formWidget(nullptr)
    , m_titleLabel(nullptr)
    , m_document(nullptr)
    , m_nameEdit(nullptr)
    , m_layerCombo(nullptr)
    , m_widthEdit(nullptr)
    , m_heightEdit(nullptr)
    , m_lineColorButton(nullptr)
    , m_lineStyleCombo(nullptr)
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
    connect(m_layerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onPropertyEdited);
    m_formLayout->addRow("Layer:", m_layerCombo);

    // Size properties (bounding box dimensions - always visible, read-only)
    m_widthEdit = new QDoubleSpinBox(m_formWidget);
    m_widthEdit->setRange(0.0, 10000);
    m_widthEdit->setDecimals(2);
    m_widthEdit->setSuffix(" mm");
    m_widthEdit->setReadOnly(true);
    m_widthEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_formLayout->addRow("Width:", m_widthEdit);

    m_heightEdit = new QDoubleSpinBox(m_formWidget);
    m_heightEdit->setRange(0.0, 10000);
    m_heightEdit->setDecimals(2);
    m_heightEdit->setSuffix(" mm");
    m_heightEdit->setReadOnly(true);
    m_heightEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_formLayout->addRow("Height:", m_heightEdit);

    // Line color
    m_lineColorButton = new QPushButton(m_formWidget);
    m_lineColorButton->setMaximumWidth(100);
    connect(m_lineColorButton, &QPushButton::clicked,
            this, &PropertiesPanel::onColorButtonClicked);
    m_formLayout->addRow("Line Color:", m_lineColorButton);

    // Line style
    m_lineStyleCombo = new QComboBox(m_formWidget);
    m_lineStyleCombo->addItem("Solid");
    m_lineStyleCombo->addItem("Dashed");
    m_lineStyleCombo->addItem("Dotted");
    connect(m_lineStyleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PropertiesPanel::onPropertyEdited);
    m_formLayout->addRow("Line Style:", m_lineStyleCombo);
}

void PropertiesPanel::createGeometryProperties()
{
    // TODO: Create geometry-specific property widgets based on object type
    // - Line: start point, end point, length, angle
    // - Circle: center, radius
    // - Rectangle: position, width, height
    // - Bezier: control points
}

void PropertiesPanel::setDocument(Document* document)
{
    // Disconnect old document
    if (m_document) {
        disconnect(m_document, nullptr, this, nullptr);
    }

    m_document = document;

    // Connect to document signals
    if (m_document) {
        connect(m_document, &Document::selectionChanged,
                this, [this]() {
            setSelectedObjects(m_document->selectedObjects());
        });

        // Update layer combo when layers change
        connect(m_document, &Document::layerAdded,
                this, [this]() {
            updateProperties();
        });
        connect(m_document, &Document::layerRemoved,
                this, [this]() {
            updateProperties();
        });
        connect(m_document, &Document::layerRenamed,
                this, [this]() {
            updateProperties();
        });
    }

    clearSelection();
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
    // Block signals while updating to avoid triggering onPropertyEdited
    m_nameEdit->blockSignals(true);
    m_layerCombo->blockSignals(true);
    m_lineStyleCombo->blockSignals(true);

    if (m_selectedObjects.isEmpty()) {
        // No selection - disable panel
        m_formWidget->setEnabled(false);
        m_titleLabel->setText("Properties");

        // Clear property values
        m_nameEdit->clear();
        m_widthEdit->setValue(0);
        m_heightEdit->setValue(0);
        m_layerCombo->clear();
    } else if (m_selectedObjects.size() == 1) {
        // Single selection - show properties
        m_formWidget->setEnabled(true);
        m_titleLabel->setText("Properties (1 object)");

        Geometry::GeometryObject* obj = m_selectedObjects.first();

        // Load common properties
        m_nameEdit->setText(obj->name());

        // Update layer combo with all available layers
        if (m_document) {
            m_layerCombo->clear();
            m_layerCombo->addItems(m_document->layers());
            m_layerCombo->setCurrentText(obj->layer());
        }

        m_lineStyleCombo->setCurrentIndex(static_cast<int>(obj->lineStyle()));

        // Update color button
        QColor color = obj->lineColor();
        QString colorStyle = QString("background-color: %1").arg(color.name());
        m_lineColorButton->setStyleSheet(colorStyle);
        m_lineColorButton->setText(color.name());

        // Show bounding box dimensions (always visible, read-only)
        QRectF bounds = obj->boundingRect();
        m_widthEdit->setValue(bounds.width());
        m_heightEdit->setValue(bounds.height());
    } else {
        // Multiple selection - show common properties
        m_formWidget->setEnabled(true);
        m_titleLabel->setText(QString("Properties (%1 objects)").arg(m_selectedObjects.size()));

        m_nameEdit->clear();
        m_nameEdit->setPlaceholderText("[Multiple objects]");

        // Calculate combined bounding box
        QRectF combinedBounds;
        for (auto* obj : m_selectedObjects) {
            QRectF bounds = obj->boundingRect();
            if (combinedBounds.isNull()) {
                combinedBounds = bounds;
            } else {
                combinedBounds = combinedBounds.united(bounds);
            }
        }
        m_widthEdit->setValue(combinedBounds.width());
        m_heightEdit->setValue(combinedBounds.height());

        // Show first object's properties for layer and style
        if (!m_selectedObjects.isEmpty()) {
            Geometry::GeometryObject* first = m_selectedObjects.first();

            if (m_document) {
                m_layerCombo->clear();
                m_layerCombo->addItems(m_document->layers());
                m_layerCombo->setCurrentText(first->layer());
            }

            m_lineStyleCombo->setCurrentIndex(static_cast<int>(first->lineStyle()));

            QColor color = first->lineColor();
            QString colorStyle = QString("background-color: %1").arg(color.name());
            m_lineColorButton->setStyleSheet(colorStyle);
            m_lineColorButton->setText(color.name());
        }
    }

    // Unblock signals
    m_nameEdit->blockSignals(false);
    m_layerCombo->blockSignals(false);
    m_lineStyleCombo->blockSignals(false);
}

void PropertiesPanel::onPropertyEdited()
{
    if (m_selectedObjects.isEmpty() || !m_document) {
        return;
    }

    // Determine which property was edited
    QObject* senderObj = sender();
    QString propertyName;
    QVariant value;

    if (senderObj == m_nameEdit) {
        propertyName = "name";
        value = m_nameEdit->text();
    } else if (senderObj == m_lineStyleCombo) {
        propertyName = "lineStyle";
        value = m_lineStyleCombo->currentIndex();
    } else if (senderObj == m_layerCombo) {
        propertyName = "layer";
        value = m_layerCombo->currentText();
    } else {
        return;
    }

    // Create and execute command
    UpdatePropertyCommand* cmd = new UpdatePropertyCommand(
        m_selectedObjects, propertyName, value);
    m_document->undoStack()->push(cmd);

    emit propertyChanged(propertyName, value);
}

void PropertiesPanel::onColorButtonClicked()
{
    if (m_selectedObjects.isEmpty() || !m_document) {
        return;
    }

    // Get current color from first selected object
    QColor currentColor = m_selectedObjects.first()->lineColor();

    // Show color dialog
    QColor newColor = QColorDialog::getColor(currentColor, this, "Choose Line Color");

    if (newColor.isValid() && newColor != currentColor) {
        // Create and execute command
        UpdatePropertyCommand* cmd = new UpdatePropertyCommand(
            m_selectedObjects, "lineColor", newColor);
        m_document->undoStack()->push(cmd);

        // Update button appearance
        QString colorStyle = QString("background-color: %1").arg(newColor.name());
        m_lineColorButton->setStyleSheet(colorStyle);
        m_lineColorButton->setText(newColor.name());

        emit propertyChanged("lineColor", newColor);
    }
}

} // namespace UI
} // namespace PatternCAD
