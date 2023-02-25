#pragma OPENCL EXTENSION cl_khr_fp64 : enable

void kernel calcMandelbrotSet(int width, int height, double2 re, double2 im, unsigned int maxIter, global unsigned int* iterations, global double2* zs)
{
    int ind = get_global_id(0);
    int2 p = (int2)(ind % width, ind / width);

    double2 C = (double2)(re.x + (re.y - re.x) * (p.x / (double)width), im.x + (im.y - im.x) * (p.y / (double)height));
    double2 Z = 0.0;

    unsigned int n = 0;
    while (Z.x*Z.x + Z.y*Z.y < 4.0 && n < maxIter)
    {
        Z = (double2)((Z.x - Z.y) * (Z.x + Z.y), 2.0 * Z.x * Z.y);
        Z += C;
        ++n;
    }
    iterations[ind] = n;
    zs[ind] = Z;
}

void kernel calcJuliaSet(int width, int height, double2 re, double2 im, unsigned int maxIter, global unsigned int* iterations, global double2* zs, double2 C)
{
    int ind = get_global_id(0);
    int2 p = (int2)(ind % width, ind / width);

    double2 Z = (double2)(re.x + (re.y - re.x) * (p.x / (double)width), im.x + (im.y - im.x) * (p.y / (double)height));

    unsigned int n = 0;
    while (Z.x*Z.x + Z.y*Z.y < 4.0 && n < maxIter)
    {
        Z = (double2)((Z.x - Z.y) * (Z.x + Z.y), 2.0 * Z.x * Z.y);
        Z += C;
        ++n;
    }
    iterations[ind] = n;
    zs[ind] = Z;
}

void kernel calcBurningShip(int width, int height, double2 re, double2 im, unsigned int maxIter, global unsigned int* iterations, global double2* zs)
{
    int ind = get_global_id(0);
    int2 p = (int2)(ind % width, ind / width);

    double2 C = (double2)(re.x + (re.y - re.x) * (p.x / (double)width), im.x + (im.y - im.x) * (p.y / (double)height));
    double2 Z = 0.0;

    unsigned int n = 0;
    while (Z.x*Z.x + Z.y*Z.y < 4.0 && n < maxIter)
    {
        Z = (double2)(Z.x*Z.x - Z.y * Z.y + C.x, 2.0 * fabs(Z.x*Z.y) + C.y);
        ++n;
    }
    iterations[ind] = n;
    zs[ind] = Z;
}

void kernel calcNewtonPools(int width, int height, double2 re, double2 im, unsigned int maxIter, global unsigned int* iterations, global double2* zs)
{
    int ind = get_global_id(0);
    int2 p = (int2)(ind % width, ind / width);

    double2 C = (double2)(re.x + (re.y - re.x) * (p.x / (double)width), im.x + (im.y - im.x) * (p.y / (double)height));
    double2 Z = C;

    double2 copyC = C;

    double2 temp = (double2)(0.0, 0.0);

    unsigned int n = 0;
    do
    {
        // 0.7 * Z + 0.3 * (1 / Z^2)
//        Z = (double2)(complexMult((double2)(0.7, 0.0), Z) + complexMult((double2)(0.3, 0.0),
//                      complexDiv((double2)(1.0, 0.0), complexMult(Z, Z))));

        Z = (double2)((2.0/3.0) * Z.x + ((Z.x*Z.x - Z.y*Z.y) / (3.0 * ((Z.x*Z.x + Z.y*Z.y)*(Z.x*Z.x + Z.y*Z.y)))),
                      (2.0/3.0) * Z.y * (1.0 - (Z.x / ((Z.x*Z.x + Z.y*Z.y)*(Z.x*Z.x + Z.y*Z.y)))));

        temp = Z - copyC;
        copyC = Z;

        ++n;
    }
    while (temp.x * temp.x + temp.y * temp.y > 0.01 && n < maxIter);

    iterations[ind] = n;
    zs[ind] = Z;
}

void kernel draw(int width, int height, unsigned int maxIter, int shift, constant unsigned int* iterations, constant double2* zs, constant char4* palette, global char4* pixels)
{
    int ind = get_global_id(0);

    unsigned int n = iterations[ind];

    if (n == maxIter)
    {
        pixels[ind] = 0;
        return;
    }

    double2 Z = zs[ind];
    double smoothed = native_log10(Z.x*Z.x + Z.y*Z.y);

    unsigned short colorIndex = (unsigned short)(shift + native_sqrt(n + 10.0 - smoothed) * 128.0) % 1024;
    pixels[ind] = palette[colorIndex];
}
