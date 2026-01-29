/**
 * DimensionInputOverlay.h
 *
 * Floating input overlay for entering dimensions on canvas
 */

#ifndef PATTERNCAD_DIMENSIONINPUTOVERLAY_H
#define PATTERNCAD_DIMENSIONINPUTOVERLAY_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace PatternCAD {
namespace UI {

/**
 * Resize mode for segment dimensioning
 */
enum class ResizeMode {
    FixStart,   // Start point fixed, end point moves
    FixEnd,     // End point fixed, start point moves
    Center      // Both points move symmetrically
};

/**
 * DimensionInputOverlay is a floating overlay that appears on the canvas
 * to allow direct numeric input for dimensions during drawing operations.
 */
class DimensionInputOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit DimensionInputOverlay(QWidget* parent = nullptr);
    ~DimensionInputOverlay() override;

    // Show overlay at specific position with a prompt
    void showAtPosition(const QPoint& globalPos, const QString& prompt = "Length:",
                       bool withAngle = false, double initialLength = 0.0, double initialAngle = 0.0,
                       bool withResizeMode = false, bool isAngleMode = false);

    // Get entered value in internal units (mm)
    double getValue() const;

    // Get entered angle in degrees
    double getAngle() const;

    // Check if angle is entered
    bool hasAngle() const;

    // Get resize mode
    ResizeMode getResizeMode() const;

signals:
    void valueAccepted(double value, double angle, ResizeMode mode);
    void valueCancelled();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onAccept();
    void onCancel();

private:
    QVBoxLayout* m_layout;
    QLabel* m_promptLabel;
    QLineEdit* m_input;
    QLabel* m_angleLabel;
    QLineEdit* m_angleInput;
    QLabel* m_resizeModeLabel;
    QComboBox* m_resizeModeCombo;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    bool m_isAngleMode;  // Flag to indicate no unit conversion needed

    void setupUI();
};

} // namespace UI
} // namespace PatternCAD

#endif // PATTERNCAD_DIMENSIONINPUTOVERLAY_H
