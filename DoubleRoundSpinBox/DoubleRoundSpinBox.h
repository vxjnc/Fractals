#pragma once

#include <QDoubleSpinBox>
#include <QObject>

class DoubleRoundSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    DoubleRoundSpinBox(QWidget* parent = nullptr);

    virtual QString textFromValue(double value) const override;
};
