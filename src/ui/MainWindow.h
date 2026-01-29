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
class QDockWidget;

namespace PatternCAD {

class Project;
class AutoSaveManager;

namespace Tools {
    class Tool;
}

namespace UI {

class Canvas;
class ToolPalette;
class PropertiesPanel;
class ObjectsPanel;
class LayersPanel;
class ParametersPanel;

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
    void onFileImportSVG();
    void onFileImportDXF();
    void onFileExportSVG();
    void onFileExportDXF();
    void onFileExportPDF();
    void onFileExit();
    void onFileOpenRecent();

    // Edit menu
    void onEditUndo();
    void onEditRedo();
    void onEditDelete();
    void onEditSelectAll();
    void onEditDeselect();
    void onEditPreferences();

    // View menu
    void onViewZoomIn();
    void onViewZoomOut();
    void onViewZoomFit();
    void onViewZoomSelection();
    void onViewZoomActual();
    void onViewToggleGrid();
    void onViewToggleSnap();

    // Modify menu
    void onModifyRotate();
    void onModifyMirror();
    void onModifyScale();
    void onModifyAlignLeft();
    void onModifyAlignRight();
    void onModifyAlignTop();
    void onModifyAlignBottom();
    void onModifyAlignCenterHorizontal();
    void onModifyAlignCenterVertical();
    void onModifyDistributeHorizontal();
    void onModifyDistributeVertical();

    // Help menu
    void onHelpKeyboardShortcuts();
    void onHelpAbout();

    // Tool selection
    void onToolSelected(const QString& toolName);
    void returnToSelectTool();

    // Project changes
    void onProjectChanged(Project* project);
    void onProjectModified(bool modified);

    // Dimension input
    void onDimensionInputRequested(const QString& mode, double initialLength, double initialAngle);

private:
    // UI setup
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();
    void setupWindowMenu();
    void loadSettings();
    void saveSettings();

    // Helper methods
    void updateWindowTitle();
    bool promptSaveChanges();
    void updateRecentFiles(const QString& filepath);
    void updateRecentFilesMenu();
    QString getDefaultDirectory() const;
    void checkForAutoSaveRecovery();

    // UI Components
    Canvas* m_canvas;
    ToolPalette* m_toolPalette;
    PropertiesPanel* m_propertiesPanel;
    LayersPanel* m_layersPanel;
    ParametersPanel* m_parametersPanel;
    ObjectsPanel* m_objectsPanel;

    // Dock widgets
    QDockWidget* m_toolsDock;
    QDockWidget* m_propertiesDock;
    QDockWidget* m_layersDock;
    QDockWidget* m_parametersDock;

    // Tool management
    QMap<QString, Tools::Tool*> m_tools;
    Tools::Tool* m_currentTool;

    // Dimension input
    QString m_currentDimensionMode;

    // Status bar widgets
    QLabel* m_statusLabel;
    QLabel* m_cursorLabel;
    QLabel* m_zoomLabel;
    class DimensionInputOverlay* m_dimensionInput;

    // Auto-save
    AutoSaveManager* m_autoSaveManager;

    // Recent files
    QMenu* m_recentFilesMenu;
    QList<QAction*> m_recentFileActions;
    static const int MaxRecentFiles = 10;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_MAINWINDOW_H
