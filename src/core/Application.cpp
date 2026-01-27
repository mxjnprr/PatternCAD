/**
 * Application.cpp
 *
 * Implementation of Application singleton
 */

#include "Application.h"
#include "Project.h"
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

namespace PatternCAD {

// Static instance
Application* Application::s_instance = nullptr;

Application::Application(QObject* parent)
    : QObject(parent)
    , m_currentProject(nullptr)
{
    // Load recent files from settings
    loadRecentFiles();
}

Application::~Application()
{
    // Save recent files
    saveRecentFiles();

    // Cleanup
    if (m_currentProject) {
        delete m_currentProject;
        m_currentProject = nullptr;
    }
}

Application* Application::instance()
{
    if (!s_instance) {
        s_instance = new Application();
    }
    return s_instance;
}

Project* Application::currentProject() const
{
    return m_currentProject;
}

void Application::setCurrentProject(Project* project)
{
    if (m_currentProject != project) {
        // Cleanup old project
        if (m_currentProject) {
            delete m_currentProject;
        }

        m_currentProject = project;
        emit projectChanged(project);
    }
}

QString Application::applicationDataPath() const
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return path;
}

QString Application::recentFilesPath() const
{
    return applicationDataPath() + "/recent-files.txt";
}

QStringList Application::recentFiles() const
{
    return m_recentFiles;
}

void Application::addRecentFile(const QString& filepath)
{
    // Remove if already in list
    m_recentFiles.removeAll(filepath);

    // Add to front
    m_recentFiles.prepend(filepath);

    // Keep only last 20 files
    while (m_recentFiles.size() > 20) {
        m_recentFiles.removeLast();
    }

    // Save and notify
    saveRecentFiles();
    emit recentFilesChanged();
}

void Application::clearRecentFiles()
{
    m_recentFiles.clear();
    saveRecentFiles();
    emit recentFilesChanged();
}

void Application::loadRecentFiles()
{
    QSettings settings;
    m_recentFiles = settings.value("recentFiles").toStringList();
}

void Application::saveRecentFiles()
{
    QSettings settings;
    settings.setValue("recentFiles", m_recentFiles);
}

} // namespace PatternCAD
