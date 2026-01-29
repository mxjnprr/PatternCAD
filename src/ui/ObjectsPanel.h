/**
 * ObjectsPanel.h
 *
 * Panel displaying all objects in the document with ability to select and zoom
 */

#ifndef PATTERNCAD_OBJECTSPANEL_H
#define PATTERNCAD_OBJECTSPANEL_H

#include <QDockWidget>
#include <QTreeWidget>

namespace PatternCAD {

class Document;

namespace Geometry {
    class GeometryObject;
}

namespace UI {

class ObjectsPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit ObjectsPanel(QWidget* parent = nullptr);
    ~ObjectsPanel();

    void setDocument(Document* document);

public slots:
    void refresh();

signals:
    void objectSelected(Geometry::GeometryObject* object);
    void zoomToObject(Geometry::GeometryObject* object);

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private:
    void setupUI();
    void populateTree();

    Document* m_document;
    QTreeWidget* m_tree;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_OBJECTSPANEL_H
