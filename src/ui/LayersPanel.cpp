/**
 * LayersPanel.cpp
 *
 * Implementation of LayersPanel widget
 */

#include "LayersPanel.h"
#include "core/Document.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>

namespace PatternCAD {
namespace UI {

LayersPanel::LayersPanel(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_layerList(nullptr)
    , m_addButton(nullptr)
    , m_removeButton(nullptr)
    , m_renameButton(nullptr)
    , m_document(nullptr)
    , m_activeLayer("Default")
{
    setupUi();
}

LayersPanel::~LayersPanel()
{
    // Qt handles cleanup of child widgets
}

void LayersPanel::setupUi()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setSpacing(4);

    // Title label
    QLabel* titleLabel = new QLabel("Layers", this);
    titleLabel->setStyleSheet("font-weight: bold;");
    m_layout->addWidget(titleLabel);

    // Layer list
    m_layerList = new QListWidget(this);
    m_layerList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_layerList, &QListWidget::itemSelectionChanged,
            this, &LayersPanel::onLayerSelectionChanged);
    connect(m_layerList, &QListWidget::itemChanged,
            this, &LayersPanel::onLayerItemChanged);
    m_layout->addWidget(m_layerList);

    // Button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(4);

    m_addButton = new QPushButton("+", this);
    m_addButton->setToolTip("Add Layer");
    m_addButton->setMaximumWidth(30);
    connect(m_addButton, &QPushButton::clicked,
            this, &LayersPanel::onAddLayer);
    buttonLayout->addWidget(m_addButton);

    m_removeButton = new QPushButton("-", this);
    m_removeButton->setToolTip("Remove Layer");
    m_removeButton->setMaximumWidth(30);
    connect(m_removeButton, &QPushButton::clicked,
            this, &LayersPanel::onRemoveLayer);
    buttonLayout->addWidget(m_removeButton);

    m_renameButton = new QPushButton("Rename", this);
    m_renameButton->setToolTip("Rename Layer");
    connect(m_renameButton, &QPushButton::clicked,
            this, &LayersPanel::onRenameLayer);
    buttonLayout->addWidget(m_renameButton);

    buttonLayout->addStretch();
    m_layout->addLayout(buttonLayout);

    // Add default layer
    QListWidgetItem* defaultItem = new QListWidgetItem("Default", m_layerList);
    defaultItem->setFlags(defaultItem->flags() | Qt::ItemIsUserCheckable);
    defaultItem->setCheckState(Qt::Checked);
    defaultItem->setSelected(true);
}

Document* LayersPanel::document() const
{
    return m_document;
}

void LayersPanel::setDocument(Document* document)
{
    m_document = document;
    refreshLayers();

    // TODO: Connect document signals for layer changes
}

void LayersPanel::refreshLayers()
{
    m_layerList->clear();

    if (!m_document) {
        // Add default layer when no document
        QListWidgetItem* defaultItem = new QListWidgetItem("Default", m_layerList);
        defaultItem->setFlags(defaultItem->flags() | Qt::ItemIsUserCheckable);
        defaultItem->setCheckState(Qt::Checked);
        defaultItem->setSelected(true);
        return;
    }

    // TODO: Load layers from document
    // for (const auto& layer : document->layers()) {
    //     QListWidgetItem* item = new QListWidgetItem(layer->name(), m_layerList);
    //     item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    //     item->setCheckState(layer->isVisible() ? Qt::Checked : Qt::Unchecked);
    //     if (layer->name() == m_activeLayer) {
    //         item->setSelected(true);
    //     }
    // }
}

void LayersPanel::updateLayerList()
{
    refreshLayers();
}

void LayersPanel::onAddLayer()
{
    bool ok;
    QString layerName = QInputDialog::getText(this,
                                              "Add Layer",
                                              "Layer name:",
                                              QLineEdit::Normal,
                                              "New Layer",
                                              &ok);

    if (ok && !layerName.isEmpty()) {
        // Check if layer already exists
        for (int i = 0; i < m_layerList->count(); ++i) {
            if (m_layerList->item(i)->text() == layerName) {
                QMessageBox::warning(this, "Layer Exists",
                                   "A layer with this name already exists.");
                return;
            }
        }

        // Add new layer
        QListWidgetItem* item = new QListWidgetItem(layerName, m_layerList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);

        // TODO: Add layer to document
        // if (m_document) {
        //     m_document->addLayer(layerName);
        // }
    }
}

void LayersPanel::onRemoveLayer()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "No Selection",
                                "Please select a layer to remove.");
        return;
    }

    // Prevent removal of last layer
    if (m_layerList->count() <= 1) {
        QMessageBox::warning(this, "Cannot Remove",
                           "Cannot remove the last layer.");
        return;
    }

    QString layerName = currentItem->text();
    int result = QMessageBox::question(this, "Remove Layer",
                                       QString("Remove layer '%1'?").arg(layerName),
                                       QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        // TODO: Remove layer from document
        // if (m_document) {
        //     m_document->removeLayer(layerName);
        // }

        delete currentItem;
    }
}

void LayersPanel::onRenameLayer()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "No Selection",
                                "Please select a layer to rename.");
        return;
    }

    QString oldName = currentItem->text();
    bool ok;
    QString newName = QInputDialog::getText(this,
                                           "Rename Layer",
                                           "New layer name:",
                                           QLineEdit::Normal,
                                           oldName,
                                           &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        // Check if new name already exists
        for (int i = 0; i < m_layerList->count(); ++i) {
            if (m_layerList->item(i)->text() == newName) {
                QMessageBox::warning(this, "Layer Exists",
                                   "A layer with this name already exists.");
                return;
            }
        }

        // Rename layer
        currentItem->setText(newName);

        // TODO: Rename layer in document
        // if (m_document) {
        //     m_document->renameLayer(oldName, newName);
        // }

        if (m_activeLayer == oldName) {
            m_activeLayer = newName;
            emit activeLayerChanged(newName);
        }
    }
}

void LayersPanel::onLayerSelectionChanged()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (currentItem) {
        m_activeLayer = currentItem->text();
        emit activeLayerChanged(m_activeLayer);
    }
}

void LayersPanel::onLayerItemChanged(QListWidgetItem* item)
{
    if (item) {
        bool visible = (item->checkState() == Qt::Checked);
        emit layerVisibilityChanged(item->text(), visible);

        // TODO: Update layer visibility in document
        // if (m_document) {
        //     m_document->setLayerVisible(item->text(), visible);
        // }
    }
}

} // namespace UI
} // namespace PatternCAD
