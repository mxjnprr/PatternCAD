/**
 * MainWindow.cpp
 *
 * Implementation of MainWindow
 */

#include "MainWindow.h"
#include "Canvas.h"
#include "ToolPalette.h"
#include "PropertiesPanel.h"
#include "LayersPanel.h"
#include "ParametersPanel.h"
#include "ObjectsPanel.h"
#include "KeyboardShortcutsDialog.h"
#include "PreferencesDialog.h"
#include "DimensionInputOverlay.h"
#include "RecoveryDialog.h"
#include "../core/Application.h"
#include "../core/Project.h"
#include "../core/Document.h"
#include "../core/Commands.h"
#include "../core/Units.h"
#include "../core/AutoSaveManager.h"
#include "../core/SettingsManager.h"
#include "../io/SVGFormat.h"
#include "../io/PDFFormat.h"
#include "../io/DXFFormat.h"
#include "../tools/SelectTool.h"
#include "../tools/PolylineTool.h"
#include "../tools/AddPointOnContourTool.h"
#include "../tools/RotateTool.h"
#include "../tools/MirrorTool.h"
#include "../tools/ScaleTool.h"
#include "../tools/SeamAllowanceTool.h"
#include "../tools/NotchTool.h"
#include "../tools/MatchPointTool.h"
#include "GradingDialog.h"
#include "ScalePatternDialog.h"
#include "../core/GradingCommands.h"
#include "../geometry/Polyline.h"
#include "../geometry/GradingSystem.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QFile>

namespace PatternCAD {
namespace UI {

// Define the static constant
const int MainWindow::MaxRecentFiles;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_canvas(nullptr)
    , m_toolPalette(nullptr)
    , m_propertiesPanel(nullptr)
    , m_layersPanel(nullptr)
    , m_parametersPanel(nullptr)
    , m_currentTool(nullptr)
    , m_statusLabel(nullptr)
    , m_cursorLabel(nullptr)
    , m_zoomLabel(nullptr)
    , m_dimensionInput(nullptr)
    , m_autoSaveManager(nullptr)
    , m_recentFilesMenu(nullptr)
{
    setupUi();
    loadSettings();

    // Connect to application
    Application* app = Application::instance();
    connect(app, &Application::projectChanged, this, &MainWindow::onProjectChanged);

    // Create default project
    Project* project = new Project();
    app->setCurrentProject(project);

    // Create a document (for now, separate from project)
    Document* document = new Document(this);
    m_canvas->setDocument(document);
    m_layersPanel->setDocument(document);
    m_propertiesPanel->setDocument(document);
    m_objectsPanel->setDocument(document);

    // Connect ObjectsPanel signals
    connect(m_objectsPanel, &ObjectsPanel::objectSelected, this, [this](Geometry::GeometryObject* obj) {
        if (m_canvas->document()) {
            m_canvas->document()->clearSelection();
            obj->setSelected(true);
            m_canvas->update();
        }
    });
    connect(m_objectsPanel, &ObjectsPanel::zoomToObject, m_canvas, &Canvas::zoomToObject);

    // Setup auto-save
    m_autoSaveManager = new AutoSaveManager(this);
    m_autoSaveManager->setDocument(document);
    connect(m_autoSaveManager, &AutoSaveManager::autoSaveCompleted,
            this, [this](const QString& filePath) {
        Q_UNUSED(filePath);
        statusBar()->showMessage(tr("Auto-saved"), 2000);
    });
    connect(m_autoSaveManager, &AutoSaveManager::autoSaveFailed,
            this, [this](const QString& error) {
        statusBar()->showMessage(tr("Auto-save failed: %1").arg(error), 3000);
    });

    // Initialize tools
    m_tools["Select"] = new Tools::SelectTool(this);
    m_tools["Polyline"] = new Tools::PolylineTool(this);
    m_tools["AddPointOnContour"] = new Tools::AddPointOnContourTool(this);
    m_tools["Rotate"] = new Tools::RotateTool(this);
    m_tools["Mirror"] = new Tools::MirrorTool(this);
    m_tools["Scale"] = new Tools::ScaleTool(this);
    m_tools["SeamAllowance"] = new Tools::SeamAllowanceTool(this);
    m_tools["Notch"] = new Tools::NotchTool(this);
    m_tools["MatchPoint"] = new Tools::MatchPointTool(this);

    // Connect SelectTool's toolChangeRequested signal
    auto* selectTool = qobject_cast<Tools::SelectTool*>(m_tools["Select"]);
    if (selectTool) {
        connect(selectTool, &Tools::SelectTool::toolChangeRequested,
                this, &MainWindow::onToolSelected);
    }

    // Set document for all tools and connect status messages
    for (auto* tool : m_tools) {
        tool->setDocument(document);
        connect(tool, &Tools::Tool::statusMessage,
                this, [this](const QString& msg) {
            statusBar()->showMessage(msg, 0);  // 0 = permanent until next message
        });
        connect(tool, &Tools::Tool::dimensionInputRequested,
                this, &MainWindow::onDimensionInputRequested);
    }

    // Connect tools to return to Select after creating an object
    // Except for AddPointOnContour which stays active for continuous editing
    for (auto it = m_tools.begin(); it != m_tools.end(); ++it) {
        if (it.key() != "Select" && it.key() != "AddPointOnContour") {
            connect(it.value(), &Tools::Tool::objectCreated,
                    this, &MainWindow::returnToSelectTool);
        }
    }

    // Set default tool to Select
    m_currentTool = m_tools["Select"];
    m_canvas->setActiveTool(m_currentTool);

    updateWindowTitle();

    // Check for auto-save recovery after window is shown
    QTimer::singleShot(500, this, &MainWindow::checkForAutoSaveRecovery);
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUi()
{
    setWindowTitle("PatternCAD");
    resize(1200, 800);

    // Create central canvas
    m_canvas = new Canvas(this);
    setCentralWidget(m_canvas);

    // Connect canvas signals
    connect(m_canvas, &Canvas::zoomChanged, this, [this](double zoom) {
        m_zoomLabel->setText(tr("%1%").arg(qRound(zoom * 100)));
    });
    connect(m_canvas, &Canvas::cursorPositionChanged, this, [this](const QPointF& pos) {
        m_cursorLabel->setText(tr("X: %1  Y: %2")
            .arg(pos.x(), 0, 'f', 1)
            .arg(pos.y(), 0, 'f', 1));
    });
    connect(m_canvas, &Canvas::escapePressed, this, &MainWindow::returnToSelectTool);
    connect(m_canvas, &Canvas::toolRequested, this, &MainWindow::onToolSelected);

    // Setup UI components
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
    setupWindowMenu();

    // Give focus to canvas so it receives keyboard events
    m_canvas->setFocus();
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, &MainWindow::onFileNew, QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::onFileOpen, QKeySequence::Open);

    // Recent Files submenu
    m_recentFilesMenu = fileMenu->addMenu(tr("Open &Recent"));
    for (int i = 0; i < MaxRecentFiles; ++i) {
        QAction* action = new QAction(this);
        action->setVisible(false);
        connect(action, &QAction::triggered, this, &MainWindow::onFileOpenRecent);
        m_recentFileActions.append(action);
        m_recentFilesMenu->addAction(action);
    }
    m_recentFilesMenu->addSeparator();
    QAction* clearRecentAction = m_recentFilesMenu->addAction(tr("Clear Recent Files"));
    connect(clearRecentAction, &QAction::triggered, [this]() {
        QSettings settings;
        settings.setValue("recentFiles", QStringList());
        updateRecentFilesMenu();
    });
    updateRecentFilesMenu();

    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Save"), this, &MainWindow::onFileSave, QKeySequence::Save);
    fileMenu->addAction(tr("Save &As..."), this, &MainWindow::onFileSaveAs, QKeySequence::SaveAs);

    // Import submenu
    QMenu* importMenu = fileMenu->addMenu(tr("&Import"));
    importMenu->addAction(tr("Import &SVG..."), this, &MainWindow::onFileImportSVG);
    importMenu->addAction(tr("Import &DXF..."), this, &MainWindow::onFileImportDXF);

    // Export submenu
    QMenu* exportMenu = fileMenu->addMenu(tr("&Export"));
    exportMenu->addAction(tr("Export as &SVG..."), this, &MainWindow::onFileExportSVG);
    exportMenu->addAction(tr("Export as &DXF..."), this, &MainWindow::onFileExportDXF);
    exportMenu->addAction(tr("Export as &PDF..."), this, &MainWindow::onFileExportPDF);

    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &MainWindow::onFileExit, QKeySequence::Quit);

