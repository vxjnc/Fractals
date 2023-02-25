#pragma once

#include <QApplication>
#include <QPixmap>
#include <QLabel>
#include <QWidget>

#include <QResizeEvent>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QKeyEvent>

#include <QFileDialog>

#include <QString>

#include "MainWindow/MainWindow.h"
#include "ui_PaletteWindow.h"
#include "vec3.cpp"

#include <array>

class MainWindow;
class AlgebraicFractal;

class PaletteWindow : public QWidget, public Ui::PaletteWindow
{
    friend MainWindow;
    Q_OBJECT;
public:
    PaletteWindow(MainWindow* mainWindow, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

public slots:
    void dialogSetPath();
    void reset();

private:
    MainWindow* mainWindow;
    AlgebraicFractal* aFractal;
};
