#include "AlgebraicFractal.h"

// [From1; From2) -> [To1; To2)
template <typename T> constexpr T norm(double value, double From1, double From2, double To1, double To2)
{
    return (value - From1) / (From2 - From1) * (To2 - To1) + To1;
}

template <typename T> static constexpr vec2<T> complexMult(const vec2<T>& a, const vec2<T>& b)
{
    return vec2<T>(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);
}
template <typename T> static constexpr vec2<T> complexDiv(const vec2<T>& a, const vec2<T>& b)
{
    return vec2<T>((a.x*b.x + a.y*b.y) / (b.x*b.x + b.y*b.y), (a.y*b.x - a.x*b.y) / (b.x*b.x + b.y*b.y));
}

static constexpr double findA(double x1, double y1, double x2, double y2)
{
    return (y1 - y2) / (x1 - x2);
}
static constexpr double findB(double x1, double y1, double x2, double y2)
{
    return y1 - findA(x1, y1, x2, y2) * x1;
}

AlgebraicFractal::AlgebraicFractal()
{
    std::vector<cl::Device> devices;
    cl::Platform::getDefault().getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty())
    {
        std::cerr << "CL_DEVICE_TYPE_GPU = 0" << std::endl;
        cl::Platform::getDefault().getDevices(CL_DEVICE_TYPE_ALL, &devices);

        if (devices.empty())
        {
            std::cerr << "CL_DEVICE_TYPE_ALL = 0" << std::endl;
            isSupportCL = false;
        }
    }

    if (isSupportCL)
    {
        device = devices[0];
        context = cl::Context({device});

        std::ifstream fkernel("kernel_code.cl", std::ios::in);
        std::string kernel_code((std::istreambuf_iterator<char>(fkernel)),
                                (std::istreambuf_iterator<char>()));
        fkernel.close();
        isSupportCL = !kernel_code.empty();

        sources.push_back({kernel_code.data(), kernel_code.size()});
        program = cl::Program(context, sources);

        if (isSupportCL && program.build({device}) != CL_SUCCESS)
        {
            std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            std::exit(2);
        }
    }

    QImage image("monotonic_cubic_interpolation.png");

    if (image.isNull())
    {
        const vec4<unsigned int> p1(0,   7,   100, 0);
        const vec4<unsigned int> p2(32,  107, 203, 164);
        const vec4<unsigned int> p3(237, 255, 255, 430);
        const vec4<unsigned int> p4(255, 170, 0,   658);
        const vec4<unsigned int> p5(0,   2,   0,   878);

        unsigned char r;
        unsigned char g;
        unsigned char b;

        #pragma omp parallel for simd schedule(static)
        for (size_t x = 0; x < palette.size(); ++x)
        {
            if (x < p2.w)
            {
                r = findA(p1.w, p1.x, p2.w, p2.x) * x + findB(p1.w, p1.x, p2.w, p2.x);
                g = findA(p1.w, p1.y, p2.w, p2.y) * x + findB(p1.w, p1.y, p2.w, p2.y);
                b = findA(p1.w, p1.z, p2.w, p2.z) * x + findB(p1.w, p1.z, p2.w, p2.z);
            }
            else if (x < p3.w)
            {
                r = findA(p2.w, p2.x, p3.w, p3.x) * x + findB(p2.w, p2.x, p3.w, p3.x);
                g = findA(p2.w, p2.y, p3.w, p3.y) * x + findB(p2.w, p2.y, p3.w, p3.y);
                b = findA(p2.w, p2.z, p3.w, p3.z) * x + findB(p2.w, p2.z, p3.w, p3.z);
            }
            else if (x < p4.w)
            {
                r = findA(p3.w, p3.x, p4.w, p4.x) * x + findB(p3.w, p3.x, p4.w, p4.x);
                g = findA(p3.w, p3.y, p4.w, p4.y) * x + findB(p3.w, p3.y, p4.w, p4.y);
                b = findA(p3.w, p3.z, p4.w, p4.z) * x + findB(p3.w, p3.z, p4.w, p4.z);
            }
            else if (x < p5.w)
            {
                r = findA(p4.w, p4.x, p5.w, p5.x) * x + findB(p4.w, p4.x, p5.w, p5.x);
                g = findA(p4.w, p4.y, p5.w, p5.y) * x + findB(p4.w, p4.y, p5.w, p5.y);
                b = findA(p4.w, p4.z, p5.w, p5.z) * x + findB(p4.w, p4.z, p5.w, p5.z);
            }
            else
                if (x < 1024)
            {
                r = findA(p5.w, p5.x, palette.size(), p1.x) * x + findB(p5.w, p5.x, palette.size(), p1.x);
                g = findA(p5.w, p5.y, palette.size(), p1.y) * x + findB(p5.w, p5.y, palette.size(), p1.y);
                b = findA(p5.w, p5.z, palette.size(), p1.z) * x + findB(p5.w, p5.z, palette.size(), p1.z);
            }
            palette[x] = qRgb(r, g, b);
        }
    }
    else
    {
        for (size_t x = 0; x < palette.size(); ++x)
        {
            palette[x] = image.pixelColor(norm<int>(x, 0, palette.size(), 0, image.width()), 0).rgb();
        }
    }
}

