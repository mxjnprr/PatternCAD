/**
 * ParametersPanel.cpp
 *
 * Implementation of parameters panel for parametric design
 */

#include "ParametersPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>

namespace PatternCAD {
namespace UI {

ParametersPanel::ParametersPanel(QWidget* parent)
    : QDockWidget(tr("Parameters"), parent)
    , m_table(nullptr)
    , m_addButton(nullptr)
    , m_deleteButton(nullptr)
    , m_duplicateButton(nullptr)
    , m_clearButton(nullptr)
    , m_searchBox(nullptr)
    , m_updating(false)
{
    setupUI();
    setObjectName("ParametersPanel");

    // Add some default parameters as examples
    addParameter(Parameter("width", 100.0, "mm"));
    addParameter(Parameter("height", 50.0, "mm"));
    addParameter(Parameter("margin", 5.0, "mm"));
}

ParametersPanel::~ParametersPanel()
{
}

void ParametersPanel::setupUI()
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    // Search box
    QHBoxLayout* searchLayout = new QHBoxLayout();
    QLabel* searchLabel = new QLabel(tr("Search:"), container);
    m_searchBox = new QLineEdit(container);
    m_searchBox->setPlaceholderText(tr("Filter parameters..."));
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchBox);
    mainLayout->addLayout(searchLayout);

    connect(m_searchBox, &QLineEdit::textChanged,
            this, &ParametersPanel::onSearchTextChanged);

    // Parameters table
    m_table = new QTableWidget(0, ColumnCount, container);
    m_table->setHorizontalHeaderLabels({
        tr("Name"),
        tr("Value"),
        tr("Unit"),
        tr("Expression"),
        tr("Group")
    });

    // Configure table
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);

    m_table->setColumnWidth(0, 120);  // Name
    m_table->setColumnWidth(1, 80);   // Value
    m_table->setColumnWidth(2, 60);   // Unit
    m_table->setColumnWidth(4, 100);  // Group

    m_table->setAlternatingRowColors(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    m_table->setSortingEnabled(true);

    mainLayout->addWidget(m_table);

    connect(m_table, &QTableWidget::itemChanged,
            this, &ParametersPanel::onItemChanged);
    connect(m_table, &QTableWidget::customContextMenuRequested,
            this, &ParametersPanel::onContextMenu);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_addButton = new QPushButton(tr("Add"), container);
    m_addButton->setToolTip(tr("Add new parameter"));
    buttonLayout->addWidget(m_addButton);

    m_deleteButton = new QPushButton(tr("Delete"), container);
    m_deleteButton->setToolTip(tr("Delete selected parameters"));
    buttonLayout->addWidget(m_deleteButton);

    m_duplicateButton = new QPushButton(tr("Duplicate"), container);
    m_duplicateButton->setToolTip(tr("Duplicate selected parameter"));
    buttonLayout->addWidget(m_duplicateButton);

    m_clearButton = new QPushButton(tr("Clear All"), container);
    m_clearButton->setToolTip(tr("Remove all parameters"));
    buttonLayout->addWidget(m_clearButton);

    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(m_addButton, &QPushButton::clicked,
            this, &ParametersPanel::onAddParameter);
    connect(m_deleteButton, &QPushButton::clicked,
            this, &ParametersPanel::onDeleteParameter);
    connect(m_duplicateButton, &QPushButton::clicked,
            this, &ParametersPanel::onDuplicateParameter);
    connect(m_clearButton, &QPushButton::clicked,
            this, &ParametersPanel::onClearAll);

    setWidget(container);
}

void ParametersPanel::addParameter(const Parameter& param)
{
    // Validate name
    if (!validateParameterName(param.name)) {
        showValidationError(tr("Invalid parameter name: %1").arg(param.name));
        return;
    }

    if (m_parameters.contains(param.name)) {
        showValidationError(tr("Parameter '%1' already exists").arg(param.name));
        return;
    }

    // Store parameter
    m_parameters.insert(param.name, param);

    // Add to table
    populateTable();

    emit parameterCreated(param.name, param.value, param.unit);
}

void ParametersPanel::removeParameter(const QString& name)
{
    if (!m_parameters.contains(name)) {
        return;
    }

    m_parameters.remove(name);
    populateTable();

    emit parameterDeleted(name);
}

void ParametersPanel::updateParameter(const QString& name, const Parameter& param)
{
    if (!m_parameters.contains(name)) {
        return;
    }

    // If name changed, handle rename
    if (name != param.name) {
        if (m_parameters.contains(param.name)) {
            showValidationError(tr("Parameter '%1' already exists").arg(param.name));
            return;
        }
        m_parameters.remove(name);
    }

    m_parameters.insert(param.name, param);
    populateTable();

    emit parameterValueChanged(param.name, param.value);
    if (!param.expression.isEmpty()) {
        emit parameterExpressionChanged(param.name, param.expression);
    }
}

Parameter ParametersPanel::getParameter(const QString& name) const
{
    return m_parameters.value(name, Parameter());
}

QList<Parameter> ParametersPanel::getAllParameters() const
{
    return m_parameters.values();
}

