/**
 * ToolPalette.cpp
 *
 * Implementation of ToolPalette widget
 */

#include "ToolPalette.h"
#include <QIcon>
#include <QLabel>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>

namespace PatternCAD {
namespace UI {

ToolPalette::ToolPalette(QWidget* parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_buttonGroup(nullptr)
    , m_currentToolName("Select")
    , m_selectButton(nullptr)
    , m_lineButton(nullptr)
    , m_circleButton(nullptr)
    , m_rectangleButton(nullptr)
    , m_polylineButton(nullptr)
{
    setupUi();
}

ToolPalette::~ToolPalette()
{
    // Qt handles cleanup of child widgets
}

void ToolPalette::setupUi()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setSpacing(2);

    // Create button group
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);
    connect(m_buttonGroup, &QButtonGroup::idClicked,
            this, &ToolPalette::onToolButtonClicked);

    // Title label
    QLabel* titleLabel = new QLabel("Tools", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 11pt;");
    m_layout->addWidget(titleLabel);

    // Selection Tools group
    addGroupSeparator("Selection");
    m_selectButton = createToolButton("Select", "select", "Select Tool (Z)", 0);

    // Drawing Tools group
    addGroupSeparator("Drawing");
    m_lineButton = createToolButton("Line", "line", "Line Tool (L)", 1);
    m_circleButton = createToolButton("Circle", "circle", "Circle Tool (C)", 2);
    m_rectangleButton = createToolButton("Rectangle", "rectangle", "Rectangle Tool (R)", 3);
    m_polylineButton = createToolButton("Polyline", "polyline", "Polyline/Draft Tool (D)", 4);

    // Add stretch at the end
    m_layout->addStretch();

    // Set default tool
    m_selectButton->setChecked(true);
}

QToolButton* ToolPalette::createToolButton(const QString& name,
                                           const QString& iconType,
                                           const QString& tooltip,
                                           int id)
{
    QToolButton* button = new QToolButton(this);
    button->setText(name);
    button->setToolTip(tooltip);
    button->setCheckable(true);
    button->setMinimumSize(70, 50);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // Create icon
    if (!iconType.isEmpty()) {
        QIcon icon = createToolIcon(iconType);
        button->setIcon(icon);
        button->setIconSize(QSize(32, 32));
    }

    m_buttonGroup->addButton(button, id);
    m_layout->addWidget(button);

    return button;
}

QString ToolPalette::currentToolName() const
{
    return m_currentToolName;
}

void ToolPalette::setCurrentTool(const QString& toolName)
{
    if (m_currentToolName != toolName) {
        m_currentToolName = toolName;

        // Update button state
        if (toolName == "Select") {
            m_selectButton->setChecked(true);
        } else if (toolName == "Line") {
            m_lineButton->setChecked(true);
        } else if (toolName == "Circle") {
            m_circleButton->setChecked(true);
        } else if (toolName == "Rectangle") {
            m_rectangleButton->setChecked(true);
        } else if (toolName == "Polyline") {
            m_polylineButton->setChecked(true);
        }

        emit toolSelected(toolName);
    }
}

void ToolPalette::onToolButtonClicked(int id)
{
    QString toolName;
    switch (id) {
        case 0: toolName = "Select"; break;
        case 1: toolName = "Line"; break;
        case 2: toolName = "Circle"; break;
        case 3: toolName = "Rectangle"; break;
        case 4: toolName = "Polyline"; break;
        default: toolName = "Select"; break;
    }

    m_currentToolName = toolName;
    emit toolSelected(toolName);
}

QIcon ToolPalette::createToolIcon(const QString& toolType)
{
    // Create a 32x32 pixmap
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(QColor(60, 60, 60), 2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    if (toolType == "select") {
        // Draw arrow cursor icon
        QPolygon arrow;
        arrow << QPoint(6, 6) << QPoint(6, 22) << QPoint(12, 18)
              << QPoint(16, 26) << QPoint(19, 24) << QPoint(15, 16)
              << QPoint(22, 14);
        painter.setBrush(QColor(60, 60, 60));
        painter.drawPolygon(arrow);
    } else if (toolType == "line") {
        // Draw line icon
        painter.drawLine(8, 24, 24, 8);
        painter.setBrush(QColor(60, 60, 60));
        painter.drawEllipse(QPoint(8, 24), 2, 2);
        painter.drawEllipse(QPoint(24, 8), 2, 2);
    } else if (toolType == "circle") {
        // Draw circle icon
        painter.drawEllipse(QPoint(16, 16), 10, 10);
    } else if (toolType == "rectangle") {
        // Draw rectangle icon
        painter.drawRect(6, 10, 20, 12);
    } else if (toolType == "polyline") {
        // Draw polyline icon (curved line with vertices)
        QPainterPath path;
        path.moveTo(6, 22);
        path.cubicTo(10, 10, 18, 10, 22, 22);
        painter.drawPath(path);
        painter.setBrush(QColor(60, 60, 60));
        painter.drawEllipse(QPoint(6, 22), 2, 2);
        painter.drawEllipse(QPoint(14, 10), 2, 2);
        painter.drawEllipse(QPoint(22, 22), 2, 2);
    }

    painter.end();
    return QIcon(pixmap);
}

void ToolPalette::addGroupSeparator(const QString& groupName)
{
    // Add label for group
    QLabel* groupLabel = new QLabel(groupName, this);
    groupLabel->setStyleSheet("color: #666; font-size: 9pt; margin-top: 8px;");
    m_layout->addWidget(groupLabel);

    // Add separator line
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #ccc;");
    line->setMaximumHeight(1);
    m_layout->addWidget(line);
}

} // namespace UI
} // namespace PatternCAD