    // Edit menu
    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), this, &MainWindow::onEditUndo, QKeySequence::Undo);
    editMenu->addAction(tr("&Redo"), this, &MainWindow::onEditRedo, QKeySequence(tr("Ctrl+Shift+Z")));
    editMenu->addSeparator();
    editMenu->addAction(tr("Select &All"), this, &MainWindow::onEditSelectAll, QKeySequence::SelectAll);
    editMenu->addAction(tr("&Deselect"), this, &MainWindow::onEditDeselect);
    editMenu->addSeparator();
    editMenu->addAction(tr("&Delete"), this, &MainWindow::onEditDelete, QKeySequence::Delete);
    editMenu->addSeparator();
    editMenu->addAction(tr("&Preferences..."), this, &MainWindow::onEditPreferences, QKeySequence::Preferences);

    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("Zoom &In"), this, &MainWindow::onViewZoomIn, QKeySequence::ZoomIn);
    viewMenu->addAction(tr("Zoom &Out"), this, &MainWindow::onViewZoomOut, QKeySequence::ZoomOut);
    viewMenu->addAction(tr("Zoom to &Fit"), this, &MainWindow::onViewZoomFit, QKeySequence(tr("F")));
    viewMenu->addAction(tr("Zoom to &Selection"), this, &MainWindow::onViewZoomSelection, QKeySequence(tr("Shift+F")));
    viewMenu->addAction(tr("Zoom &Actual (100%)"), this, &MainWindow::onViewZoomActual, QKeySequence(tr("Ctrl+0")));
    viewMenu->addSeparator();
    viewMenu->addAction(tr("Toggle &Grid"), this, &MainWindow::onViewToggleGrid);
    viewMenu->addAction(tr("Toggle &Snap to Grid"), this, &MainWindow::onViewToggleSnap);

    // Draw menu (placeholder)
    menuBar()->addMenu(tr("&Draw"));

    // Modify menu (shortcuts defined as global actions below, not here)
    QMenu* modifyMenu = menuBar()->addMenu(tr("&Modify"));
    modifyMenu->addAction(tr("&Rotate... (R)"), this, &MainWindow::onModifyRotate);
    modifyMenu->addAction(tr("&Mirror... (M)"), this, &MainWindow::onModifyMirror);
    modifyMenu->addAction(tr("&Scale... (S)"), this, &MainWindow::onModifyScale);
    modifyMenu->addAction(tr("Scale &Pattern... (Ctrl+Shift+S)"), this, &MainWindow::onModifyScalePattern, QKeySequence(tr("Ctrl+Shift+S")));
    modifyMenu->addSeparator();

    // Align submenu
    QMenu* alignMenu = modifyMenu->addMenu(tr("&Align"));
    alignMenu->addAction(tr("Align &Left"), this, &MainWindow::onModifyAlignLeft, QKeySequence(tr("Ctrl+Shift+L")));
    alignMenu->addAction(tr("Align &Right"), this, &MainWindow::onModifyAlignRight, QKeySequence(tr("Ctrl+Shift+R")));
    alignMenu->addAction(tr("Align &Top"), this, &MainWindow::onModifyAlignTop, QKeySequence(tr("Ctrl+Shift+T")));
    alignMenu->addAction(tr("Align &Bottom"), this, &MainWindow::onModifyAlignBottom, QKeySequence(tr("Ctrl+Shift+B")));
    alignMenu->addSeparator();
    alignMenu->addAction(tr("Align Center &Horizontal"), this, &MainWindow::onModifyAlignCenterHorizontal, QKeySequence(tr("Ctrl+Shift+H")));
    alignMenu->addAction(tr("Align Center &Vertical"), this, &MainWindow::onModifyAlignCenterVertical, QKeySequence(tr("Ctrl+Shift+V")));

    // Distribute submenu
    QMenu* distributeMenu = modifyMenu->addMenu(tr("&Distribute"));
    distributeMenu->addAction(tr("Distribute &Horizontal"), this, &MainWindow::onModifyDistributeHorizontal, QKeySequence(tr("Ctrl+Shift+D")));
    distributeMenu->addAction(tr("Distribute &Vertical"), this, &MainWindow::onModifyDistributeVertical, QKeySequence(tr("Ctrl+Shift+E")));
    
    // Grading
    modifyMenu->addSeparator();
    modifyMenu->addAction(tr("&Grading Rules... (Ctrl+G)"), this, &MainWindow::onModifyGradingRules, QKeySequence(tr("Ctrl+G")));
    modifyMenu->addAction(tr("Generate Graded &Sizes (Ctrl+Shift+G)"), this, &MainWindow::onModifyGenerateGradedSizes, QKeySequence(tr("Ctrl+Shift+G")));

    // Tools menu (shortcuts defined as global actions below, not here)
    QMenu* toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("&Select (Z)"), [this]() { onToolSelected("Select"); });
    toolsMenu->addAction(tr("&Polyline (P)"), [this]() { onToolSelected("Polyline"); });
    toolsMenu->addAction(tr("&Add Point on Contour (A)"), [this]() { onToolSelected("AddPointOnContour"); });
    toolsMenu->addSeparator();
    toolsMenu->addAction(tr("Seam &Allowance (S)"), [this]() { onToolSelected("SeamAllowance"); });
    toolsMenu->addAction(tr("&Notch (N)"), [this]() { onToolSelected("Notch"); });
    toolsMenu->addAction(tr("&Match Point (T)"), [this]() { onToolSelected("MatchPoint"); });
    toolsMenu->addSeparator();
    toolsMenu->addAction(tr("&Rotate (R)"), this, &MainWindow::onModifyRotate);
    toolsMenu->addAction(tr("&Mirror (M)"), this, &MainWindow::onModifyMirror);
    toolsMenu->addAction(tr("&Scale (S)"), this, &MainWindow::onModifyScale);

    // Help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&Keyboard Shortcuts..."), this, &MainWindow::onHelpKeyboardShortcuts, QKeySequence::HelpContents);
    helpMenu->addSeparator();
    helpMenu->addAction(tr("&About PatternCAD"), this, &MainWindow::onHelpAbout);

    // Tool keyboard shortcuts
    QAction* selectToolAction = new QAction(tr("&Select Tool"), this);
    selectToolAction->setShortcut(QKeySequence(Qt::Key_Z));
    connect(selectToolAction, &QAction::triggered, [this]() { onToolSelected("Select"); });
    addAction(selectToolAction);

    QAction* polylineToolAction = new QAction(tr("&Polyline Tool"), this);
    polylineToolAction->setShortcut(QKeySequence(Qt::Key_D));
    connect(polylineToolAction, &QAction::triggered, [this]() { onToolSelected("Polyline"); });
    addAction(polylineToolAction);

    QAction* addPointToolAction = new QAction(tr("&Edit Contour Tool"), this);
    addPointToolAction->setShortcut(QKeySequence(Qt::Key_A));
    connect(addPointToolAction, &QAction::triggered, [this]() { onToolSelected("AddPointOnContour"); });
    addAction(addPointToolAction);

    // Transformation tool shortcuts
    QAction* rotateToolAction = new QAction(tr("&Rotate Tool"), this);
    rotateToolAction->setShortcut(QKeySequence(Qt::Key_R));
    connect(rotateToolAction, &QAction::triggered, this, &MainWindow::onModifyRotate);
    addAction(rotateToolAction);

    QAction* mirrorToolAction = new QAction(tr("&Mirror Tool"), this);
    mirrorToolAction->setShortcut(QKeySequence(Qt::Key_M));
    connect(mirrorToolAction, &QAction::triggered, this, &MainWindow::onModifyMirror);
    addAction(mirrorToolAction);

    QAction* scaleToolAction = new QAction(tr("&Scale Tool"), this);
    scaleToolAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_S));
    connect(scaleToolAction, &QAction::triggered, this, &MainWindow::onModifyScale);
    addAction(scaleToolAction);

    QAction* seamAllowanceToolAction = new QAction(tr("&Seam Allowance Tool"), this);
    seamAllowanceToolAction->setShortcut(QKeySequence(Qt::Key_S));
    connect(seamAllowanceToolAction, &QAction::triggered, [this]() { onToolSelected("SeamAllowance"); });
    addAction(seamAllowanceToolAction);

    QAction* notchToolAction = new QAction(tr("&Notch Tool"), this);
    notchToolAction->setShortcut(QKeySequence(Qt::Key_N));
    connect(notchToolAction, &QAction::triggered, [this]() { onToolSelected("Notch"); });
    addAction(notchToolAction);

    QAction* matchPointToolAction = new QAction(tr("&Match Point Tool"), this);
    // No shortcut - access via menu only (M is used by Mirror Tool)
    connect(matchPointToolAction, &QAction::triggered, [this]() { onToolSelected("MatchPoint"); });
    addAction(matchPointToolAction);

    // Additional global shortcuts (not in menus)

    // Backspace as alternative to Delete
    QAction* backspaceDeleteAction = new QAction(this);
    backspaceDeleteAction->setShortcut(QKeySequence(Qt::Key_Backspace));
    connect(backspaceDeleteAction, &QAction::triggered, this, &MainWindow::onEditDelete);
    addAction(backspaceDeleteAction);

    // Zoom shortcuts without Ctrl
    QAction* zoomInPlusAction = new QAction(this);
    zoomInPlusAction->setShortcut(QKeySequence(Qt::Key_Plus));
    connect(zoomInPlusAction, &QAction::triggered, this, &MainWindow::onViewZoomIn);
    addAction(zoomInPlusAction);

    QAction* zoomInEqualAction = new QAction(this);
    zoomInEqualAction->setShortcut(QKeySequence(Qt::Key_Equal));
    connect(zoomInEqualAction, &QAction::triggered, this, &MainWindow::onViewZoomIn);
    addAction(zoomInEqualAction);

    QAction* zoomOutMinusAction = new QAction(this);
    zoomOutMinusAction->setShortcut(QKeySequence(Qt::Key_Minus));
    connect(zoomOutMinusAction, &QAction::triggered, this, &MainWindow::onViewZoomOut);
    addAction(zoomOutMinusAction);
}