void ParametersPanel::clearParameters()
{
    m_parameters.clear();
    populateTable();
}

void ParametersPanel::populateTable()
{
    m_updating = true;

    // Store sort state
    bool sortingEnabled = m_table->isSortingEnabled();
    m_table->setSortingEnabled(false);

    // Clear table
    m_table->setRowCount(0);

    // Get search filter
    QString searchText = m_searchBox->text().toLower();

    // Add filtered parameters
    int row = 0;
    for (const Parameter& param : m_parameters.values()) {
        // Apply search filter
        if (!searchText.isEmpty()) {
            if (!param.name.toLower().contains(searchText) &&
                !param.group.toLower().contains(searchText)) {
                continue;
            }
        }

        m_table->insertRow(row);
        updateTableRow(row, param);
        row++;
    }

    // Restore sort state
    m_table->setSortingEnabled(sortingEnabled);

    m_updating = false;
}

void ParametersPanel::updateTableRow(int row, const Parameter& param)
{
    // Name
    QTableWidgetItem* nameItem = new QTableWidgetItem(param.name);
    m_table->setItem(row, ColumnName, nameItem);

    // Value
    QTableWidgetItem* valueItem = new QTableWidgetItem(QString::number(param.value, 'f', 2));
    m_table->setItem(row, ColumnValue, valueItem);

    // Unit
    QTableWidgetItem* unitItem = new QTableWidgetItem(param.unit);
    m_table->setItem(row, ColumnUnit, unitItem);

    // Expression
    QTableWidgetItem* exprItem = new QTableWidgetItem(param.expression);
    exprItem->setToolTip(tr("Formula: %1").arg(param.expression));
    if (!param.expression.isEmpty()) {
        exprItem->setForeground(QBrush(QColor(50, 100, 200)));  // Blue for expressions
    }
    m_table->setItem(row, ColumnExpression, exprItem);

    // Group
    QTableWidgetItem* groupItem = new QTableWidgetItem(param.group);
    m_table->setItem(row, ColumnGroup, groupItem);
}

void ParametersPanel::onAddParameter()
{
    // Generate unique name
    QString name = generateUniqueName("parameter");

    Parameter param(name, 0.0, "mm");
    param.group = "Custom";

    addParameter(param);

    // Select and edit the new parameter
    int row = findParameterRow(name);
    if (row >= 0) {
        m_table->selectRow(row);
        m_table->editItem(m_table->item(row, ColumnName));
    }
}

void ParametersPanel::onDeleteParameter()
{
    QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    // Get selected parameter names
    QSet<QString> paramNames;
    for (QTableWidgetItem* item : selectedItems) {
        int row = item->row();
        QTableWidgetItem* nameItem = m_table->item(row, ColumnName);
        if (nameItem) {
            paramNames.insert(nameItem->text());
        }
    }

    if (paramNames.isEmpty()) {
        return;
    }

    // Confirm deletion
    QString message;
    if (paramNames.size() == 1) {
        message = tr("Delete parameter '%1'?").arg(*paramNames.begin());
    } else {
        message = tr("Delete %1 parameters?").arg(paramNames.size());
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Deletion"),
        message,
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        for (const QString& name : paramNames) {
            removeParameter(name);
        }
    }
}

void ParametersPanel::onDuplicateParameter()
{
    QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    // Get first selected parameter
    int row = selectedItems.first()->row();
    QTableWidgetItem* nameItem = m_table->item(row, ColumnName);
    if (!nameItem) {
        return;
    }

    QString originalName = nameItem->text();
    Parameter original = getParameter(originalName);
    if (original.name.isEmpty()) {
        return;
    }

    // Create duplicate with unique name
    Parameter duplicate = original;
    duplicate.name = generateUniqueName(originalName + "_copy");

    addParameter(duplicate);

    // Select the new parameter
    int newRow = findParameterRow(duplicate.name);
    if (newRow >= 0) {
        m_table->selectRow(newRow);
    }
}

void ParametersPanel::onClearAll()
{
    if (m_parameters.isEmpty()) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Clear All Parameters"),
        tr("Delete all %1 parameters?").arg(m_parameters.size()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        clearParameters();
    }
}

