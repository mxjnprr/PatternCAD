/**
 * MainWindow.h
 *
 * Main application window
 */

#ifndef PATTERNCAD_MAINWINDOW_H
#define PATTERNCAD_MAINWINDOW_H

#include <QMainWindow>
#include <QString>

// Forward declarations
class QLabel;

namespace PatternCAD {

class Project;

namespace UI {

class Canvas;
class ToolPalette;
class PropertiesPanel;
class LayersPanel;

/**
 * MainWindow provides the main application interface with:
 * - Menu bar and toolbars
 * - Central canvas for pattern editing
 * - Dockable panels (tools, properties, layers)
 * - Status bar
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // File operations
    void openFile(const QString& filepath);
    void saveFile();
    void saveFileAs();
    void newProject();

protected:
    // Event handlers
    void closeEvent(QCloseEvent* event) override;

private slots:
    // File menu
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onFileExit();

    // Edit menu
    void onEditUndo();
    void onEditRedo();

    // View menu
    void onViewZoomIn();
    void onViewZoomOut();
    void onViewZoomFit();

    // Help menu
    void onHelpAbout();

    // Project changes
    void onProjectChanged(Project* project);
    void onProjectModified(bool modified);

private:
    // UI setup
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();
    void loadSettings();
    void saveSettings();

    // Helper methods
    void updateWindowTitle();
    bool promptSaveChanges();

    // UI Components
    Canvas* m_canvas;
    ToolPalette* m_toolPalette;
    PropertiesPanel* m_propertiesPanel;
    LayersPanel* m_layersPanel;

    // Status bar widgets
    QLabel* m_statusLabel;
    QLabel* m_cursorLabel;
    QLabel* m_zoomLabel;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_MAINWINDOW_H
