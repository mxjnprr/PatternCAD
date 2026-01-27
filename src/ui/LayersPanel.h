/**
 * LayersPanel.h
 *
 * QWidget for layer management and organization
 */

#ifndef PATTERNCAD_LAYERSPANEL_H
#define PATTERNCAD_LAYERSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QString>

namespace PatternCAD {

// Forward declarations
class Document;

namespace UI {

/**
 * LayersPanel provides layer management:
 * - List of layers with visibility toggles
 * - Active layer selection
 * - Add, remove, rename layers
 * - Layer reordering (z-order)
 * - Lock/unlock layers
 */
class LayersPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LayersPanel(QWidget* parent = nullptr);
    ~LayersPanel();

    // Document
    Document* document() const;
    void setDocument(Document* document);

    // Layer management
    void refreshLayers();

signals:
    void activeLayerChanged(const QString& layerName);
    void layerVisibilityChanged(const QString& layerName, bool visible);

private slots:
    void onAddLayer();
    void onRemoveLayer();
    void onRenameLayer();
    void onLayerSelectionChanged();
    void onLayerItemChanged(QListWidgetItem* item);

private:
    // UI setup
    void setupUi();
    void updateLayerList();

    // Private members
    QVBoxLayout* m_layout;
    QListWidget* m_layerList;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QPushButton* m_renameButton;
    Document* m_document;
    QString m_activeLayer;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_LAYERSPANEL_H
