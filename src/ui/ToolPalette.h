/**
 * ToolPalette.h
 *
 * QWidget for drawing tool selection
 */

#ifndef PATTERNCAD_TOOLPALETTE_H
#define PATTERNCAD_TOOLPALETTE_H

#include <QWidget>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QString>

namespace PatternCAD {

// Forward declarations
namespace Tools {
    class Tool;
}

namespace UI {

/**
 * ToolPalette provides a palette of drawing tools:
 * - Selection tool
 * - Line tool
 * - Circle tool
 * - Rectangle tool
 * - Bezier curve tool
 * - Measurement tools
 * - Transform tools
 */
class ToolPalette : public QWidget
{
    Q_OBJECT

public:
    explicit ToolPalette(QWidget* parent = nullptr);
    ~ToolPalette();

    // Current tool
    QString currentToolName() const;
    void setCurrentTool(const QString& toolName);

signals:
    void toolSelected(const QString& toolName);

private slots:
    void onToolButtonClicked(int id);

private:
    // UI setup
    void setupUi();
    QToolButton* createToolButton(const QString& name,
                                   const QString& iconPath,
                                   const QString& tooltip,
                                   int id);
    QIcon createToolIcon(const QString& toolType);
    void addGroupSeparator(const QString& groupName);

    // Private members
    QVBoxLayout* m_layout;
    QButtonGroup* m_buttonGroup;
    QString m_currentToolName;

    // Tool buttons
    QToolButton* m_selectButton;
    QToolButton* m_lineButton;
    QToolButton* m_circleButton;
    QToolButton* m_rectangleButton;
    QToolButton* m_polylineButton;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_TOOLPALETTE_H
