/**
 * GradingDialog.cpp
 *
 * Implementation of GradingDialog
 */

#include "GradingDialog.h"
#include "geometry/Polyline.h"
#include "geometry/GradingSystem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QInputDialog>

namespace PatternCAD {

GradingDialog::GradingDialog(Geometry::Polyline* polyline, QWidget* parent)
    : QDialog(parent)
    , m_polyline(polyline)
    , m_gradingSystem(nullptr)
{
    setWindowTitle(tr("Grading Rules"));
    setMinimumSize(600, 500);
    
    // Create or clone grading system
    if (m_polyline->gradingSystem()) {
        m_gradingSystem = m_polyline->gradingSystem()->clone(this);
    } else {
        m_gradingSystem = new GradingSystem(this);
        m_gradingSystem->setStandardSizes();  // Default to XS-XL
    }
    
    setupUI();
    populateSizesTable();
    populateRulesTable();
    updateVertexCombo();
}

GradingDialog::~GradingDialog()
{
}

void GradingDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // === Sizes Group ===
    QGroupBox* sizesGroup = new QGroupBox(tr("Sizes"));
    QVBoxLayout* sizesLayout = new QVBoxLayout(sizesGroup);
    
    // Sizes table
    m_sizesTable = new QTableWidget(0, 2);
    m_sizesTable->setHorizontalHeaderLabels({tr("Name"), tr("Offset")});
    m_sizesTable->horizontalHeader()->setStretchLastSection(true);
    m_sizesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    sizesLayout->addWidget(m_sizesTable);
    
    // Base size selector
    QHBoxLayout* baseLayout = new QHBoxLayout();
    baseLayout->addWidget(new QLabel(tr("Base Size:")));
    m_baseSizeCombo = new QComboBox();
    connect(m_baseSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GradingDialog::onBaseSizeChanged);
    baseLayout->addWidget(m_baseSizeCombo);
    baseLayout->addStretch();
    sizesLayout->addLayout(baseLayout);
    
    // Size buttons
    QHBoxLayout* sizeBtnLayout = new QHBoxLayout();
    m_addSizeBtn = new QPushButton(tr("Add Size"));
    m_removeSizeBtn = new QPushButton(tr("Remove Size"));
    m_standardSizesBtn = new QPushButton(tr("Standard (XS-XL)"));
    m_numericSizesBtn = new QPushButton(tr("Numeric (34-46)"));
    
    connect(m_addSizeBtn, &QPushButton::clicked, this, &GradingDialog::onAddSize);
    connect(m_removeSizeBtn, &QPushButton::clicked, this, &GradingDialog::onRemoveSize);
    connect(m_standardSizesBtn, &QPushButton::clicked, this, &GradingDialog::onSetStandardSizes);
    connect(m_numericSizesBtn, &QPushButton::clicked, this, &GradingDialog::onSetNumericSizes);
    
    sizeBtnLayout->addWidget(m_addSizeBtn);
    sizeBtnLayout->addWidget(m_removeSizeBtn);
    sizeBtnLayout->addWidget(m_standardSizesBtn);
    sizeBtnLayout->addWidget(m_numericSizesBtn);
    sizesLayout->addLayout(sizeBtnLayout);
    
    mainLayout->addWidget(sizesGroup);
    
    // === Rules Group ===
    QGroupBox* rulesGroup = new QGroupBox(tr("Grading Rules"));
    QVBoxLayout* rulesLayout = new QVBoxLayout(rulesGroup);
    
    // Rules table
    m_rulesTable = new QTableWidget(0, 4);
    m_rulesTable->setHorizontalHeaderLabels({tr("Name"), tr("Vertex"), tr("X (mm/size)"), tr("Y (mm/size)")});
    m_rulesTable->horizontalHeader()->setStretchLastSection(true);
    m_rulesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(m_rulesTable, &QTableWidget::cellChanged, this, &GradingDialog::onRuleChanged);
    rulesLayout->addWidget(m_rulesTable);
    
    // Rule buttons
    QHBoxLayout* ruleBtnLayout = new QHBoxLayout();
    m_addRuleBtn = new QPushButton(tr("Add Rule"));
    m_removeRuleBtn = new QPushButton(tr("Remove Rule"));
    
    connect(m_addRuleBtn, &QPushButton::clicked, this, &GradingDialog::onAddRule);
    connect(m_removeRuleBtn, &QPushButton::clicked, this, &GradingDialog::onRemoveRule);
    
    ruleBtnLayout->addWidget(m_addRuleBtn);
    ruleBtnLayout->addWidget(m_removeRuleBtn);
    ruleBtnLayout->addStretch();
    rulesLayout->addLayout(ruleBtnLayout);
    
    mainLayout->addWidget(rulesGroup);
    
    // === Info ===
    m_infoLabel = new QLabel(tr("Rules specify how each vertex moves per size step. "
                                "Positive X moves right, positive Y moves down."));
    m_infoLabel->setWordWrap(true);
    mainLayout->addWidget(m_infoLabel);
    