void MainWindow::setupToolBar()
{
    // Toolbar removed - File menu provides all necessary actions
    // QToolBar* toolbar = addToolBar(tr("Main Toolbar"));
    // toolbar->setObjectName("MainToolbar");
    // toolbar->addAction(tr("New"));
    // toolbar->addAction(tr("Open"));
    // toolbar->addAction(tr("Save"));
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("Ready"));
    m_cursorLabel = new QLabel(tr("X: 0.0  Y: 0.0"));
    m_zoomLabel = new QLabel(tr("100%"));

    // Create dimension input overlay (floating on canvas)
    m_dimensionInput = new DimensionInputOverlay(this);
    m_dimensionInput->hide();

    // Connect dimension input signals
    connect(m_dimensionInput, &DimensionInputOverlay::valueAccepted, [this](double value, double angle, UI::ResizeMode mode, bool isUniform) {
        // Apply the dimension value to the active tool
        if (m_currentTool) {
            // Check tool type and apply appropriate dimension
            if (m_currentDimensionMode == "angle") {
                // For angle mode, value is the angle
                if (auto* rotateTool = qobject_cast<Tools::RotateTool*>(m_currentTool)) {
                    rotateTool->onNumericAngleEntered(value);
                    statusBar()->showMessage(tr("Rotation applied: %1°").arg(value, 0, 'f', 1), 3000);
                }
            } else if (m_currentDimensionMode == "scale") {
                // For scale mode, value is X percentage, angle is Y percentage
                if (auto* scaleTool = qobject_cast<Tools::ScaleTool*>(m_currentTool)) {
                    // isUniform is now passed directly in the signal
                    double scaleY = isUniform ? value : angle;  // Use angle field for Y if non-uniform
                    qDebug() << "MainWindow - Scale input: X=" << value << "angle=" << angle << "isUniform=" << isUniform << "scaleY=" << scaleY;
                    scaleTool->onNumericScaleEntered(value, scaleY, isUniform);
                    QString scaleMsg = isUniform
                        ? tr("Scale applied: %1% (uniform)").arg(value, 0, 'f', 1)
                        : tr("Scale applied: X=%1%, Y=%2% (non-uniform)").arg(value, 0, 'f', 1).arg(scaleY, 0, 'f', 1);
                    statusBar()->showMessage(scaleMsg, 3000);
                }
            } else if (auto* polylineTool = qobject_cast<Tools::PolylineTool*>(m_currentTool)) {
                polylineTool->applyLength(value, angle);
                QString msg = tr("Segment created with length: %1").arg(Units::formatLength(value, 2));
                if (angle != 0.0) {
                    msg += tr(", angle: %1°").arg(angle, 0, 'f', 1);
                }
                statusBar()->showMessage(msg, 3000);
            } else if (auto* selectTool = qobject_cast<Tools::SelectTool*>(m_currentTool)) {
                if (m_currentDimensionMode == "segment") {
                    selectTool->applySegmentLength(value, angle, mode);
                    statusBar()->showMessage(tr("Segment dimension applied"), 2000);
                } else {
                    selectTool->applyFreeSegmentLength(value, angle);
                    statusBar()->showMessage(tr("Free segment adjusted"), 2000);
                }
            }
        }
        m_canvas->setFocus();  // Return focus to canvas
    });

    connect(m_dimensionInput, &DimensionInputOverlay::valueCancelled, [this]() {
        statusBar()->showMessage(tr("Dimension input cancelled"), 2000);
        m_canvas->setFocus();  // Return focus to canvas
    });

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_cursorLabel);
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::setupDockWidgets()
{
    // Configure dock widgets to show tabs on top
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);

    // Tool Palette (left side)
    m_toolPalette = new ToolPalette(this);
    m_toolsDock = new QDockWidget(tr("Tools"), this);
    m_toolsDock->setObjectName("ToolsDock");
    m_toolsDock->setWidget(m_toolPalette);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolsDock);

    // Connect tool palette signals
    connect(m_toolPalette, &ToolPalette::toolSelected,
            this, &MainWindow::onToolSelected);

    // Properties Panel (right side)
    m_propertiesPanel = new PropertiesPanel(this);
    m_propertiesDock = new QDockWidget(tr("Properties"), this);
    m_propertiesDock->setObjectName("PropertiesDock");
    m_propertiesDock->setWidget(m_propertiesPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_propertiesDock);

    // Layers Panel (right side)
    m_layersPanel = new LayersPanel(this);
    m_layersDock = new QDockWidget(tr("Layers"), this);
    m_layersDock->setObjectName("LayersDock");
    m_layersDock->setWidget(m_layersPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_layersDock);

    // Parameters Panel (right side)
    m_parametersPanel = new ParametersPanel(this);
    m_parametersDock = new QDockWidget(tr("Parameters"), this);
    m_parametersDock->setObjectName("ParametersDock");
    m_parametersDock->setWidget(m_parametersPanel);
    addDockWidget(Qt::RightDockWidgetArea, m_parametersDock);

    // Objects Panel (right side)
    m_objectsPanel = new ObjectsPanel(this);
    m_objectsPanel->setObjectName("ObjectsPanel");
    addDockWidget(Qt::RightDockWidgetArea, m_objectsPanel);

    // Tabify the right side panels
    tabifyDockWidget(m_propertiesDock, m_layersDock);
    tabifyDockWidget(m_layersDock, m_parametersDock);
    tabifyDockWidget(m_parametersDock, m_objectsPanel);

    // Make Properties visible by default
    m_propertiesDock->raise();
}

