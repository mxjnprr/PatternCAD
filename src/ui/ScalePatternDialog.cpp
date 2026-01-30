/**
 * ScalePatternDialog.cpp
 *
 * Implementation of ScalePatternDialog
 */

#include "ScalePatternDialog.h"
#include "geometry/Polyline.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>

namespace PatternCAD {

ScalePatternDialog::ScalePatternDialog(Geometry::Polyline* polyline, QWidget* parent)
    : QDialog(parent)
    , m_polyline(polyline)
    , m_scaleX(1.0)
    , m_scaleY(1.0)
{
    setWindowTitle(tr("Scale Pattern"));
    setMinimumWidth(400);
    
    // Calculate original dimensions
    QRectF bounds = m_polyline->boundingRect();
    m_originalWidth = bounds.width();
    m_originalHeight = bounds.height();
    
    setupUI();
    updatePreview();
}

ScalePatternDialog::~ScalePatternDialog()
{
}

void ScalePatternDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // === Mode Selection ===
    QGroupBox* modeGroup = new QGroupBox(tr("Scale Mode"));
    QVBoxLayout* modeLayout = new QVBoxLayout(modeGroup);
    
    m_modeGroup = new QButtonGroup(this);
    m_percentageMode = new QRadioButton(tr("Scale by percentage"));
    m_dimensionMode = new QRadioButton(tr("Scale to fit dimension"));
    m_modeGroup->addButton(m_percentageMode, 0);
    m_modeGroup->addButton(m_dimensionMode, 1);
    m_percentageMode->setChecked(true);
    
    connect(m_percentageMode, &QRadioButton::toggled, this, &ScalePatternDialog::onModeChanged);
    connect(m_dimensionMode, &QRadioButton::toggled, this, &ScalePatternDialog::onModeChanged);
    
    modeLayout->addWidget(m_percentageMode);
    modeLayout->addWidget(m_dimensionMode);
    mainLayout->addWidget(modeGroup);
    
    // === Percentage Scaling ===
    QGroupBox* percentGroup = new QGroupBox(tr("Scale Percentage"));
    QVBoxLayout* percentLayout = new QVBoxLayout(percentGroup);
    
    QHBoxLayout* percentXLayout = new QHBoxLayout();
    percentXLayout->addWidget(new QLabel(tr("Width:")));
    m_percentageXSpin = new QDoubleSpinBox();
    m_percentageXSpin->setRange(1.0, 1000.0);
    m_percentageXSpin->setValue(100.0);
    m_percentageXSpin->setSuffix(" %");
    m_percentageXSpin->setDecimals(1);
    connect(m_percentageXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ScalePatternDialog::onPercentageChanged);
    percentXLayout->addWidget(m_percentageXSpin);
    percentLayout->addLayout(percentXLayout);
    
    QHBoxLayout* percentYLayout = new QHBoxLayout();
    percentYLayout->addWidget(new QLabel(tr("Height:")));
    m_percentageYSpin = new QDoubleSpinBox();
    m_percentageYSpin->setRange(1.0, 1000.0);
    m_percentageYSpin->setValue(100.0);
    m_percentageYSpin->setSuffix(" %");
    m_percentageYSpin->setDecimals(1);
    connect(m_percentageYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ScalePatternDialog::onPercentageChanged);
    percentYLayout->addWidget(m_percentageYSpin);
    percentLayout->addLayout(percentYLayout);
    
    m_uniformCheck = new QCheckBox(tr("Uniform scaling (maintain aspect ratio)"));
    m_uniformCheck->setChecked(true);
    connect(m_uniformCheck, &QCheckBox::toggled, this, &ScalePatternDialog::onUniformChanged);
    percentLayout->addWidget(m_uniformCheck);
    
    mainLayout->addWidget(percentGroup);
    
    // === Dimension Scaling ===
    QGroupBox* dimGroup = new QGroupBox(tr("Target Dimension"));
    QVBoxLayout* dimLayout = new QVBoxLayout(dimGroup);
    
    QHBoxLayout* widthLayout = new QHBoxLayout();
    m_fitWidth = new QRadioButton(tr("Fit width to:"));
    m_fitWidth->setChecked(true);
    m_targetWidthSpin = new QDoubleSpinBox();
    m_targetWidthSpin->setRange(1.0, 10000.0);
    m_targetWidthSpin->setValue(m_originalWidth);
    m_targetWidthSpin->setSuffix(" mm");
    m_targetWidthSpin->setDecimals(1);
    connect(m_targetWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ScalePatternDialog::onDimensionChanged);
    connect(m_fitWidth, &QRadioButton::toggled, this, &ScalePatternDialog::onDimensionChanged);
    widthLayout->addWidget(m_fitWidth);
    widthLayout->addWidget(m_targetWidthSpin);
    dimLayout->addLayout(widthLayout);
    