    // === Dialog buttons ===
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &GradingDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void GradingDialog::populateSizesTable()
{
    m_sizesTable->blockSignals(true);
    m_sizesTable->setRowCount(0);
    m_baseSizeCombo->clear();
    
    for (int i = 0; i < m_gradingSystem->sizeCount(); ++i) {
        SizeInfo size = m_gradingSystem->sizeAt(i);
        
        int row = m_sizesTable->rowCount();
        m_sizesTable->insertRow(row);
        m_sizesTable->setItem(row, 0, new QTableWidgetItem(size.name));
        m_sizesTable->setItem(row, 1, new QTableWidgetItem(QString::number(size.offset)));
        
        m_baseSizeCombo->addItem(size.name);
    }
    
    m_baseSizeCombo->setCurrentIndex(m_gradingSystem->baseSizeIndex());
    m_sizesTable->blockSignals(false);
}

void GradingDialog::populateRulesTable()
{
    m_rulesTable->blockSignals(true);
    m_rulesTable->setRowCount(0);
    
    for (int i = 0; i < m_gradingSystem->ruleCount(); ++i) {
        GradeRule rule = m_gradingSystem->ruleAt(i);
        
        int row = m_rulesTable->rowCount();
        m_rulesTable->insertRow(row);
        m_rulesTable->setItem(row, 0, new QTableWidgetItem(rule.name));
        m_rulesTable->setItem(row, 1, new QTableWidgetItem(QString::number(rule.vertexIndex)));
        m_rulesTable->setItem(row, 2, new QTableWidgetItem(QString::number(rule.incrementPerSize.x())));
        m_rulesTable->setItem(row, 3, new QTableWidgetItem(QString::number(rule.incrementPerSize.y())));
    }
    
    m_rulesTable->blockSignals(false);
}

void GradingDialog::updateVertexCombo()
{
    // This could populate a combo for selecting vertices visually
    // For now, users enter vertex index directly
}

void GradingDialog::onAddSize()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Size"), 
                                          tr("Size name:"), QLineEdit::Normal, 
                                          "", &ok);
    if (ok && !name.isEmpty()) {
        double offset = QInputDialog::getDouble(this, tr("Add Size"),
                                                 tr("Offset from base:"),
                                                 0.0, -10.0, 10.0, 1, &ok);
        if (ok) {
            m_gradingSystem->addSize(name, offset);
            populateSizesTable();
        }
    }
}

void GradingDialog::onRemoveSize()
{
    int row = m_sizesTable->currentRow();
    if (row >= 0) {
        m_gradingSystem->removeSize(row);
        populateSizesTable();
    }
}

void GradingDialog::onSetStandardSizes()
{
    m_gradingSystem->setStandardSizes();
    populateSizesTable();
}

void GradingDialog::onSetNumericSizes()
{
    // Default: European sizes 34-46
    m_gradingSystem->setNumericSizes(34, 46, 2);
    populateSizesTable();
}

void GradingDialog::onBaseSizeChanged(int index)
{
    if (index >= 0) {
        m_gradingSystem->setBaseSizeIndex(index);
    }
}

void GradingDialog::onAddRule()
{
    if (!m_polyline) return;
    
    bool ok;
    int vertex = QInputDialog::getInt(this, tr("Add Grading Rule"),
                                       tr("Vertex index (0-%1):").arg(m_polyline->vertexCount() - 1),
                                       0, 0, m_polyline->vertexCount() - 1, 1, &ok);
    if (ok) {
        GradeRule rule;
        rule.name = tr("Vertex %1").arg(vertex);
        rule.vertexIndex = vertex;
        rule.incrementPerSize = QPointF(0, 0);
        
        m_gradingSystem->addRule(rule);
        populateRulesTable();
    }
}

void GradingDialog::onRemoveRule()
{
    int row = m_rulesTable->currentRow();
    if (row >= 0) {
        m_gradingSystem->removeRule(row);
        populateRulesTable();
    }
}

void GradingDialog::onRuleChanged(int row, int column)
{
    if (row < 0 || row >= m_gradingSystem->ruleCount()) return;
    
    GradeRule rule = m_gradingSystem->ruleAt(row);
    
    QTableWidgetItem* item = m_rulesTable->item(row, column);
    if (!item) return;
    
    switch (column) {
        case 0: // Name
            rule.name = item->text();
            break;
        case 1: // Vertex
            rule.vertexIndex = item->text().toInt();
            break;
        case 2: // X increment
            rule.incrementPerSize.setX(item->text().toDouble());
            break;
        case 3: // Y increment
            rule.incrementPerSize.setY(item->text().toDouble());
            break;
    }
    
    m_gradingSystem->updateRule(row, rule);
}

void GradingDialog::onAccept()
{
    if (!m_gradingSystem->isValid()) {
        QMessageBox::warning(this, tr("Invalid Configuration"),
                            m_gradingSystem->validationError());
        return;
    }
    
    accept();
}

} // namespace PatternCAD