unsigned int AlgebraicFractal::maxIter() const
{
    return maxIterations;
}
void AlgebraicFractal::setMaxIter(int newMaxIter)
{
    maxIterations = newMaxIter;
    needUpdateIterations = true;
    emit maxIterChanged(newMaxIter);
}

double AlgebraicFractal::posX() const
{
    return (ReRange.x + ReRange.y) / 2.0;
}
double AlgebraicFractal::posY() const
{
    return (ImRange.x + ImRange.y) / 2.0;
}

void AlgebraicFractal::setPosX(double newX)
{
    double x = posX();
    ReRange += newX - x;

    needUpdateIterations = true;
    emit posXChanged(newX);
}
void AlgebraicFractal::setPosY(double newY)
{
    double y = posY();
    ImRange += newY - y;

    needUpdateIterations = true;
    emit posYChanged(newY);
}

double AlgebraicFractal::scale() const
{
    return 0.25 * (ReRange.y - ReRange.x);
}
void AlgebraicFractal::setScale(double newScale)
{
    if (newScale != 0.0)
    {
        vec2<double> pastPos(posX(), posY());

        double pastScale = scale();

        ReRange *= newScale / pastScale;
        ImRange *= newScale / pastScale;

        setPosX(pastPos.x);
        setPosY(pastPos.y);

        needUpdateIterations = true;
        emit(scaleChanged(newScale));
    }
}

