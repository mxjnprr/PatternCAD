/**
 * ToolPalette.cpp
 *
 * Implementation of ToolPalette widget
 */

#include "ToolPalette.h"
#include <QIcon>
#include <QLabel>

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
    , m_bezierButton(nullptr)
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
    m_layout->setSpacing(4);

    // Create button group
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);
    connect(m_buttonGroup, &QButtonGroup::idClicked,
            this, &ToolPalette::onToolButtonClicked);

    // Title label
    QLabel* titleLabel = new QLabel("Tools", this);
    titleLabel->setStyleSheet("font-weight: bold;");
    m_layout->addWidget(titleLabel);

    // Create tool buttons
    m_selectButton = createToolButton("Select", "", "Select Tool (S)", 0);
    m_lineButton = createToolButton("Line", "", "Line Tool (L)", 1);
    m_circleButton = createToolButton("Circle", "", "Circle Tool (C)", 2);
    m_rectangleButton = createToolButton("Rectangle", "", "Rectangle Tool (R)", 3);
    m_bezierButton = createToolButton("Bezier", "", "Bezier Curve Tool (B)", 4);

    // Add stretch at the end
    m_layout->addStretch();

    // Set default tool
    m_selectButton->setChecked(true);
}

QToolButton* ToolPalette::createToolButton(const QString& name,
                                           const QString& iconPath,
                                           const QString& tooltip,
                                           int id)
{
    QToolButton* button = new QToolButton(this);
    button->setText(name);
    button->setToolTip(tooltip);
    button->setCheckable(true);
    button->setMinimumSize(60, 40);

    // TODO: Load icon from resources
    // if (!iconPath.isEmpty()) {
    //     button->setIcon(QIcon(iconPath));
    //     button->setIconSize(QSize(24, 24));
    // }

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
        } else if (toolName == "Bezier") {
            m_bezierButton->setChecked(true);
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
        case 4: toolName = "Bezier"; break;
        default: toolName = "Select"; break;
    }

    m_currentToolName = toolName;
    emit toolSelected(toolName);
}

} // namespace UI
} // namespace PatternCAD
