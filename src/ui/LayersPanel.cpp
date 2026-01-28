/**
 * LayersPanel.cpp
 *
 * Implementation of LayersPanel widget
 */

#include "LayersPanel.h"
#include "core/Document.h"
#include "geometry/GeometryObject.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QRandomGenerator>
#include <QMenu>

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

    // Enable drag-and-drop reordering
    m_layerList->setDragDropMode(QAbstractItemView::InternalMove);
    m_layerList->setDefaultDropAction(Qt::MoveAction);

    // Enable context menu
    m_layerList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_layerList, &QWidget::customContextMenuRequested,
            this, &LayersPanel::onLayerContextMenu);

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

        // Store layer name and lock state in user data
        item->setData(Qt::UserRole, layerName);
        bool isLocked = m_document->isLayerLocked(layerName);
        item->setData(Qt::UserRole + 1, isLocked);

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
        QString displayText = QString("%1 (%2)").arg(layerName).arg(objectCount);

        // Add lock indicator if locked
        if (isLocked) {
            displayText += " 🔒";
        }

        item->setText(displayText);

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
        QString layerName = item->data(Qt::UserRole).toString();
        if (layerName.isEmpty()) {
            layerName = item->text().split(" (")[0]; // Extract name before count
        }
        QColor currentColor = m_document ? m_document->layerColor(layerName) : Qt::black;

        QColor newColor = QColorDialog::getColor(currentColor, this, "Choose Layer Color");
        if (newColor.isValid() && m_document) {
            m_document->setLayerColor(layerName, newColor);
            refreshLayers();
        }
    }
}

void LayersPanel::onLayerContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = m_layerList->itemAt(pos);
    if (!item || !m_document) {
        return;
    }

    QString layerName = item->data(Qt::UserRole).toString();
    bool isLocked = item->data(Qt::UserRole + 1).toBool();

    QMenu contextMenu(this);

    // Rename action
    QAction* renameAction = contextMenu.addAction("Rename Layer");
    connect(renameAction, &QAction::triggered, this, &LayersPanel::onRenameLayer);

    // Delete action
    QAction* deleteAction = contextMenu.addAction("Delete Layer");
    connect(deleteAction, &QAction::triggered, this, &LayersPanel::onRemoveLayer);

    contextMenu.addSeparator();

    // Lock/Unlock toggle
    QAction* lockAction = contextMenu.addAction(isLocked ? "Unlock Layer" : "Lock Layer");
    connect(lockAction, &QAction::triggered, this, [this, item, layerName, isLocked]() {
        if (m_document) {
            m_document->setLayerLocked(layerName, !isLocked);
            emit layerLockChanged(layerName, !isLocked);
            refreshLayers();
        }
    });

    contextMenu.addSeparator();

    // Merge Down action
    int currentRow = m_layerList->row(item);
    QAction* mergeDownAction = contextMenu.addAction("Merge Down");
    mergeDownAction->setEnabled(currentRow < m_layerList->count() - 1);
    connect(mergeDownAction, &QAction::triggered, this, &LayersPanel::onMergeDown);

    // Duplicate Layer action
    QAction* duplicateAction = contextMenu.addAction("Duplicate Layer");
    connect(duplicateAction, &QAction::triggered, this, &LayersPanel::onDuplicateLayer);

    contextMenu.addSeparator();

    // Select All Objects action
    QAction* selectAllAction = contextMenu.addAction("Select All Objects");
    connect(selectAllAction, &QAction::triggered, this, &LayersPanel::onSelectAllObjects);

    contextMenu.exec(m_layerList->mapToGlobal(pos));
}

void LayersPanel::onMergeDown()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (!currentItem || !m_document) {
        return;
    }

    int currentRow = m_layerList->row(currentItem);
    if (currentRow >= m_layerList->count() - 1) {
        QMessageBox::information(this, "Cannot Merge", "Cannot merge the bottom layer.");
        return;
    }

    QString currentLayerName = currentItem->data(Qt::UserRole).toString();
    QListWidgetItem* nextItem = m_layerList->item(currentRow + 1);
    QString nextLayerName = nextItem->data(Qt::UserRole).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Merge Layers",
        QString("Merge layer '%1' into '%2'?").arg(currentLayerName, nextLayerName),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        // Move all objects from current layer to next layer
        auto objects = m_document->objectsOnLayer(currentLayerName);
        for (auto* obj : objects) {
            obj->setLayer(nextLayerName);
        }

        // Remove the current layer
        m_document->removeLayer(currentLayerName);
        refreshLayers();
    }
}

void LayersPanel::onDuplicateLayer()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (!currentItem || !m_document) {
        return;
    }

    QString layerName = currentItem->data(Qt::UserRole).toString();
    QString newLayerName = layerName + " Copy";

    // Ensure unique name
    int copyNum = 1;
    while (m_document->layers().contains(newLayerName)) {
        newLayerName = QString("%1 Copy %2").arg(layerName).arg(++copyNum);
    }

    // Add new layer with same color
    QColor layerColor = m_document->layerColor(layerName);
    m_document->addLayer(newLayerName, layerColor);

    // Note: Objects are not duplicated, only the layer itself
    // If you want to duplicate objects too, you'd need to implement that here

    refreshLayers();
}

void LayersPanel::onSelectAllObjects()
{
    QListWidgetItem* currentItem = m_layerList->currentItem();
    if (!currentItem || !m_document) {
        return;
    }

    QString layerName = currentItem->data(Qt::UserRole).toString();
    QList<Geometry::GeometryObject*> layerObjects = m_document->objectsOnLayer(layerName);

    if (layerObjects.isEmpty()) {
        QMessageBox::information(this, "No Objects", "This layer has no objects.");
        return;
    }

    m_document->setSelectedObjects(layerObjects);
}

} // namespace UI
} // namespace PatternCAD
