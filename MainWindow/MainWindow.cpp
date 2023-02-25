#include "MainWindow.h"
typedef QPair<MainWindow::TypeFractal, int> qpair;
Q_DECLARE_METATYPE(qpair);

static void remove(QLayout* layout)
{
    QLayoutItem* child;
    while(layout->count() != 0)
    {
        child = layout->takeAt(0);
        if(child->layout() != 0)
        {
            remove(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    this->setupUi(this);
    fwidget->setLayout(horizontalLayout);

    pageAlgebraic->setLayout(vlytAlgebraic);
    pageGeometric->setLayout(vlytGeometric);
    pageStochastic->setLayout(vlytStochastic);

    fractalLabel->resize(503, 379);

    connect(actnExit, &QAction::triggered, qApp, &QApplication::exit);
    connect(actnSave, &QAction::triggered, this, &MainWindow::saveFractalImage);

    connect(actnPalette, &QAction::triggered, this, [=]()
    {
        if (paletteWindow == nullptr)
        {
            paletteWindow = new PaletteWindow(this);
            if (!std::filesystem::exists("plugins/palettes/monotonic_cubic_interpolation.png"))
            {
                paletteWindow->leditPath->setText("None");
            }
        }
        paletteWindow->show();
    });


    connect(actnAboutQt,      &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(actnAboutProgram, &QAction::triggered, this,
[](){QMessageBox::about(nullptr, "О программе",
R"(Автор: Морозов Никита
Версия: 0.3
Дата компиляции: xx.01.2023)");
});


    idTimer = startTimer(100);

    // Algebraic
    {
        actnMandelbrotSet->setProperty("typeFractal", QVariant::fromValue(qpair{TypeFractal::AlgebraicFractal, AlgebraicFractal::MandelbrotSet}));
        actnJuliaSet->setProperty(     "typeFractal", QVariant::fromValue(qpair{TypeFractal::AlgebraicFractal, AlgebraicFractal::JuliaSet}));
        actnBuddhabrot->setProperty(   "typeFractal", QVariant::fromValue(qpair{TypeFractal::AlgebraicFractal, AlgebraicFractal::Buddhabrot}));
        actnBurningShip->setProperty(  "typeFractal", QVariant::fromValue(qpair{TypeFractal::AlgebraicFractal, AlgebraicFractal::BurningShip}));
        actnNewtonPools->setProperty(  "typeFractal", QVariant::fromValue(qpair{TypeFractal::AlgebraicFractal, AlgebraicFractal::NewtonPools}));

        connect(actnMandelbrotSet, &QAction::changed, this, &MainWindow::changeFractal);
        connect(actnJuliaSet,      &QAction::changed, this, &MainWindow::changeFractal);
        connect(actnBuddhabrot,    &QAction::changed, this, &MainWindow::changeFractal);
        connect(actnBurningShip,   &QAction::changed, this, &MainWindow::changeFractal);
        connect(actnNewtonPools,   &QAction::changed, this, &MainWindow::changeFractal);

        connect(sboxRe, SIGNAL(valueChanged(double)), &aFractal, SLOT(setPosX(double)));
        connect(&aFractal, &AlgebraicFractal::posXChanged, sboxRe, &QDoubleSpinBox::setValue);

        connect(sboxIm, SIGNAL(valueChanged(double)), &aFractal, SLOT(setPosY(double)));
        connect(&aFractal, &AlgebraicFractal::posYChanged, sboxIm, &QDoubleSpinBox::setValue);

        connect(sboxScale, SIGNAL(valueChanged(double)), &aFractal, SLOT(setScale(double)));
        connect(&aFractal, &AlgebraicFractal::scaleChanged, sboxScale, &QDoubleSpinBox::setValue);

        connect(sboxMaxIter, SIGNAL(valueChanged(int)), &aFractal, SLOT(setMaxIter(int)));
        connect(&aFractal, &AlgebraicFractal::maxIterChanged, sboxMaxIter, &QSpinBox::setValue);

        connect(slrReal, &QSlider::valueChanged, this, [=](int value){ aFractal.setCReal(value / 5000.0); });
        connect(&aFractal, &AlgebraicFractal::CRealChanged, this, [=](double value) { slrReal->setValue(value * 5000.0); });

        connect(slrImag, &QSlider::valueChanged, this, [=](int value) { aFractal.setCImag(value / 5000.0); });
        connect(&aFractal, &AlgebraicFractal::CImagChanged, this, [=](double value) { slrImag->setValue(value * 5000.0); });

        slrReal->setVisible(false);
        slrImag->setVisible(false);
    }
    // Geometric
    {
        actnLSystem->setProperty("typeFractal", QVariant::fromValue(qpair{TypeFractal::GeometricFractal, GeometricFractal::LSystem}));

        connect(actnLSystem, &QAction::triggered, this, &MainWindow::changeFractal);


        connect(leditAxiom, SIGNAL(textChanged(const QString&)), &gFractal, SLOT(setAxiom(const QString&)));
        connect(&gFractal, &GeometricFractal::axiomChanged, this, [=](const QString& text)
        {
            int pos = leditAxiom->cursorPosition();
            leditAxiom->setText(text);
            leditAxiom->setCursorPosition(pos);
        });

        connect(leditRules, SIGNAL(textChanged(const QString&)), &gFractal, SLOT(setRules(const QString&)));
        connect(&gFractal, &GeometricFractal::rulesChanged, this, [=](const QString& text)
        {
            int pos = leditRules->cursorPosition();
            leditRules->setText(text);
            leditRules->setCursorPosition(pos);
        });
        gFractal.setRules(gFractal.getStringRules());

        connect(sboxIter, SIGNAL(valueChanged(int)), &gFractal, SLOT(setIterations(int)));
        connect(&gFractal, SIGNAL(iterationsChanged(int)), sboxIter, SLOT(setValue(int)));

        connect(sboxLength, SIGNAL(valueChanged(int)), &gFractal, SLOT(setLength(int)));
        connect(&gFractal, SIGNAL(lengthChanged(int)), sboxLength, SLOT(setValue(int)));

        connect(sboxLAngle, SIGNAL(valueChanged(int)), &gFractal, SLOT(setLAngle(int)));
        connect(&gFractal, SIGNAL(lAngleChanged(int)), sboxLAngle, SLOT(setValue(int)));

        connect(sboxRAngle, SIGNAL(valueChanged(int)), &gFractal, SLOT(setRAngle(int)));
        connect(&gFractal, SIGNAL(rAngleChanged(int)), sboxRAngle, SLOT(setValue(int)));
    }
    // Stochastic
    {
        actnIFS->setProperty("typeFractal", QVariant::fromValue(qpair{TypeFractal::StochasticFractal, StochasticFractal::IFS}));

        connect(actnIFS, &QAction::changed, this, &MainWindow::changeFractal);


        connect(sboxScaleFactor, SIGNAL(valueChanged(double)), &sFractal, SLOT(setScaleFactor(double)));
        connect(sboxScaleFactor, &QDoubleSpinBox::textChanged, this, [=](){ image.fill(Qt::black); });
        connect(&sFractal, &StochasticFractal::scaleFactorChanged, sboxScaleFactor, &QDoubleSpinBox::setValue);

        connect(sboxCountIter, SIGNAL(valueChanged(int)), &sFractal, SLOT(setCountIter(int)));
        connect(&sFractal, SIGNAL(countIterChanged(int)), sboxCountIter, SLOT(setValue(int)));

        for (const auto& [key, _] : sFractal.map()) comboFractal->addItem(key);

        connect(comboFractal, &QComboBox::currentTextChanged, this, &MainWindow::changeFractalTransformations);

        changeFractalTransformations(comboFractal->currentText());
    }
}
MainWindow::~MainWindow()
{
    killTimer(idTimer);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);

    if (isPressedLMB)
    {
        aFractal.plusScreen(posMouse.x, posMouse.y, image.width(), image.height(), 2.0);
    }
    if (isPressedRMB)
    {
        aFractal.minusScreen(posMouse.x, posMouse.y, image.width(), image.height(), 2.0);
    }
};
void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if (typeFractal == TypeFractal::AlgebraicFractal)
    {
        aFractal.draw(image);
    }
    else if (typeFractal == TypeFractal::GeometricFractal)
    {
        gFractal.draw(image);
    }
    else if (typeFractal == TypeFractal::StochasticFractal)
    {
        sFractal.drawPoints(image, comboFractal->currentText());
    }

    fractalLabel->setPixmap(QPixmap::fromImage(image));
};
void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    image = QImage(fractalLabel->size(), QImage::Format::Format_RGB32);
};
void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    qApp->closeAllWindows();
};

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key::Key_Plus)
    {
        aFractal.plusScreen(image.width() / 2.0, image.height() / 2.0, image.width(), image.height(), 1.0);
    }
    else if (event->key() == Qt::Key::Key_Minus)
    {
        aFractal.minusScreen(image.width() / 2.0, image.height() / 2.0, image.width(), image.height(), 1.0);
    }

    else if (event->key() == Qt::Key::Key_Up)
    {
        if (typeFractal == TypeFractal::AlgebraicFractal)
        {
            aFractal.setMaxIter(aFractal.maxIter() + 64);
        }
        else if (typeFractal == TypeFractal::GeometricFractal)
        {
            gFractal.setIterations(gFractal.iterations() + 1);
        }
    }
    else if (event->key() == Qt::Key::Key_Down)
    {
        if (typeFractal == TypeFractal::AlgebraicFractal)
        {
            if (aFractal.maxIter() > 64)
            {
                aFractal.setMaxIter(aFractal.maxIter() - 64);
            }
        }
        else if (typeFractal == TypeFractal::GeometricFractal)
        {
            gFractal.setIterations(gFractal.iterations() - 1);
        }
    }

    else if (event->key() == Qt::Key::Key_Escape)
    {
        if (qApp->focusWidget() != nullptr) qApp->focusWidget()->clearFocus();
    }

    else if (event->modifiers() == Qt::Modifier::CTRL && event->key() == Qt::Key::Key_C)
    {
        QGuiApplication::clipboard()->setImage(image);
    }

    else if (event->key() == Qt::Key::Key_R)
    {
        image.fill(Qt::black);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    posMouse.x = event->windowPos().x();
    posMouse.y = event->windowPos().y();

    if (typeFractal == TypeFractal::GeometricFractal)
    {
        gFractal.offset = static_cast<QPoint>(posMouse);

        image.fill(Qt::black);
    }
}
void MainWindow::mousePressEvent(QMouseEvent* event)
{
    posMouse.x = event->windowPos().x();
    posMouse.y = event->windowPos().y();
    if (event->button() == Qt::MouseButton::RightButton && posMouse.x < fractalLabel->width() && posMouse.y < fractalLabel->height()) isPressedRMB = true;
    if (event->button() == Qt::MouseButton::LeftButton  && posMouse.x < fractalLabel->width() && posMouse.y < fractalLabel->height()) isPressedLMB = true;
}
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::RightButton) isPressedRMB = false;
    if (event->button() == Qt::MouseButton::LeftButton) isPressedLMB = false;
}

