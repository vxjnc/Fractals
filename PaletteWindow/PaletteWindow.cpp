#include "PaletteWindow.h"

typedef std::array<QRgb, 1024> paletteType;

// [From1; From2) -> [To1; To2)
template <typename T> constexpr T norm(double value, double From1, double From2, double To1, double To2)
{
    return (value - From1) / (From2 - From1) * (To2 - To1) + To1;
}

PaletteWindow::PaletteWindow(MainWindow* mainWindow, QWidget* parent) : QWidget(parent), mainWindow(mainWindow), aFractal(&(mainWindow->aFractal))
{
    this->setupUi(this);
    this->setLayout(verticalLayout);

    slrShift->setValue(aFractal->shift());

    connect(aFractal, &AlgebraicFractal::shiftChanged, slrShift, &QSlider::setValue);
    connect(slrShift, &QSlider::valueChanged,          aFractal, &AlgebraicFractal::setShift);

    connect(btnBrowse, &QPushButton::clicked, this, &PaletteWindow::dialogSetPath);
    connect(btnReset,  &QPushButton::clicked, this, &PaletteWindow::reset);
}

void PaletteWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    const auto& palette = aFractal->palette;
    int shift = aFractal->shift();

    const int width = this->lblPalette->width();
    const int height = this->lblPalette->height();

    QImage image(width, height, mainWindow->image.format());

    QRgb* fLineBits = reinterpret_cast<QRgb*>(image.scanLine(0));
    #pragma omp parallel for simd schedule(static)
    for (int x = 0; x < width; ++x)
    {
        fLineBits[x] = aFractal->palette[norm<int>((x + norm<int>(shift, 0, 1024, 0, width)) % width, 0, width, 0, palette.size())];
    }

    QRgb* lineBits = nullptr;
    #pragma omp parallel for simd schedule(static) private(lineBits)
    for (int y = 1; y < height; ++y)
    {
        lineBits = reinterpret_cast<QRgb*>(image.scanLine(y));
        memcpy(static_cast<void*>(lineBits), static_cast<void*>(fLineBits), width * 4);
    }

    lblPalette->setPixmap(QPixmap::fromImage(image));
}

void PaletteWindow::keyPressEvent(QKeyEvent* event)
{
    if (qApp->focusWidget() == nullptr && event->key() == Qt::Key::Key_Shift)
    {
        mainWindow->keyPressEvent(event);
    }
    else if (event->key() == Qt::Key::Key_Escape)
    {
        if (qApp->focusWidget() != nullptr) qApp->focusWidget()->clearFocus();
    }
}
void PaletteWindow::keyReleaseEvent(QKeyEvent *event)
{
    mainWindow->keyReleaseEvent(event);
}

void PaletteWindow::dialogSetPath()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Открыть файл"), QString("plugins/palettes/"), "*.png ; *.jpg ;; *.*");
    if (path == QString())
    {
        return;
    }
    leditPath->setText(path);

    QImage image(path);
    #pragma omp parallel for simd schedule(static)
    for (size_t x = 0; x < mainWindow->aFractal.palette.size(); ++x)
    {
        mainWindow->aFractal.palette[x] = image.pixelColor(norm<int>(x, 0.0, aFractal->palette.size(), 0.0, image.width()), 0).rgb();
    }
}
void PaletteWindow::reset()
{
    static const char* path = "plugins/palettes/monotonic_cubic_interpolation.png";

    QImage image(path);

    if (image.isNull())
    {
        leditPath->setText("None");
        mainWindow->aFractal.setShift(512);
        return;
    }

    leditPath->setText(path);

    #pragma omp parallel for simd schedule(static)
    for (size_t x = 0; x < aFractal->palette.size(); ++x)
    {
        aFractal->palette[x] = image.pixelColor(norm<int>(x, 0.0, aFractal->palette.size(), 0.0, image.width()), 0).rgb();
    }

    mainWindow->aFractal.setShift(512);
}
