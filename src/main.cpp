/**
 * PatternCAD - Professional Parametric Pattern Design Software
 *
 * Main application entry point
 */

#include <QApplication>
#include <QCommandLineParser>
#include "core/Application.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    // Create Qt application
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("PatternCAD");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("PatternCAD");
    QApplication::setOrganizationDomain("patterncad.org");

    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("Professional Parametric Pattern Design Software");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add file argument
    parser.addPositionalArgument("file", "Pattern file to open (.patterncad)");

    parser.process(app);

    // Create application instance (singleton)
    PatternCAD::Application* application = PatternCAD::Application::instance();

    // Create and show main window
    PatternCAD::UI::MainWindow mainWindow;
    mainWindow.show();

    // Open file if specified
    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        mainWindow.openFile(args.first());
    }

    // Run application event loop
    return app.exec();
}