void AlgebraicFractal::draw(QImage& image) const
{
    if (static_cast<size_t>(image.width()) * image.height() != cacheIter.size())
    {
        cacheIter.resize(image.width() * image.height());
        cacheZ.resize(image.width() * image.height());
        count.resize(image.width() * image.height());
        needUpdateIterations = true;
    }

    if (needUpdateIterations)
    {
        switch (typeFractal)
        {
        case Type::MandelbrotSet:
            calcMandelbrotSet(image.width(), image.height());
            break;
        case Type::JuliaSet:
            calcJuliaSet(image.width(), image.height());
            break;
        case Type::Buddhabrot:
            calcBuddhabrot(image.width(), image.height());
            break;
        case Type::BurningShip:
            calcBurningShip(image.width(), image.height());
            break;
        case Type::NewtonPools:
            calcNewtonPools(image.width(), image.height());
            break;
        default:
            break;
        }
        needUpdateIterations = false;
    }

    if (typeFractal == Buddhabrot)
    {
        drawBuddhabrot(image);
    }
    else
    {
        if (isSupportCL) drawCL(image);
        else drawCPU(image);
    }
}
void AlgebraicFractal::drawBuddhabrot(QImage& image) const
{
    const int imageWidth = image.width();
    const int imageHeight = image.height();

    constexpr int minIter = 32;

    size_t maxElement = 0;
    for (auto now : count)
    {
        if (minIter < now && now < maxIterations)
        {
            if (now > maxElement) maxElement = now;
        }
    }

    int bright;
    QRgb* lineBits;

    for (int y = 0; y < imageHeight; ++y)
    {
        lineBits = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < imageWidth; ++x)
        {
            bright = qBound(0, norm<int>(count[y * imageWidth + x], 0.0, maxElement, 0.0, 255.0), 255);
            lineBits[x] = qRgb(bright, bright, bright);
        }
    }
}
void AlgebraicFractal::drawCPU(QImage& image) const
{
    QRgb* lineBits;

    for (int y = 0; y < image.height(); ++y)
    {
        lineBits = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x)
        {
            lineBits[x] = getColor(cacheIter[y * image.width() + x], cacheZ[y * image.width() + x]);
        }
    }
}
void AlgebraicFractal::drawCL(QImage& image) const
{
    cl::Buffer buffer_pixels (context, CL_MEM_WRITE_ONLY, image.sizeInBytes());
    cl::Buffer buffer_iters  (context, CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * cacheIter.size(), cacheIter.data());
    cl::Buffer buffer_z      (context, CL_MEM_COPY_HOST_PTR, sizeof(vec2<double>) * cacheZ.size(),    cacheZ.data());
    cl::Buffer buffer_palette(context, CL_MEM_COPY_HOST_PTR, sizeof(QRgb)         * palette.size(),   const_cast<unsigned int*>(palette.data()));

    cl::CommandQueue queue(context, device);

    cl::Kernel kernel = cl::Kernel(program, "draw");
    kernel.setArg(0, image.width());
    kernel.setArg(1, image.height());
    kernel.setArg(2, maxIterations);
    kernel.setArg(3, shiftPalette);
    kernel.setArg(4, buffer_iters);
    kernel.setArg(5, buffer_z);
    kernel.setArg(6, buffer_palette);
    kernel.setArg(7, buffer_pixels);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(image.width() * image.height()), cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(buffer_pixels, CL_FALSE, 0, image.sizeInBytes(), image.bits());
}

void AlgebraicFractal::plusScreen( double x, double y, int width, int height, double speedZoom)
{
    double normX = norm<double>(x, 0.0, width,  ReRange.x, ReRange.y);
    double normY = norm<double>(y, 0.0, height, ImRange.x, ImRange.y);

    vec2<double> pos(posX(), posY());

    double distRe = ReRange.y - ReRange.x;
    double distIm = ImRange.y - ImRange.x;

    ReRange += (normX - pos.x) * speedZoom * 0.05;
    ImRange += (normY - pos.y) * speedZoom * 0.05;

    ReRange.x += distRe * speedZoom * 0.025;
    ReRange.y -= distRe * speedZoom * 0.025;

    ImRange.x += distIm * speedZoom * 0.025;
    ImRange.y -= distIm * speedZoom * 0.025;

    needUpdateIterations = true;
    emit(scaleChanged(scale()));

    emit(posXChanged(posX()));
    emit(posYChanged(posY()));
}
void AlgebraicFractal::minusScreen(double x, double y, int width, int height, double speedZoom)
{
    double normX = norm<double>(x, 0.0, width,  ReRange.x, ReRange.y);
    double normY = norm<double>(y, 0.0, height, ImRange.x, ImRange.y);

    vec2<double> pos(posX(), posY());

    double distRe = ReRange.y - ReRange.x;
    double distIm = ImRange.y - ImRange.x;

    ReRange -= (normX - pos.x) * speedZoom * 0.05;
    ImRange -= (normY - pos.y) * speedZoom * 0.05;

    ReRange.x -= distRe * speedZoom * 0.025;
    ReRange.y += distRe * speedZoom * 0.025;

    ImRange.x -= distIm * speedZoom * 0.025;
    ImRange.y += distIm * speedZoom * 0.025;

    needUpdateIterations = true;
    emit(scaleChanged(scale()));

    emit(posXChanged(posX()));
    emit(posYChanged(posY()));
}