void ParametersPanel::onItemChanged(QTableWidgetItem* item)
{
    if (m_updating || !item) {
        return;
    }

    int row = item->row();
    int col = item->column();

    // Get original parameter name from current row
    QTableWidgetItem* nameItem = m_table->item(row, ColumnName);
    if (!nameItem) {
        return;
    }

    QString currentName = nameItem->text();

    // For name column, currentName might be the new name, so we need to find the parameter
    // by checking all parameters in that row position
    QString originalName;
    if (col == ColumnName) {
        // Find parameter at this row position
        for (const Parameter& p : m_parameters.values()) {
            if (findParameterRow(p.name) == row) {
                originalName = p.name;
                break;
            }
        }
        if (originalName.isEmpty()) {
            return;
        }
    } else {
        originalName = currentName;
    }

    Parameter param = getParameter(originalName);
    if (param.name.isEmpty()) {
        return;
    }

    m_updating = true;

    try {
        switch (col) {
            case ColumnName: {
                QString newName = item->text().trimmed();
                if (!validateParameterName(newName, originalName)) {
                    item->setText(param.name);  // Revert
                    showValidationError(tr("Invalid parameter name"));
                } else {
                    param.name = newName;
                    updateParameter(originalName, param);
                }
                break;
            }
            case ColumnValue: {
                bool ok;
                double value = item->text().toDouble(&ok);
                if (ok) {
                    param.value = value;
                    updateParameter(originalName, param);
                } else {
                    item->setText(QString::number(param.value, 'f', 2));  // Revert
                    showValidationError(tr("Invalid numeric value"));
                }
                break;
            }
            case ColumnUnit:
                param.unit = item->text().trimmed();
                updateParameter(originalName, param);
                break;
            case ColumnExpression:
                param.expression = item->text().trimmed();
                updateParameter(originalName, param);
                break;
            case ColumnGroup:
                param.group = item->text().trimmed();
                updateParameter(originalName, param);
                break;
        }
    } catch (...) {
        // Revert on error
        populateTable();
    }

    m_updating = false;
}

void ParametersPanel::onContextMenu(const QPoint& pos)
{
    QTableWidgetItem* item = m_table->itemAt(pos);

    QMenu menu(this);

    QAction* addAction = menu.addAction(tr("Add Parameter"));
    connect(addAction, &QAction::triggered, this, &ParametersPanel::onAddParameter);

    if (item) {
        QAction* deleteAction = menu.addAction(tr("Delete Parameter"));
        connect(deleteAction, &QAction::triggered, this, &ParametersPanel::onDeleteParameter);

        QAction* duplicateAction = menu.addAction(tr("Duplicate Parameter"));
        connect(duplicateAction, &QAction::triggered, this, &ParametersPanel::onDuplicateParameter);

        menu.addSeparator();

        QAction* copyNameAction = menu.addAction(tr("Copy Name"));
        connect(copyNameAction, &QAction::triggered, this, &ParametersPanel::onCopyName);

        QAction* copyValueAction = menu.addAction(tr("Copy Value"));
        connect(copyValueAction, &QAction::triggered, this, &ParametersPanel::onCopyValue);
    }

    menu.addSeparator();
    QAction* clearAction = menu.addAction(tr("Clear All"));
    connect(clearAction, &QAction::triggered, this, &ParametersPanel::onClearAll);

    menu.exec(m_table->mapToGlobal(pos));
}

void ParametersPanel::onSearchTextChanged(const QString& /*text*/)
{
    populateTable();
}

void ParametersPanel::onCopyName()
{
    QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    int row = selectedItems.first()->row();
    QTableWidgetItem* nameItem = m_table->item(row, ColumnName);
    if (nameItem) {
        QApplication::clipboard()->setText(nameItem->text());
    }
}

void ParametersPanel::onCopyValue()
{
    QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    int row = selectedItems.first()->row();
    QTableWidgetItem* valueItem = m_table->item(row, ColumnValue);
    if (valueItem) {
        QApplication::clipboard()->setText(valueItem->text());
    }
}

bool ParametersPanel::validateParameterName(const QString& name, const QString& excludeName) const
{
    if (name.isEmpty()) {
        return false;
    }

    // Must start with letter or underscore
    // Can contain letters, digits, and underscores
    QRegularExpression re("^[a-zA-Z_][a-zA-Z0-9_]*$");
    if (!re.match(name).hasMatch()) {
        return false;
    }

    // Check uniqueness (excluding the parameter being renamed)
    if (name != excludeName && m_parameters.contains(name)) {
        return false;
    }

    return true;
}

bool ParametersPanel::validateExpression(const QString& expression) const
{
    // Basic validation - just check if not empty
    // Full expression parsing will be implemented with ParametricEngine (Epic-002)
    if (expression.isEmpty()) {
        return true;
    }

    // Check for basic syntax - must not start/end with operators
    QString trimmed = expression.trimmed();
    if (trimmed.startsWith('+') || trimmed.startsWith('*') || trimmed.startsWith('/') ||
        trimmed.endsWith('+') || trimmed.endsWith('-') || trimmed.endsWith('*') || trimmed.endsWith('/')) {
        return false;
    }

    return true;
}

void ParametersPanel::showValidationError(const QString& message)
{
    QMessageBox::warning(this, tr("Validation Error"), message);
}

int ParametersPanel::findParameterRow(const QString& name) const
{
    for (int row = 0; row < m_table->rowCount(); ++row) {
        QTableWidgetItem* item = m_table->item(row, ColumnName);
        if (item && item->text() == name) {
            return row;
        }
    }
    return -1;
}

QString ParametersPanel::generateUniqueName(const QString& baseName) const
{
    QString name = baseName;
    int counter = 1;

    while (m_parameters.contains(name)) {
        name = QString("%1_%2").arg(baseName).arg(counter);
        counter++;
    }

    return name;
}

} // namespace UI
} // namespace PatternCAD
