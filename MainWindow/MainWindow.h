#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QWindow>
#include <QMessageBox>
#include <QStatusBar>
#include <QPainter>
#include <QClipboard>
#include <QMenuBar>
#include <QTimer>

#include <QTimerEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QFileDialog>
#include <QInputDialog>

#include <QPair>
#include <QStyle>
#include <QStyleFactory>

#include "AlgebraicFractal/AlgebraicFractal.h"
#include "GeometricFractal/GeometricFractal.h"
#include "StochasticFractal/StochasticFractal.h"
#include "ui_MainWindow.h"
#include "PaletteWindow/PaletteWindow.h"

#include <iomanip>
#include <sstream>
#include <numeric>

class PaletteWindow;
class AlgebraicFractal;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
    friend PaletteWindow;
public:
    enum class TypeFractal
    {
        None,
        AlgebraicFractal,
        GeometricFractal,
        StochasticFractal
    };

    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void timerEvent(QTimerEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void changeFractalTransformations(const QString& fractal);

    void saveFractalImage();

public slots:
    void changeFractal();

private:
    AlgebraicFractal aFractal;
    GeometricFractal gFractal;
    StochasticFractal sFractal;

    PaletteWindow* paletteWindow = nullptr;

    TypeFractal typeFractal = TypeFractal::AlgebraicFractal;

    int idTimer;

    vec2<int> posMouse;
    bool isPressedLMB = false;
    bool isPressedRMB = false;

    QImage image;
};