void MainWindow::setupWindowMenu()
{
    // Find or create Window menu (insert before Help menu)
    QMenu* windowMenu = nullptr;
    QList<QAction*> menuActions = menuBar()->actions();

    // Find Help menu
    QMenu* helpMenu = nullptr;
    for (QAction* action : menuActions) {
        if (action->menu() && action->menu()->title() == tr("&Help")) {
            helpMenu = action->menu();
            break;
        }
    }

    // Insert Window menu before Help menu
    if (helpMenu) {
        windowMenu = new QMenu(tr("&Window"), this);
        menuBar()->insertMenu(helpMenu->menuAction(), windowMenu);
    } else {
        windowMenu = menuBar()->addMenu(tr("&Window"));
    }

    // Add toggle actions for each dock widget
    windowMenu->addAction(m_toolsDock->toggleViewAction());
    windowMenu->addAction(m_propertiesDock->toggleViewAction());
    windowMenu->addAction(m_layersDock->toggleViewAction());
    windowMenu->addAction(m_parametersDock->toggleViewAction());
}

void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindow/state").toByteArray());

    // Load auto-save settings
    if (m_autoSaveManager) {
        SettingsManager& settingsManager = SettingsManager::instance();
        FileIOSettings fileIO = settingsManager.fileIO();

        m_autoSaveManager->setEnabled(fileIO.autoSaveEnabled);
        m_autoSaveManager->setInterval(fileIO.autoSaveInterval);
        m_autoSaveManager->setMaxAutoSaves(fileIO.autoSaveVersions);

        // Set auto-save directory based on location preference
        if (fileIO.autoSaveLocation == 1 && !fileIO.autoSaveCustomDirectory.isEmpty()) {
            // Custom directory
            m_autoSaveManager->setAutoSaveDirectory(fileIO.autoSaveCustomDirectory);
        } else {
            // Next to file (empty string means next to file)
            m_autoSaveManager->setAutoSaveDirectory("");
        }
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/state", saveState());

    // Note: Auto-save settings are now managed by SettingsManager via PreferencesDialog
}

void MainWindow::updateWindowTitle()
{
    Project* project = Application::instance()->currentProject();
    Document* document = m_canvas->document();
    QString title = "PatternCAD";

    if (project) {
        QString filename;
        if (!project->filepath().isEmpty()) {
            // Extract filename from filepath
            QFileInfo fileInfo(project->filepath());
            filename = fileInfo.fileName();
        } else {
            filename = "Untitled";
        }

        title += " - " + filename;

        // Show modified indicator if document is modified
        if (document && document->isModified()) {
            title += " *";
        }
    }

    setWindowTitle(title);
}

// File menu slots
void MainWindow::onFileNew()
{
    if (promptSaveChanges()) {
        Project* project = new Project();
        Application::instance()->setCurrentProject(project);
        updateWindowTitle();
    }
}

void MainWindow::onFileOpen()
{
    if (!promptSaveChanges()) {
        return;
    }

    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("Open Pattern File"),
        getDefaultDirectory(),
        tr("PatternCAD Files (*.patterncad);;All Files (*)")
    );

    if (!filepath.isEmpty()) {
        openFile(filepath);
    }
}

