/**
 * Application.h
 *
 * Application singleton class managing global state and resources
 */

#ifndef PATTERNCAD_APPLICATION_H
#define PATTERNCAD_APPLICATION_H

#include <QObject>
#include <QString>
#include <memory>

namespace PatternCAD {

class Project;

class Application : public QObject
{
    Q_OBJECT

public:
    // Singleton instance
    static Application* instance();

    // Destructor
    ~Application();

    // Current project
    Project* currentProject() const;
    void setCurrentProject(Project* project);

    // Application settings
    QString applicationDataPath() const;
    QString recentFilesPath() const;

    // Recent files management
    QStringList recentFiles() const;
    void addRecentFile(const QString& filepath);
    void clearRecentFiles();

signals:
    void projectChanged(Project* project);
    void recentFilesChanged();

private:
    // Private constructor (singleton)
    explicit Application(QObject* parent = nullptr);

    // Non-copyable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Private members
    static Application* s_instance;
    Project* m_currentProject;
    QStringList m_recentFiles;

    // Helper methods
    void loadRecentFiles();
    void saveRecentFiles();
};

} // namespace PatternCAD

#endif // PATTERNCAD_APPLICATION_H
