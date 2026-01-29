/**
 * ObjectsPanel.cpp
 *
 * Implementation of ObjectsPanel
 */

#include "ObjectsPanel.h"
#include "core/Document.h"
#include "geometry/GeometryObject.h"
#include <QVBoxLayout>
#include <QHeaderView>

namespace PatternCAD {
namespace UI {

ObjectsPanel::ObjectsPanel(QWidget* parent)
    : QDockWidget(tr("Objects"), parent)
    , m_document(nullptr)
    , m_tree(nullptr)
{
    setupUI();
}

ObjectsPanel::~ObjectsPanel()
{
}

void ObjectsPanel::setupUI()
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    // Tree widget
    m_tree = new QTreeWidget(widget);
    m_tree->setHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Layer"));
    m_tree->setAlternatingRowColors(true);
    m_tree->setRootIsDecorated(false);
    m_tree->header()->setStretchLastSection(false);
    m_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    layout->addWidget(m_tree);
    setWidget(widget);

    // Connect signals
    connect(m_tree, &QTreeWidget::itemClicked, this, &ObjectsPanel::onItemClicked);
    connect(m_tree, &QTreeWidget::itemDoubleClicked, this, &ObjectsPanel::onItemDoubleClicked);
}

void ObjectsPanel::setDocument(Document* document)
{
    if (m_document) {
        disconnect(m_document, nullptr, this, nullptr);
    }

    m_document = document;

    if (m_document) {
        connect(m_document, &Document::objectAdded, this, &ObjectsPanel::refresh);
        connect(m_document, &Document::objectRemoved, this, &ObjectsPanel::refresh);
    }

    refresh();
}

void ObjectsPanel::refresh()
{
    m_tree->clear();

    if (!m_document) {
        return;
    }

    populateTree();
}

void ObjectsPanel::populateTree()
{
    const QList<Geometry::GeometryObject*>& objects = m_document->objects();

    for (Geometry::GeometryObject* obj : objects) {
        QTreeWidgetItem* item = new QTreeWidgetItem(m_tree);
        
        // Name
        QString name = obj->name();
        if (name.isEmpty()) {
            name = QString("%1 #%2").arg(obj->typeName()).arg(obj->id());
        }
        item->setText(0, name);
        
        // Type
        item->setText(1, obj->typeName());
        
        // Layer
        item->setText(2, obj->layer());
        
        // Store object pointer
        item->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(obj)));
        
        // Visual feedback for visibility
        if (!obj->isVisible()) {
            item->setForeground(0, Qt::gray);
            item->setForeground(1, Qt::gray);
            item->setForeground(2, Qt::gray);
        }
    }
}

void ObjectsPanel::onItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    
    if (!item) {
        return;
    }
    
    // Get object pointer
    quintptr ptr = item->data(0, Qt::UserRole).value<quintptr>();
    Geometry::GeometryObject* obj = reinterpret_cast<Geometry::GeometryObject*>(ptr);
    
    if (obj) {
        emit objectSelected(obj);
    }
}

void ObjectsPanel::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    
    if (!item) {
        return;
    }
    
    // Get object pointer
    quintptr ptr = item->data(0, Qt::UserRole).value<quintptr>();
    Geometry::GeometryObject* obj = reinterpret_cast<Geometry::GeometryObject*>(ptr);
    
    if (obj) {
        emit zoomToObject(obj);
    }
}

} // namespace UI
} // namespace PatternCAD