void MainWindow::onFileSave()
{
    saveFile();
}

void MainWindow::onFileSaveAs()
{
    saveFileAs();
}

void MainWindow::onFileImportSVG()
{
    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("Import SVG"),
        getDefaultDirectory(),
        tr("SVG Files (*.svg)")
    );

    if (filepath.isEmpty()) {
        return;
    }

    Document* document = m_canvas->document();
    if (!document) {
        statusBar()->showMessage(tr("Error: No document available"), 3000);
        return;
    }

    statusBar()->showMessage(tr("Importing SVG..."));

    IO::SVGFormat svgFormat;
    if (svgFormat.importFile(filepath, document)) {
        statusBar()->showMessage(tr("Imported SVG: %1").arg(filepath), 3000);
        m_canvas->zoomFit(); // Auto-zoom to fit imported objects
        m_canvas->update();
    } else {
        QMessageBox::warning(this, tr("Import Failed"),
                           tr("Failed to import SVG file: %1\n%2")
                           .arg(filepath)
                           .arg(svgFormat.lastError()));
        statusBar()->showMessage(tr("Import failed"), 3000);
    }
}

void MainWindow::onFileImportDXF()
{
    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("Import DXF"),
        getDefaultDirectory(),
        tr("DXF Files (*.dxf)")
    );

    if (filepath.isEmpty()) {
        return;
    }

    Document* document = m_canvas->document();
    if (!document) {
        statusBar()->showMessage(tr("Error: No document available"), 3000);
        return;
    }

    statusBar()->showMessage(tr("Importing DXF..."));

    IO::DXFFormat dxfFormat;
    if (dxfFormat.importFile(filepath, document)) {
        statusBar()->showMessage(tr("Imported DXF: %1").arg(filepath), 3000);
        m_canvas->zoomFit(); // Auto-zoom to fit imported objects
        m_canvas->update();
    } else {
        QMessageBox::warning(this, tr("Import Failed"),
                           tr("Failed to import DXF file: %1\n%2")
                           .arg(filepath)
                           .arg(dxfFormat.lastError()));
        statusBar()->showMessage(tr("Import failed"), 3000);
    }
}

void MainWindow::onFileExportSVG()
{
    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Export as SVG"),
        getDefaultDirectory(),
        tr("SVG Files (*.svg)")
    );

    if (filepath.isEmpty()) {
        return;
    }

    // Add extension if not present
    if (!filepath.endsWith(".svg", Qt::CaseInsensitive)) {
        filepath += ".svg";
    }

    Document* document = m_canvas->document();
    if (!document) {
        statusBar()->showMessage(tr("Error: No document available"), 3000);
        return;
    }

    statusBar()->showMessage(tr("Exporting to SVG..."));

    IO::SVGFormat svgFormat;
    if (svgFormat.exportFile(filepath, document)) {
        statusBar()->showMessage(tr("Exported to SVG: %1").arg(filepath), 3000);
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
                           tr("Failed to export SVG file: %1\n%2")
                           .arg(filepath)
                           .arg(svgFormat.lastError()));
        statusBar()->showMessage(tr("Export failed"), 3000);
    }
}

void MainWindow::onFileExportDXF()
{
    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Export as DXF"),
        getDefaultDirectory(),
        tr("DXF Files (*.dxf)")
    );

    if (filepath.isEmpty()) {
        return;
    }

    // Add extension if not present
    if (!filepath.endsWith(".dxf", Qt::CaseInsensitive)) {
        filepath += ".dxf";
    }

    Document* document = m_canvas->document();
    if (!document) {
        statusBar()->showMessage(tr("Error: No document available"), 3000);
        return;
    }

    statusBar()->showMessage(tr("Exporting to DXF..."));

    IO::DXFFormat dxfFormat;
    if (dxfFormat.exportFile(filepath, document)) {
        statusBar()->showMessage(tr("Exported to DXF: %1").arg(filepath), 3000);
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
                           tr("Failed to export DXF file: %1\n%2")
                           .arg(filepath)
                           .arg(dxfFormat.lastError()));
        statusBar()->showMessage(tr("Export failed"), 3000);
    }
}

void MainWindow::onFileExportPDF()
{
    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Export as PDF"),
        getDefaultDirectory(),
        tr("PDF Files (*.pdf)")
    );

    if (filepath.isEmpty()) {
        return;
    }

    // Add extension if not present
    if (!filepath.endsWith(".pdf", Qt::CaseInsensitive)) {
        filepath += ".pdf";
    }

    Document* document = m_canvas->document();
    if (!document) {
        statusBar()->showMessage(tr("Error: No document available"), 3000);
        return;
    }

    statusBar()->showMessage(tr("Exporting to PDF..."));

    IO::PDFFormat pdfFormat;
    if (pdfFormat.exportFile(filepath, document)) {
        statusBar()->showMessage(tr("Exported to PDF: %1").arg(filepath), 3000);
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
                           tr("Failed to export PDF file: %1\n%2")
                           .arg(filepath)
                           .arg(pdfFormat.lastError()));
        statusBar()->showMessage(tr("Export failed"), 3000);
    }
}

void MainWindow::onFileExit()
{
    close();
}

// Edit menu slots
void MainWindow::onEditUndo()
{
    Document* document = m_canvas->document();
    if (document && document->canUndo()) {
        document->undo();
        QString undoText = document->undoStack()->undoText();
        statusBar()->showMessage(tr("Undo: %1").arg(undoText), 2000);
    }
}

void MainWindow::onEditRedo()
{
    Document* document = m_canvas->document();
    if (document && document->canRedo()) {
        document->redo();
        QString redoText = document->undoStack()->redoText();
        statusBar()->showMessage(tr("Redo: %1").arg(redoText), 2000);
    }
}

// View menu slots
void MainWindow::onViewZoomIn()
{
    if (m_canvas) {
        m_canvas->zoomIn();
    }
}

void MainWindow::onViewZoomOut()
{
    if (m_canvas) {
        m_canvas->zoomOut();
    }
}

void MainWindow::onViewZoomFit()
{
    if (m_canvas) {
        m_canvas->zoomFit();
    }
}

void MainWindow::onViewZoomSelection()
{
    if (m_canvas) {
        m_canvas->zoomToSelection();
        statusBar()->showMessage(tr("Zoom to selection"), 2000);
    }
}

void MainWindow::onViewZoomActual()
{
    if (m_canvas) {
        m_canvas->zoomToActual();  // 100% zoom
        statusBar()->showMessage(tr("Zoom 100%"), 2000);
    }
}

void MainWindow::onViewToggleGrid()
{
    if (m_canvas) {
        bool visible = m_canvas->gridVisible();
        m_canvas->setGridVisible(!visible);
        statusBar()->showMessage(tr("Grid %1").arg(!visible ? tr("shown") : tr("hidden")), 2000);
    }
}

