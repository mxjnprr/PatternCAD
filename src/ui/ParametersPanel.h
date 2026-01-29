/**
 * ParametersPanel.h
 *
 * Dockable panel for managing parametric design parameters
 */

#ifndef PATTERNCAD_PARAMETERSPANEL_H
#define PATTERNCAD_PARAMETERSPANEL_H

#include <QDockWidget>
#include <QString>
#include <QMap>

class QTableWidget;
class QPushButton;
class QLineEdit;
class QTableWidgetItem;

namespace PatternCAD {

// Forward declaration for future parametric engine integration
class ParametricEngine;

namespace UI {

/**
 * Parameter data structure
 */
struct Parameter {
    QString name;
    double value;
    QString unit;           // "mm", "cm", "inches", "deg", etc.
    QString expression;     // Formula like "waist_width / 2"
    QString group;          // Category like "Measurements", "Calculated", etc.

    Parameter() : value(0.0), unit("mm") {}
    Parameter(const QString& n, double v, const QString& u = "mm")
        : name(n), value(v), unit(u) {}
};

/**
 * ParametersPanel provides UI for viewing and editing parametric design parameters.
 *
 * Features:
 * - Table view with columns: Name, Value, Unit, Expression, Group
 * - Add/Delete parameters
 * - Inline editing
 * - Search/filter
 * - Future: Integration with ParametricEngine from Epic-002
 */
class ParametersPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit ParametersPanel(QWidget* parent = nullptr);
    ~ParametersPanel() override;

    // Parameter management
    void addParameter(const Parameter& param);
    void removeParameter(const QString& name);
    void updateParameter(const QString& name, const Parameter& param);
    Parameter getParameter(const QString& name) const;
    QList<Parameter> getAllParameters() const;

    // Clear all parameters
    void clearParameters();

    // Future: Connect to parametric engine (Epic-002)
    // void setParametricEngine(ParametricEngine* engine);

signals:
    void parameterCreated(const QString& name, double value, const QString& unit);
    void parameterDeleted(const QString& name);
    void parameterValueChanged(const QString& name, double value);
    void parameterExpressionChanged(const QString& name, const QString& expression);

private slots:
    void onAddParameter();
    void onDeleteParameter();
    void onDuplicateParameter();
    void onClearAll();
    void onItemChanged(QTableWidgetItem* item);
    void onContextMenu(const QPoint& pos);
    void onSearchTextChanged(const QString& text);
    void onCopyName();
    void onCopyValue();

private:
    void setupUI();
    void populateTable();
    void updateTableRow(int row, const Parameter& param);
    bool validateParameterName(const QString& name, const QString& excludeName = "") const;
    bool validateExpression(const QString& expression) const;
    void showValidationError(const QString& message);
    int findParameterRow(const QString& name) const;
    QString generateUniqueName(const QString& baseName) const;

    // UI Components
    QTableWidget* m_table;
    QPushButton* m_addButton;
    QPushButton* m_deleteButton;
    QPushButton* m_duplicateButton;
    QPushButton* m_clearButton;
    QLineEdit* m_searchBox;

    // Data storage (until ParametricEngine is implemented)
    QMap<QString, Parameter> m_parameters;

    // Future: Reference to parametric engine
    // ParametricEngine* m_engine;

    // Table column indices
    enum ColumnIndex {
        ColumnName = 0,
        ColumnValue = 1,
        ColumnUnit = 2,
        ColumnExpression = 3,
        ColumnGroup = 4,
        ColumnCount = 5
    };

    // Validation state
    bool m_updating;  // Flag to prevent recursive updates during editing
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_PARAMETERSPANEL_H
