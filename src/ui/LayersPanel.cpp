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
#include <QColorDialog>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>

namespace PatternCAD {
namespace UI {

LayersPanel::LayersPanel(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_layerList(nullptr)
    , m_addButton(nullptr)
    , m_removeButton(nullptr)
    , m_renameButton(nullptr)
    , m_colorButton(nullptr)
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
    connect(m_layerList, &QListWidget::itemDoubleClicked,
            this, &LayersPanel::onLayerDoubleClicked);
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

    m_colorButton = new QPushButton("Color", this);
    m_colorButton->setToolTip("Change Layer Color");
    connect(m_colorButton, &QPushButton::clicked,
            this, &LayersPanel::onChangeColor);
    buttonLayout->addWidget(m_colorButton);

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
    // Disconnect old document if any
    if (m_document) {
        disconnect(m_document, nullptr, this, nullptr);
    }

    m_document = document;

    // Connect document signals
    if (m_document) {
        connect(m_document, &Document::layerAdded,
                this, &LayersPanel::refreshLayers);
        connect(m_document, &Document::layerRemoved,
                this, &LayersPanel::refreshLayers);
        connect(m_document, &Document::layerRenamed,
                this, [this](const QString&, const QString&) {
            refreshLayers();
        });
        connect(m_document, &Document::activeLayerChanged,
                this, [this](const QString& layerName) {
            m_activeLayer = layerName;
            refreshLayers();
        });
        connect(m_document, &Document::objectAdded,
                this, &LayersPanel::refreshLayers);
        connect(m_document, &Document::objectRemoved,
                this, &LayersPanel::refreshLayers);
    }

    refreshLayers();
}

void LayersPanel::refreshLayers()
{
    // Block signals during refresh to avoid triggering change events
    m_layerList->blockSignals(true);
    m_layerList->clear();

    if (!m_document) {
        // Add default layer when no document
        QListWidgetItem* defaultItem = new QListWidgetItem("Default", m_layerList);
        defaultItem->setFlags(defaultItem->flags() | Qt::ItemIsUserCheckable);
        defaultItem->setCheckState(Qt::Checked);
        defaultItem->setSelected(true);
        m_layerList->blockSignals(false);
        return;
    }

    // Load layers from document
    for (const QString& layerName : m_document->layers()) {
        QListWidgetItem* item = new QListWidgetItem(m_layerList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(m_document->isLayerVisible(layerName) ? Qt::Checked : Qt::Unchecked);

        // Create colored icon for the layer
        QColor layerColor = m_document->layerColor(layerName);
        QPixmap pixmap(16, 16);
        pixmap.fill(layerColor);
        QPainter painter(&pixmap);
        painter.setPen(Qt::black);
        painter.drawRect(0, 0, 15, 15);
        item->setIcon(QIcon(pixmap));

        // Count objects on this layer
        int objectCount = m_document->objectsOnLayer(layerName).size();
        item->setText(QString("%1 (%2)").arg(layerName).arg(objectCount));

        if (layerName == m_document->activeLayer()) {
            item->setSelected(true);
            m_activeLayer = layerName;
        }
    }

    m_layerList->blockSignals(false);
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
        // Add layer to document (which will trigger refresh via signal)
        if (m_document) {
            // Check if layer already exists
            if (m_document->layers().contains(layerName)) {
                QMessageBox::warning(this, "Layer Exists",
                                   "A layer with this name already exists.");
                return;
            }

            // Generate a random color for the new layer
            int hue = QRandomGenerator::global()->bounded(360);
            QColor newColor = QColor::fromHsv(hue, 200, 200);
            m_document->addLayer(layerName, newColor);
        } else {
            // No document: add to UI only
            for (int i = 0; i < m_layerList->count(); ++i) {
                if (m_layerList->item(i)->text() == layerName) {
                    QMessageBox::warning(this, "Layer Exists",
                                       "A layer with this name already exists.");
                    return;
                }
            }

            QListWidgetItem* item = new QListWidgetItem(layerName, m_layerList);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);
        }
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
        // Remove layer from document (which will trigger refresh via signal)
        if (m_document) {
            m_document->removeLayer(layerName);
        } else {
            delete currentItem;
        }
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
        // Rename layer in document (which will trigger refresh via signal)
        if (m_document) {
            // Check if new name already exists
            if (m_document->layers().contains(newName)) {
                QMessageBox::warning(this, "Layer Exists",
                                   "A layer with this name already exists.");
                return;
            }

            m_document->renameLayer(oldName, newName);
        } else {
            // No document: rename in UI only
            for (int i = 0; i < m_layerList->count(); ++i) {
                if (m_layerList->item(i)->text() == newName) {
                    QMessageBox::warning(this, "Layer Exists",
                                       "A layer with this name already exists.");
                    return;
                }
            }

            currentItem->setText(newName);

            if (m_activeLayer == oldName) {
                m_activeLayer = newName;
                emit activeLayerChanged(newName);
            }
        }
    }
}

void LayersPanel::onLayerSelectionChanged()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (currentItem) {
        QString layerName = currentItem->text();
        m_activeLayer = layerName;

        // Update document's active layer
        if (m_document) {
            m_document->setActiveLayer(layerName);
        }

        emit activeLayerChanged(layerName);
    }
}

void LayersPanel::onLayerItemChanged(QListWidgetItem* item)
{
    if (item) {
        bool visible = (item->checkState() == Qt::Checked);
        QString layerName = item->text();

        // Update layer visibility in document
        if (m_document) {
            m_document->setLayerVisible(layerName, visible);
        }

        emit layerVisibilityChanged(layerName, visible);
    }
}

void LayersPanel::onChangeColor()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "No Selection",
                                "Please select a layer to change color.");
        return;
    }

    QString layerName = currentItem->text();
    QColor currentColor = m_document ? m_document->layerColor(layerName) : Qt::black;

    QColor newColor = QColorDialog::getColor(currentColor, this, "Choose Layer Color");
    if (newColor.isValid() && m_document) {
        m_document->setLayerColor(layerName, newColor);
        refreshLayers();
    }
}

void LayersPanel::onLayerDoubleClicked(QListWidgetItem* item)
{
    if (item) {
        QString layerName = item->text();
        QColor currentColor = m_document ? m_document->layerColor(layerName) : Qt::black;

        QColor newColor = QColorDialog::getColor(currentColor, this, "Choose Layer Color");
        if (newColor.isValid() && m_document) {
            m_document->setLayerColor(layerName, newColor);
            refreshLayers();
        }
    }
}

} // namespace UI
} // namespace PatternCAD