void MainWindow::onViewToggleSnap()
{
    if (m_canvas) {
        bool snap = m_canvas->snapToGrid();
        m_canvas->setSnapToGrid(!snap);
        statusBar()->showMessage(tr("Snap to grid %1").arg(!snap ? tr("enabled") : tr("disabled")), 2000);
    }
}

// Edit menu slots
void MainWindow::onEditDelete()
{
    // First, check if the active tool is SelectTool and has a vertex targeted
    // If so, let the tool handle the deletion
    if (m_canvas && m_canvas->activeTool()) {
        Tools::Tool* activeTool = m_canvas->activeTool();
        if (activeTool->name() == "Select") {
            // Cast to SelectTool to check if vertex is targeted
            Tools::SelectTool* selectTool = dynamic_cast<Tools::SelectTool*>(activeTool);
            if (selectTool && selectTool->hasVertexTargeted()) {
                // Let SelectTool handle the vertex deletion
                // Create a fake key event and pass it to the tool
                QKeyEvent deleteEvent(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
                selectTool->keyPressEvent(&deleteEvent);
                return;
            }
        }
    }

    // No vertex targeted, proceed with normal object deletion
    Document* document = m_canvas->document();
    if (!document) return;

    const QList<Geometry::GeometryObject*> selected = document->selectedObjects();
    if (selected.isEmpty()) {
        statusBar()->showMessage(tr("No objects selected"), 2000);
        return;
    }

    // Confirmation for bulk delete
    if (selected.size() > 10) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            tr("Delete Objects"),
            tr("Delete %1 selected objects?").arg(selected.size()),
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    document->removeObjects(selected);
    statusBar()->showMessage(tr("Deleted %1 object(s)").arg(selected.size()), 2000);
}

void MainWindow::onEditSelectAll()
{
    Document* document = m_canvas->document();
    if (!document) return;

    document->selectAll();
    int count = document->selectedObjects().size();
    statusBar()->showMessage(tr("Selected %1 object(s)").arg(count), 2000);
}

void MainWindow::onEditDeselect()
{
    Document* document = m_canvas->document();
    if (!document) return;

    document->clearSelection();
    statusBar()->showMessage(tr("Selection cleared"), 2000);
}

void MainWindow::onEditPreferences()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

// Modify menu slots
void MainWindow::onModifyRotate()
{
    // Activate the Rotate tool
    onToolSelected("Rotate");
}

void MainWindow::onModifyMirror()
{
    // Activate the Mirror tool
    onToolSelected("Mirror");
}

void MainWindow::onModifyScale()
{
    // Activate the Scale tool
    onToolSelected("Scale");
}

void MainWindow::onModifyScalePattern()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;
    
    // Get selected polyline
    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    Geometry::Polyline* polyline = nullptr;
    
    for (Geometry::GeometryObject* obj : selectedObjects) {
        polyline = dynamic_cast<Geometry::Polyline*>(obj);
        if (polyline) break;
    }
    
    if (!polyline) {
        statusBar()->showMessage(tr("Select a pattern piece to scale"), 3000);
        return;
    }
    
    // Open scale dialog
    ScalePatternDialog dialog(polyline, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Apply scaling via command
        auto* command = new ScalePatternCommand(
            polyline,
            dialog.scaleX(),
            dialog.scaleY(),
            dialog.scaleSeamAllowance(),
            dialog.scaleNotchDepths()
        );
        if (document->undoStack()) {
            document->undoStack()->push(command);
        }
        statusBar()->showMessage(tr("Pattern scaled to %.0f%% × %.0f%%")
                                .arg(dialog.scaleX() * 100)
                                .arg(dialog.scaleY() * 100), 3000);
    }
}

void MainWindow::onModifyGradingRules()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;
    
    // Get selected polyline
    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    Geometry::Polyline* polyline = nullptr;
    
    for (Geometry::GeometryObject* obj : selectedObjects) {
        polyline = dynamic_cast<Geometry::Polyline*>(obj);
        if (polyline) break;
    }
    
    if (!polyline) {
        statusBar()->showMessage(tr("Select a polyline to configure grading rules"), 3000);
        return;
    }
    
    // Open grading dialog
    GradingDialog dialog(polyline, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Apply grading rules via command
        GradingSystem* newGrading = dialog.gradingSystem()->clone();
        auto* command = new SetGradingRulesCommand(polyline, newGrading);
        if (document->undoStack()) {
            document->undoStack()->push(command);
        }
        statusBar()->showMessage(tr("Grading rules applied"), 3000);
    }
}

void MainWindow::onModifyGenerateGradedSizes()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;
    
    // Get selected polyline
    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    Geometry::Polyline* polyline = nullptr;
    
    for (Geometry::GeometryObject* obj : selectedObjects) {
        polyline = dynamic_cast<Geometry::Polyline*>(obj);
        if (polyline) break;
    }
    
    if (!polyline) {
        statusBar()->showMessage(tr("Select a polyline with grading rules to generate sizes"), 3000);
        return;
    }
    
    GradingSystem* grading = polyline->gradingSystem();
    if (!grading || grading->sizeCount() == 0) {
        statusBar()->showMessage(tr("No grading rules defined. Use Grading Rules (G) first."), 3000);
        return;
    }
    
    if (grading->ruleCount() == 0) {
        statusBar()->showMessage(tr("No grading rules defined. Add rules in Grading Rules dialog."), 3000);
        return;
    }
    
    // Generate all sizes
    auto* command = new GenerateGradedSizesCommand(document, polyline);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
    
    int generatedCount = command->generatedPolylines().size();
    statusBar()->showMessage(tr("Generated %1 graded sizes").arg(generatedCount), 3000);
}

void MainWindow::onModifyAlignLeft()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 2) {
        statusBar()->showMessage(tr("Select at least 2 objects to align"), 3000);
        return;
    }

    auto* command = new AlignObjectsCommand(selectedObjects, AlignMode::Left);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyAlignRight()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 2) {
        statusBar()->showMessage(tr("Select at least 2 objects to align"), 3000);
        return;
    }

    auto* command = new AlignObjectsCommand(selectedObjects, AlignMode::Right);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyAlignTop()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 2) {
        statusBar()->showMessage(tr("Select at least 2 objects to align"), 3000);
        return;
    }

    auto* command = new AlignObjectsCommand(selectedObjects, AlignMode::Top);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyAlignBottom()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 2) {
        statusBar()->showMessage(tr("Select at least 2 objects to align"), 3000);
        return;
    }

    auto* command = new AlignObjectsCommand(selectedObjects, AlignMode::Bottom);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyAlignCenterHorizontal()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 2) {
        statusBar()->showMessage(tr("Select at least 2 objects to align"), 3000);
        return;
    }

    auto* command = new AlignObjectsCommand(selectedObjects, AlignMode::CenterHorizontal);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyAlignCenterVertical()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 2) {
        statusBar()->showMessage(tr("Select at least 2 objects to align"), 3000);
        return;
    }

    auto* command = new AlignObjectsCommand(selectedObjects, AlignMode::CenterVertical);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyDistributeHorizontal()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 3) {
        statusBar()->showMessage(tr("Select at least 3 objects to distribute"), 3000);
        return;
    }

    auto* command = new DistributeObjectsCommand(selectedObjects, DistributeMode::Horizontal);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