AlgebraicFractal::Type AlgebraicFractal::type()
{
    return typeFractal;
}
void AlgebraicFractal::setType(Type newType)
{
    typeFractal = newType;

    needUpdateIterations = true;
    emit(typeChanged(newType));
}

int AlgebraicFractal::shift() const
{
    return shiftPalette;
}
void AlgebraicFractal::setShift(int newShift)
{
    shiftPalette = newShift;
    emit shiftChanged(newShift);
}

double AlgebraicFractal::CReal()
{
    return juliaC.x;
}
void AlgebraicFractal::setCReal(double newReal)
{
    juliaC.x = newReal;

    needUpdateIterations = true;
    emit CRealChanged(newReal);
}
double AlgebraicFractal::CImag()
{
    return juliaC.y;
}
void AlgebraicFractal::setCImag(double newImag)
{
    juliaC.y = newImag;

    needUpdateIterations = true;
    emit CImagChanged(newImag);
}

void AlgebraicFractal::calcMandelbrotSet(int w, int h) const
{
    if (isSupportCL) calcMandelbrotSetCL(w, h);
    else calcMandelbrotSetCPU(w, h);
}
void AlgebraicFractal::calcJuliaSet(int w, int h) const
{
    if (isSupportCL) calcJuliaSetCL(w, h);
    else calcJuliaSetCPU(w, h);
}
void AlgebraicFractal::calcBuddhabrot(int w, int h) const
{
    calcBuddhabrotCPU(w, h);
}
void AlgebraicFractal::calcBurningShip(int w, int h) const
{
    if (isSupportCL) calcBurningShipCL(w, h);
    else calcBurningShipCPU(w, h);
}
void AlgebraicFractal::calcNewtonPools(int w, int h) const
{
    if (isSupportCL) calcNewtonPoolsCL(w, h);
    else calcNewtonPoolsCPU(w, h);
}