void MainWindow::changeFractalTransformations(const QString& fractal)
{
    image.fill(Qt::black);
    remove(vlytTransformations);

    StochasticFractal::transformations t = sFractal.transformation(fractal);

    for (const auto& i : t)
    {
        QHBoxLayout* hlyt = new QHBoxLayout();
        hlyt->setSpacing(0);

        QDoubleSpinBox* aWidget = new QDoubleSpinBox(); aWidget->setValue(i.a);
        QDoubleSpinBox* bWidget = new QDoubleSpinBox(); bWidget->setValue(i.b);
        QDoubleSpinBox* cWidget = new QDoubleSpinBox(); cWidget->setValue(i.c);
        QDoubleSpinBox* dWidget = new QDoubleSpinBox(); dWidget->setValue(i.d);
        QDoubleSpinBox* eWidget = new QDoubleSpinBox(); eWidget->setValue(i.e);
        QDoubleSpinBox* fWidget = new QDoubleSpinBox(); fWidget->setValue(i.f);
        QDoubleSpinBox* pWidget = new QDoubleSpinBox(); pWidget->setValue(i.p);

        aWidget->setDecimals(2); aWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
        bWidget->setDecimals(2); bWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
        cWidget->setDecimals(2); cWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
        dWidget->setDecimals(2); dWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
        eWidget->setDecimals(2); eWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
        fWidget->setDecimals(2); fWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);
        pWidget->setDecimals(2); pWidget->setButtonSymbols(QAbstractSpinBox::NoButtons);

        QSizePolicy qsp(QSizePolicy::Minimum, QSizePolicy::Fixed);
        aWidget->setSizePolicy(qsp);
        bWidget->setSizePolicy(qsp);
        cWidget->setSizePolicy(qsp);
        dWidget->setSizePolicy(qsp);
        eWidget->setSizePolicy(qsp);
        fWidget->setSizePolicy(qsp);
        pWidget->setSizePolicy(qsp);

        hlyt->addWidget(aWidget);
        hlyt->addWidget(bWidget);
        hlyt->addWidget(cWidget);
        hlyt->addWidget(dWidget);
        hlyt->addWidget(eWidget);
        hlyt->addWidget(fWidget);
        hlyt->addWidget(pWidget);

        vlytTransformations->addLayout(hlyt);
    }
}