void MainWindow::onModifyDistributeVertical()
{
    Document* document = m_canvas ? m_canvas->document() : nullptr;
    if (!document) return;

    QList<Geometry::GeometryObject*> selectedObjects = document->selectedObjects();
    if (selectedObjects.size() < 3) {
        statusBar()->showMessage(tr("Select at least 3 objects to distribute"), 3000);
        return;
    }

    auto* command = new DistributeObjectsCommand(selectedObjects, DistributeMode::Vertical);
    if (document->undoStack()) {
        document->undoStack()->push(command);
    }
}

// Help menu slots
void MainWindow::onHelpKeyboardShortcuts()
{
    KeyboardShortcutsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("About PatternCAD"),
        tr("<h3>PatternCAD 0.1.0</h3>"
           "<p>Professional Parametric Pattern Design Software</p>"
           "<p>A desktop application for creating parametric sewing patterns "
           "with constraint-based design.</p>"
           "<p>Copyright © 2026</p>"));
}

// Project change handlers
void MainWindow::onProjectChanged(Project* project)
{
    if (project) {
        connect(project, &Project::modifiedChanged, this, &MainWindow::onProjectModified);
    }
    updateWindowTitle();
}

void MainWindow::onProjectModified(bool /*modified*/)
{
    updateWindowTitle();
}

void MainWindow::onDimensionInputRequested(const QString& mode, double initialLength, double initialAngle)
{
    if (!m_dimensionInput) {
        return;
    }

    // Store mode for valueAccepted
    m_currentDimensionMode = mode;

    qDebug() << "Dimension input requested, mode:" << mode << "initial length:" << initialLength << "angle:" << initialAngle;

    // Determine prompt based on mode
    QString prompt = "Length (cm):";
    bool withAngle = false;
    bool withResizeMode = false;

    if (mode == "length" || mode == "freeSegment" || mode == "segment") {
        prompt = "Length (cm):";
        withAngle = true;  // Show angle field for length mode
        if (mode == "segment") {
            withResizeMode = true;  // Show resize mode choice for segment dimensioning
        }
    } else if (mode == "coordinate") {
        prompt = "Coordinate:";
    } else if (mode == "circle") {
        prompt = "Radius:";
    } else if (mode == "rectangle") {
        prompt = "Width x Height:";
    } else if (mode == "angle") {
        prompt = "Angle (degrees):";
        withAngle = false;  // Don't show second angle field, we only need one input
    } else if (mode == "scale") {
        prompt = "Scale (%):";
        withAngle = false;  // Don't show second angle field
    }

    // Get current cursor position in global coordinates
    QPoint globalPos = QCursor::pos();

    // Show the overlay at cursor position with initial values
    bool isAngleMode = (mode == "angle");
    bool isScaleMode = (mode == "scale");
    m_dimensionInput->showAtPosition(globalPos, prompt, withAngle, initialLength, initialAngle, withResizeMode, isAngleMode, isScaleMode);

    if (isAngleMode) {
        statusBar()->showMessage(tr("🔄 Enter angle in degrees (Enter=apply, Esc=cancel)"));
    } else if (isScaleMode) {
        statusBar()->showMessage(tr("📐 Enter scale percentage (Enter=apply, Esc=cancel)"));
    } else {
        statusBar()->showMessage(tr("📏 Enter length and angle (Enter=apply, Esc=cancel)"));
    }
}

// File operations
void MainWindow::openFile(const QString& filepath)
{
    Document* document = m_canvas->document();
    if (!document) {
        statusBar()->showMessage(tr("Error: No document available"), 3000);
        return;
    }

    statusBar()->showMessage(tr("Opening %1...").arg(filepath));

    if (document->load(filepath)) {
        Project* project = Application::instance()->currentProject();
        if (project) {
            project->setFilepath(filepath);
        }
        if (m_autoSaveManager) {
            m_autoSaveManager->setFilePath(filepath);
        }
        updateRecentFiles(filepath);
        m_canvas->viewport()->update();
        updateWindowTitle();
        statusBar()->showMessage(tr("Opened %1").arg(filepath), 3000);
    } else {
        QMessageBox::warning(this, tr("Open Failed"),
                           tr("Failed to open file: %1").arg(filepath));
        statusBar()->showMessage(tr("Failed to open file"), 3000);
    }
}

void MainWindow::saveFile()
{
    Project* project = Application::instance()->currentProject();
    if (!project) return;

    if (project->filepath().isEmpty()) {
        saveFileAs();
    } else {
        Document* document = m_canvas->document();
        if (!document) {
            statusBar()->showMessage(tr("Error: No document available"), 3000);
            return;
        }

        statusBar()->showMessage(tr("Saving %1...").arg(project->filepath()));

        if (document->save(project->filepath())) {
            updateRecentFiles(project->filepath());
            updateWindowTitle();
            statusBar()->showMessage(tr("Saved %1").arg(project->filepath()), 3000);
        } else {
            QMessageBox::warning(this, tr("Save Failed"),
                               tr("Failed to save file: %1").arg(project->filepath()));
            statusBar()->showMessage(tr("Failed to save file"), 3000);
        }
    }
}

void MainWindow::saveFileAs()
{
    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Save Pattern File"),
        getDefaultDirectory(),
        tr("PatternCAD Files (*.patterncad)")
    );

    if (!filepath.isEmpty()) {
        // Add extension if not present
        if (!filepath.endsWith(".patterncad", Qt::CaseInsensitive)) {
            filepath += ".patterncad";
        }

        Document* document = m_canvas->document();
        if (!document) {
            statusBar()->showMessage(tr("Error: No document available"), 3000);
            return;
        }

        statusBar()->showMessage(tr("Saving as %1...").arg(filepath));

        if (document->save(filepath)) {
            Project* project = Application::instance()->currentProject();
            if (project) {
                project->setFilepath(filepath);
            }
            if (m_autoSaveManager) {
                m_autoSaveManager->setFilePath(filepath);
            }
            updateRecentFiles(filepath);
            updateWindowTitle();
            statusBar()->showMessage(tr("Saved as %1").arg(filepath), 3000);
        } else {
            QMessageBox::warning(this, tr("Save Failed"),
                               tr("Failed to save file: %1").arg(filepath));
            statusBar()->showMessage(tr("Failed to save file"), 3000);
        }
    }
}

void MainWindow::newProject()
{
    if (promptSaveChanges()) {
        Project* project = new Project();
        Application::instance()->setCurrentProject(project);
    }
}