void AlgebraicFractal::calcMandelbrotSetCPU(int w, int h) const
{
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            vec2<double> C(norm<double>(x, 0.0, w - 1, ReRange.x, ReRange.y), norm<double>(y, 0.0, h - 1, ImRange.x, ImRange.y));
            vec2<double> Z;

            unsigned int n = 0;
            while (Z.x*Z.x + Z.y*Z.y < 4.0 && n < maxIterations)
            {
                Z = complexMult(Z, Z);
                Z += C;
                ++n;
            }
            cacheIter[y * w + x] = n;
            cacheZ[y * w + x] = Z;
        }
    }
}
void AlgebraicFractal::calcJuliaSetCPU(int w, int h) const
{
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            vec2<double> Z(norm<double>(x, 0.0, w - 1, ReRange.x, ReRange.y), norm<double>(y, 0.0, h - 1, ImRange.x, ImRange.y));

            unsigned int n = 0;
            while (Z.x*Z.x + Z.y*Z.y < 4.0 && n < maxIterations)
            {
                Z = complexMult(Z, Z);
                Z += juliaC;
                ++n;
            }
            cacheIter[y * w + x] = n;
            cacheZ[y * w + x] = Z;
        }
    }
}
void AlgebraicFractal::calcBuddhabrotCPU(int w, int h) const
{
    vec2<double> C;
    vec2<double> Z;

    size_t n;

    constexpr int countPoints = 100000;
    constexpr int minIter = 32;

    std::vector<vec2<double>> CNonMSet;
    CNonMSet.reserve(countPoints);
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> distRe(ReRange.x, ReRange.y);
    std::uniform_real_distribution<double> distIm(ImRange.x, ImRange.y);

    while (CNonMSet.size() < countPoints)
    {
        C.x = distRe(gen);
        C.y = distIm(gen);

        Z.x = 0.0;
        Z.y = 0.0;

        n = 0;

        while (Z.x * Z.x + Z.y * Z.y < 4.0 && n < maxIterations)
        {
            Z = vec2<double>(Z.x * Z.x - Z.y * Z.y + C.x,
                             2.0 * Z.x * Z.y + C.y);
            ++n;
        }

        if (minIter < n && n < maxIterations)
        {
            CNonMSet.push_back(C);
        }
    }

    std::fill(count.begin(), count.end(), 0);
    int px;
    int py;

    for (const auto& Cnon : CNonMSet)
    {
        Z.x = 0.0;
        Z.y = 0.0;

        n = 0;

        while (Z.x * Z.x + Z.y * Z.y < 4.0 && n < maxIterations)
        {
            Z = vec2<double>(Z.x * Z.x - Z.y * Z.y + Cnon.x,
                             2.0 * Z.x * Z.y + Cnon.y);
            ++n;

            px = norm<int>(Z.x, ReRange.x, ReRange.y, 0.0, w);
            py = norm<int>(Z.y, ImRange.x, ImRange.y, 0.0, h);

            if (0 <= py && py < h &&
                0 <= px && px < w)
            {
                ++count[py * w + px];
            }
        }
    }
}
void AlgebraicFractal::calcBurningShipCPU(int w, int h) const
{
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            vec2<double> C(norm<double>(x, 0.0, w - 1, ReRange.x, ReRange.y), norm<double>(y, 0.0, h - 1, ImRange.x, ImRange.y));
            vec2<double> Z;

            unsigned int n = 0;
            while (Z.x*Z.x + Z.y*Z.y < 4.0 && n < maxIterations)
            {
                Z = vec2<double>(Z.x * Z.x - Z.y * Z.y, 2.0 * std::abs(Z.x * Z.y));
                Z += C;
                ++n;
            }
            cacheIter[y * w + x] = n;
            cacheZ[y * w + x] = Z;
        }
    }
}
void AlgebraicFractal::calcNewtonPoolsCPU(int w, int h) const
{
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            vec2<double> C(norm<double>(x, 0.0, w - 1, ReRange.x, ReRange.y), norm<double>(y, 0.0, h - 1, ImRange.x, ImRange.y));
            vec2<double> Z = C;

            vec2<double> copyC = C;
            vec2<double> temp;

            unsigned int n = 0;
            do
            {
                // 0.7 * Z + 0.3 * (1 / Z^2)
                Z = vec2<double>(complexMult(vec2<double>(0.7), Z) + complexMult(vec2<double>(0.3),
                            complexDiv(vec2<double>(1.0), complexMult(Z, Z))));

                temp = Z - copyC;
                copyC = Z;

                ++n;
            }
            while ((temp.x * temp.x + temp.y * temp.y) > 0.01 && n < maxIterations);

            cacheIter[y * w + x] = n;
            cacheZ[y * w + x] = Z;
        }
    }
}