    QHBoxLayout* heightLayout = new QHBoxLayout();
    m_fitHeight = new QRadioButton(tr("Fit height to:"));
    m_targetHeightSpin = new QDoubleSpinBox();
    m_targetHeightSpin->setRange(1.0, 10000.0);
    m_targetHeightSpin->setValue(m_originalHeight);
    m_targetHeightSpin->setSuffix(" mm");
    m_targetHeightSpin->setDecimals(1);
    connect(m_targetHeightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ScalePatternDialog::onDimensionChanged);
    connect(m_fitHeight, &QRadioButton::toggled, this, &ScalePatternDialog::onDimensionChanged);
    heightLayout->addWidget(m_fitHeight);
    heightLayout->addWidget(m_targetHeightSpin);
    dimLayout->addLayout(heightLayout);
    
    dimGroup->setEnabled(false);  // Disabled by default
    mainLayout->addWidget(dimGroup);
    
    // Store for later enabling/disabling
    percentGroup->setObjectName("percentGroup");
    dimGroup->setObjectName("dimGroup");
    
    // === Options ===
    QGroupBox* optionsGroup = new QGroupBox(tr("Options"));
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    m_scaleSeamCheck = new QCheckBox(tr("Scale seam allowance width"));
    m_scaleSeamCheck->setChecked(false);
    optionsLayout->addWidget(m_scaleSeamCheck);
    
    m_scaleNotchCheck = new QCheckBox(tr("Scale notch depths"));
    m_scaleNotchCheck->setChecked(true);
    optionsLayout->addWidget(m_scaleNotchCheck);
    
    mainLayout->addWidget(optionsGroup);
    
    // === Preview ===
    QGroupBox* previewGroup = new QGroupBox(tr("Result"));
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);
    
    m_previewLabel = new QLabel();
    m_previewLabel->setText(tr("Original: %.1f × %.1f mm").arg(m_originalWidth).arg(m_originalHeight));
    previewLayout->addWidget(m_previewLabel);
    
    m_resultLabel = new QLabel();
    m_resultLabel->setStyleSheet("font-weight: bold;");
    previewLayout->addWidget(m_resultLabel);
    
    mainLayout->addWidget(previewGroup);
    
    // === Dialog buttons ===
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ScalePatternDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

void ScalePatternDialog::onModeChanged()
{
    bool isPercentMode = m_percentageMode->isChecked();
    
    // Enable/disable groups
    QGroupBox* percentGroup = findChild<QGroupBox*>("percentGroup");
    QGroupBox* dimGroup = findChild<QGroupBox*>("dimGroup");
    
    if (percentGroup) percentGroup->setEnabled(isPercentMode);
    if (dimGroup) dimGroup->setEnabled(!isPercentMode);
    
    updatePreview();
}

void ScalePatternDialog::onPercentageChanged()
{
    if (m_uniformCheck->isChecked()) {
        // Sync the other spinner
        QDoubleSpinBox* sender = qobject_cast<QDoubleSpinBox*>(QObject::sender());
        if (sender == m_percentageXSpin) {
            m_percentageYSpin->blockSignals(true);
            m_percentageYSpin->setValue(m_percentageXSpin->value());
            m_percentageYSpin->blockSignals(false);
        } else if (sender == m_percentageYSpin) {
            m_percentageXSpin->blockSignals(true);
            m_percentageXSpin->setValue(m_percentageYSpin->value());
            m_percentageXSpin->blockSignals(false);
        }
    }
    updatePreview();
}

void ScalePatternDialog::onDimensionChanged()
{
    updatePreview();
}

void ScalePatternDialog::onUniformChanged(bool uniform)
{
    if (uniform) {
        m_percentageYSpin->setValue(m_percentageXSpin->value());
    }
    m_percentageYSpin->setEnabled(!uniform);
}

void ScalePatternDialog::updatePreview()
{
    if (m_percentageMode->isChecked()) {
        m_scaleX = m_percentageXSpin->value() / 100.0;
        m_scaleY = m_percentageYSpin->value() / 100.0;
    } else {
        // Dimension mode - calculate scale from target dimension
        if (m_fitWidth->isChecked()) {
            m_scaleX = m_targetWidthSpin->value() / m_originalWidth;
            m_scaleY = m_scaleX;  // Uniform scaling
        } else {
            m_scaleY = m_targetHeightSpin->value() / m_originalHeight;
            m_scaleX = m_scaleY;  // Uniform scaling
        }
    }
    
    double newWidth = m_originalWidth * m_scaleX;
    double newHeight = m_originalHeight * m_scaleY;
    
    m_resultLabel->setText(tr("Result: %.1f × %.1f mm (%.0f%% × %.0f%%)")
                           .arg(newWidth)
                           .arg(newHeight)
                           .arg(m_scaleX * 100)
                           .arg(m_scaleY * 100));
}

bool ScalePatternDialog::scaleSeamAllowance() const
{
    return m_scaleSeamCheck->isChecked();
}

bool ScalePatternDialog::scaleNotchDepths() const
{
    return m_scaleNotchCheck->isChecked();
}

void ScalePatternDialog::onAccept()
{
    updatePreview();  // Ensure scales are calculated
    accept();
}

} // namespace PatternCAD