void MainWindow::onToolSelected(const QString& toolName)
{
    if (m_tools.contains(toolName)) {
        m_currentTool = m_tools[toolName];
        m_canvas->setActiveTool(m_currentTool);
        // Don't show generic tool description here - each tool shows its own
        // detailed status message in activate() with keyboard shortcuts
    }
}

void MainWindow::returnToSelectTool()
{
    // Automatically return to Select tool after creating an object
    onToolSelected("Select");
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (promptSaveChanges()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::promptSaveChanges()
{
    Project* project = Application::instance()->currentProject();
    if (!project || !project->isModified()) {
        return true;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Save Changes?"),
        tr("The project has unsaved changes. Do you want to save them?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (reply == QMessageBox::Save) {
        saveFile();
        return true;
    } else if (reply == QMessageBox::Discard) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::onFileOpenRecent()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString filepath = action->data().toString();
        if (!filepath.isEmpty()) {
            if (promptSaveChanges()) {
                openFile(filepath);
            }
        }
    }
}

void MainWindow::updateRecentFiles(const QString& filepath)
{
    QSettings settings;
    QStringList recentFiles = settings.value("recentFiles").toStringList();

    // Remove if already exists (to move to top)
    recentFiles.removeAll(filepath);

    // Add to beginning
    recentFiles.prepend(filepath);

    // Keep only MaxRecentFiles
    while (recentFiles.size() > MaxRecentFiles) {
        recentFiles.removeLast();
    }

    // Save to settings
    settings.setValue("recentFiles", recentFiles);

    // Update menu
    updateRecentFilesMenu();
}

void MainWindow::updateRecentFilesMenu()
{
    QSettings settings;
    QStringList recentFiles = settings.value("recentFiles").toStringList();

    // Remove non-existent files
    QStringList existingFiles;
    for (const QString& filepath : recentFiles) {
        if (QFileInfo::exists(filepath)) {
            existingFiles.append(filepath);
        }
    }

    // Update settings if files were removed
    if (existingFiles.size() != recentFiles.size()) {
        settings.setValue("recentFiles", existingFiles);
        recentFiles = existingFiles;
    }

    // Update actions
    int numRecentFiles = qMin(recentFiles.size(), MaxRecentFiles);
    for (int i = 0; i < numRecentFiles; ++i) {
        QString filepath = recentFiles[i];
        QFileInfo fileInfo(filepath);
        QString text = QString("&%1 %2").arg(i + 1).arg(fileInfo.fileName());

        m_recentFileActions[i]->setText(text);
        m_recentFileActions[i]->setData(filepath);
        m_recentFileActions[i]->setVisible(true);
        m_recentFileActions[i]->setStatusTip(filepath);
        m_recentFileActions[i]->setToolTip(filepath);
    }

    // Hide unused actions
    for (int i = numRecentFiles; i < MaxRecentFiles; ++i) {
        m_recentFileActions[i]->setVisible(false);
    }

    // Enable/disable menu based on whether there are recent files
    m_recentFilesMenu->setEnabled(numRecentFiles > 0);
}

QString MainWindow::getDefaultDirectory() const
{
    // Try to use current project directory
    Project* project = Application::instance()->currentProject();
    if (project && !project->filepath().isEmpty()) {
        QFileInfo fileInfo(project->filepath());
        return fileInfo.absolutePath();
    }

    // Try to use last recent file directory
    QSettings settings;
    QStringList recentFiles = settings.value("recentFiles").toStringList();
    if (!recentFiles.isEmpty() && QFileInfo::exists(recentFiles.first())) {
        QFileInfo fileInfo(recentFiles.first());
        return fileInfo.absolutePath();
    }

    // Fall back to current working directory
    return QDir::currentPath();
}

void MainWindow::checkForAutoSaveRecovery()
{
    // Get all auto-save directories to check
    QStringList directoriesToCheck;

    // 1. Check custom auto-save directory if configured
    if (m_autoSaveManager) {
        QString autoSaveDir = m_autoSaveManager->autoSaveDirectory();
        if (!autoSaveDir.isEmpty()) {
            directoriesToCheck << autoSaveDir;
        }
    }

    // 2. Check recent files directories
    QSettings settings;
    QStringList recentFiles = settings.value("recentFiles").toStringList();
    for (const QString& filePath : recentFiles) {
        QFileInfo fileInfo(filePath);
        QString dir = fileInfo.absolutePath();
        if (!directoriesToCheck.contains(dir)) {
            directoriesToCheck << dir;
        }
    }

    // 3. Check last used directories
    SettingsManager& settingsManager = SettingsManager::instance();
    FileIOSettings fileIO = settingsManager.fileIO();
    if (!fileIO.lastOpenDirectory.isEmpty() && !directoriesToCheck.contains(fileIO.lastOpenDirectory)) {
        directoriesToCheck << fileIO.lastOpenDirectory;
    }
    if (!fileIO.lastSaveDirectory.isEmpty() && !directoriesToCheck.contains(fileIO.lastSaveDirectory)) {
        directoriesToCheck << fileIO.lastSaveDirectory;
    }

    // 4. Check home directory
    QString homeDir = QDir::homePath();
    if (!directoriesToCheck.contains(homeDir)) {
        directoriesToCheck << homeDir;
    }

    // Scan for auto-save files
    QStringList allAutoSaveFiles;
    for (const QString& dir : directoriesToCheck) {
        QStringList files = AutoSaveManager::findAllAutoSaveFiles(dir);
        allAutoSaveFiles.append(files);
    }

    // Remove duplicates
    allAutoSaveFiles.removeDuplicates();

    if (allAutoSaveFiles.isEmpty()) {
        return; // No recovery files found
    }

    // Show recovery dialog
    RecoveryDialog dialog(allAutoSaveFiles, this);
    int result = dialog.exec();

    if (result == QDialog::Accepted) {
        QString selectedFile = dialog.selectedFile();
        bool shouldDelete = dialog.shouldDeleteAutoSaves();

        if (shouldDelete) {
            // Delete all auto-save files
            for (const QString& filePath : allAutoSaveFiles) {
                QFile::remove(filePath);
            }
            statusBar()->showMessage(tr("Auto-save files discarded"), 3000);
        } else if (!selectedFile.isEmpty()) {
            // Recover selected file
            Document* document = m_canvas->document();
            if (document && document->load(selectedFile)) {
                document->setModified(true); // Mark as modified (needs save)
                m_canvas->viewport()->update();
                updateWindowTitle();
                statusBar()->showMessage(tr("Recovered from auto-save: %1").arg(selectedFile), 5000);

                // Don't delete the auto-save file yet - keep it until user saves
            } else {
                QMessageBox::warning(this, tr("Recovery Failed"),
                                   tr("Failed to load auto-save file: %1").arg(selectedFile));
            }
        }
    }
    // If cancelled, keep auto-save files for next startup
}

} // namespace UI
} // namespace PatternCAD
