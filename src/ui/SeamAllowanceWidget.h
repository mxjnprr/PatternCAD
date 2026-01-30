/**
 * SeamAllowanceWidget.h
 *
 * Configuration widget for seam allowance tool
 */

#ifndef PATTERNCAD_SEAMALLOWANCEWIDGET_H
#define PATTERNCAD_SEAMALLOWANCEWIDGET_H

#include "geometry/SeamAllowance.h"
#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

namespace PatternCAD {
namespace UI {

/**
 * SeamAllowanceWidget is a floating widget that appears on the canvas
 * to configure seam allowance parameters during tool usage.
 */
class SeamAllowanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SeamAllowanceWidget(QWidget* parent = nullptr);
    ~SeamAllowanceWidget() override;

    // Show widget at specific position
    void showAtPosition(const QPoint& globalPos);

    // Get/Set values
    double width() const;
    void setWidth(double width);

    CornerType cornerType() const;
    void setCornerType(CornerType type);

signals:
    void widthChanged(double width);
    void cornerTypeChanged(CornerType type);

private slots:
    void onWidthChanged(double value);
    void onCornerTypeChanged(int index);

private:
    void setupUi();

    QVBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QDoubleSpinBox* m_widthSpinBox;
    QComboBox* m_cornerTypeCombo;
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_SEAMALLOWANCEWIDGET_H