void AlgebraicFractal::calcMandelbrotSetCL(int w, int h) const
{
    cl::Buffer buffer_iters(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * w * h);
    cl::Buffer buffer_z(    context, CL_MEM_WRITE_ONLY, sizeof(vec2<double>) * w * h);

    cl::CommandQueue queue(context, device);

    cl::Kernel kernel = cl::Kernel(program, "calcMandelbrotSet");
    kernel.setArg(0, w);
    kernel.setArg(1, h);
    kernel.setArg(2, ReRange);
    kernel.setArg(3, ImRange);
    kernel.setArg(4, maxIterations);
    kernel.setArg(5, buffer_iters);
    kernel.setArg(6, buffer_z);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(w * h), cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(buffer_iters, CL_FALSE, 0, sizeof(unsigned int) * h * w, cacheIter.data());
    queue.enqueueReadBuffer(buffer_z,     CL_FALSE, 0, sizeof(vec2<double>) * h * w, cacheZ.data());
}
void AlgebraicFractal::calcJuliaSetCL(int w, int h) const
{
    cl::Buffer buffer_iters(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * w * h);
    cl::Buffer buffer_z(    context, CL_MEM_WRITE_ONLY, sizeof(vec2<double>) * w * h);

    cl::CommandQueue queue(context, device);

    cl::Kernel kernel = cl::Kernel(program, "calcJuliaSet");
    kernel.setArg(0, w);
    kernel.setArg(1, h);
    kernel.setArg(2, ReRange);
    kernel.setArg(3, ImRange);
    kernel.setArg(4, maxIterations);
    kernel.setArg(5, buffer_iters);
    kernel.setArg(6, buffer_z);
    kernel.setArg(7, juliaC);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(w * h), cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(buffer_iters, CL_FALSE, 0, sizeof(unsigned int) * h * w, cacheIter.data());
    queue.enqueueReadBuffer(buffer_z,     CL_FALSE, 0, sizeof(vec2<double>) * h * w, cacheZ.data());
}
void AlgebraicFractal::calcBurningShipCL(int w, int h) const
{
    cl::Buffer buffer_iters(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * w * h);
    cl::Buffer buffer_z(    context, CL_MEM_WRITE_ONLY, sizeof(vec2<double>) * w * h);

    cl::CommandQueue queue(context, device);

    cl::Kernel kernel = cl::Kernel(program, "calcBurningShip");
    kernel.setArg(0, w);
    kernel.setArg(1, h);
    kernel.setArg(2, ReRange);
    kernel.setArg(3, ImRange);
    kernel.setArg(4, maxIterations);
    kernel.setArg(5, buffer_iters);
    kernel.setArg(6, buffer_z);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(w * h), cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(buffer_iters, CL_FALSE, 0, sizeof(unsigned int) * h * w, cacheIter.data());
    queue.enqueueReadBuffer(buffer_z,     CL_FALSE, 0, sizeof(vec2<double>) * h * w, cacheZ.data());
}
void AlgebraicFractal::calcNewtonPoolsCL(int w, int h) const
{
    cl::Buffer buffer_iters(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * w * h);
    cl::Buffer buffer_z(    context, CL_MEM_WRITE_ONLY, sizeof(vec2<double>) * w * h);

    cl::CommandQueue queue(context, device);

    cl::Kernel kernel = cl::Kernel(program, "calcNewtonPools");
    kernel.setArg(0, w);
    kernel.setArg(1, h);
    kernel.setArg(2, ReRange);
    kernel.setArg(3, ImRange);
    kernel.setArg(4, maxIterations);
    kernel.setArg(5, buffer_iters);
    kernel.setArg(6, buffer_z);
    kernel.setArg(7, juliaC);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(w * h), cl::NullRange);
    queue.finish();

    queue.enqueueReadBuffer(buffer_iters, CL_FALSE, 0, sizeof(unsigned int) * h * w, cacheIter.data());
    queue.enqueueReadBuffer(buffer_z,     CL_FALSE, 0, sizeof(vec2<double>) * h * w, cacheZ.data());
}

QRgb AlgebraicFractal::getColor(unsigned int iteration, const vec2<double>& Z) const
{
    if (iteration == maxIterations)
    {
        return Qt::black;
    }

    double smoothed = 0.0;
    if (typeFractal != Type::NewtonPools && typeFractal != Type::BurningShip)
    {
        smoothed = std::log10(Z.x * Z.x + Z.y * Z.y);
    }

    unsigned short colorIndex = static_cast<unsigned short>(shiftPalette + std::sqrt(iteration + 10.0 - smoothed) * palette.size() / 8) % palette.size();
    return palette[colorIndex];
}