void MainWindow::saveFractalImage()
{
    using namespace std::string_literals;

    std::string nameFile;

    if (typeFractal == TypeFractal::AlgebraicFractal)
    {
        char* str = new char[62];
        snprintf(str, 62, "%.16g;%.16g", aFractal.posX(), aFractal.posY());

        nameFile += str;

        delete[] str;
    }

    QString pathSave = QFileDialog::getSaveFileName(this, tr("Сохраненить изображения"),
                                                    QString::fromStdString(nameFile + ".png"s),
                                                    " *.png ;; *.jpg");

    if (!pathSave.isNull())
    {
        image.save(pathSave, pathSave.contains(".jpg") ? "JPG" : "PNG");
    }
}

void MainWindow::changeFractal()
{
    QAction* senderAction = reinterpret_cast<QAction*>(sender());

    if (qApp->focusWidget() != nullptr) qApp->focusWidget()->clearFocus();

    QPair<TypeFractal, int> newFractal = senderAction->property("typeFractal").value<qpair>();

    std::array<QAction*, 7> fractals
    {
        actnMandelbrotSet,
        actnJuliaSet,
        actnBuddhabrot,
        actnBurningShip,
        actnNewtonPools,
        actnLSystem,
        actnIFS
    };
    for (auto action : fractals)
    {
        action->blockSignals(true);
        action->setChecked(false);
        action->blockSignals(false);
    }
    slrReal->setVisible(false);
    slrImag->setVisible(false);

    if (newFractal.first == TypeFractal::AlgebraicFractal)
    {
        fractalSettings->setCurrentIndex(0);

        aFractal.setType(static_cast<AlgebraicFractal::Type>(newFractal.second));

        aFractal.setPosX(0.0);
        aFractal.setPosY(0.0);
        aFractal.setScale(1.0);

        if (aFractal.type() == AlgebraicFractal::JuliaSet)
        {
            slrReal->setVisible(true);
            slrImag->setVisible(true);
        }
    }
    else if (newFractal.first == TypeFractal::GeometricFractal)
    {
        fractalSettings->setCurrentIndex(1);

        image.fill(Qt::black);
    }
    else if (newFractal.first == TypeFractal::StochasticFractal)
    {
        fractalSettings->setCurrentIndex(2);
        image.fill(Qt::black);
    }

    typeFractal = newFractal.first;

    senderAction->blockSignals(true);
    senderAction->setChecked(true);
    senderAction->blockSignals(false);
}
