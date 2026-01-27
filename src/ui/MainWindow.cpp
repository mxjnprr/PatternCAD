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
#include "../core/Application.h"
#include "../core/Project.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>

namespace PatternCAD {
namespace UI {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_canvas(nullptr)
    , m_toolPalette(nullptr)
    , m_propertiesPanel(nullptr)
    , m_layersPanel(nullptr)
    , m_statusLabel(nullptr)
    , m_cursorLabel(nullptr)
    , m_zoomLabel(nullptr)
{
    setupUi();
    loadSettings();

    // Connect to application
    Application* app = Application::instance();
    connect(app, &Application::projectChanged, this, &MainWindow::onProjectChanged);

    // Create default project
    Project* project = new Project();
    app->setCurrentProject(project);

    updateWindowTitle();
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

    // Setup UI components
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&New"), this, &MainWindow::onFileNew, QKeySequence::New);
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::onFileOpen, QKeySequence::Open);
    fileMenu->addAction(tr("&Save"), this, &MainWindow::onFileSave, QKeySequence::Save);
    fileMenu->addAction(tr("Save &As..."), this, &MainWindow::onFileSaveAs, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("E&xit"), this, &MainWindow::onFileExit, QKeySequence::Quit);

    // Edit menu
    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), this, &MainWindow::onEditUndo, QKeySequence::Undo);
    editMenu->addAction(tr("&Redo"), this, &MainWindow::onEditRedo, QKeySequence::Redo);

    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("Zoom &In"), this, &MainWindow::onViewZoomIn, QKeySequence::ZoomIn);
    viewMenu->addAction(tr("Zoom &Out"), this, &MainWindow::onViewZoomOut, QKeySequence::ZoomOut);
    viewMenu->addAction(tr("Zoom to &Fit"), this, &MainWindow::onViewZoomFit, QKeySequence(tr("F")));

    // Draw menu (placeholder)
    menuBar()->addMenu(tr("&Draw"));

    // Modify menu (placeholder)
    menuBar()->addMenu(tr("&Modify"));

    // Tools menu (placeholder)
    menuBar()->addMenu(tr("&Tools"));

    // Window menu (placeholder)
    menuBar()->addMenu(tr("&Window"));

    // Help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About PatternCAD"), this, &MainWindow::onHelpAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar* toolbar = addToolBar(tr("Main Toolbar"));
    toolbar->setObjectName("MainToolbar");

    // Add toolbar actions (placeholders for now)
    toolbar->addAction(tr("New"));
    toolbar->addAction(tr("Open"));
    toolbar->addAction(tr("Save"));
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("Ready"));
    m_cursorLabel = new QLabel(tr("X: 0.0  Y: 0.0"));
    m_zoomLabel = new QLabel(tr("100%"));

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_cursorLabel);
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::setupDockWidgets()
{
    // Tool Palette (left side)
    m_toolPalette = new ToolPalette(this);
    QDockWidget* toolDock = new QDockWidget(tr("Tools"), this);
    toolDock->setObjectName("ToolsDock");
    toolDock->setWidget(m_toolPalette);
    addDockWidget(Qt::LeftDockWidgetArea, toolDock);

    // Properties Panel (right side)
    m_propertiesPanel = new PropertiesPanel(this);
    QDockWidget* propertiesDock = new QDockWidget(tr("Properties"), this);
    propertiesDock->setObjectName("PropertiesDock");
    propertiesDock->setWidget(m_propertiesPanel);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);

    // Layers Panel (right side)
    m_layersPanel = new LayersPanel(this);
    QDockWidget* layersDock = new QDockWidget(tr("Layers"), this);
    layersDock->setObjectName("LayersDock");
    layersDock->setWidget(m_layersPanel);
    addDockWidget(Qt::RightDockWidgetArea, layersDock);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindow/state").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/state", saveState());
}

void MainWindow::updateWindowTitle()
{
    Project* project = Application::instance()->currentProject();
    QString title = "PatternCAD";

    if (project) {
        title += " - " + project->name();
        if (project->isModified()) {
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
        QString(),
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

void MainWindow::onFileExit()
{
    close();
}

// Edit menu slots
void MainWindow::onEditUndo()
{
    // TODO: Implement undo
    statusBar()->showMessage(tr("Undo - Not yet implemented"), 2000);
}

void MainWindow::onEditRedo()
{
    // TODO: Implement redo
    statusBar()->showMessage(tr("Redo - Not yet implemented"), 2000);
}

// View menu slots
void MainWindow::onViewZoomIn()
{
    // TODO: Implement zoom
    statusBar()->showMessage(tr("Zoom In - Not yet implemented"), 2000);
}

void MainWindow::onViewZoomOut()
{
    // TODO: Implement zoom
    statusBar()->showMessage(tr("Zoom Out - Not yet implemented"), 2000);
}

void MainWindow::onViewZoomFit()
{
    // TODO: Implement zoom to fit
    statusBar()->showMessage(tr("Zoom to Fit - Not yet implemented"), 2000);
}

// Help menu slots
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

// File operations
void MainWindow::openFile(const QString& filepath)
{
    // TODO: Implement file loading
    statusBar()->showMessage(tr("Opening %1...").arg(filepath), 2000);
}

void MainWindow::saveFile()
{
    Project* project = Application::instance()->currentProject();
    if (!project) return;

    if (project->filepath().isEmpty()) {
        saveFileAs();
    } else {
        // TODO: Implement file saving
        statusBar()->showMessage(tr("Saving %1...").arg(project->filepath()), 2000);
    }
}

void MainWindow::saveFileAs()
{
    QString filepath = QFileDialog::getSaveFileName(
        this,
        tr("Save Pattern File"),
        QString(),
        tr("PatternCAD Files (*.patterncad)")
    );

    if (!filepath.isEmpty()) {
        // TODO: Implement file saving
        statusBar()->showMessage(tr("Saving as %1...").arg(filepath), 2000);
    }
}

void MainWindow::newProject()
{
    if (promptSaveChanges()) {
        Project* project = new Project();
        Application::instance()->setCurrentProject(project);
    }
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

} // namespace UI
} // namespace PatternCAD
