#include <cmath>
#include <complex>
#include <iostream>
#include <tuple>
#include "array2d.h"

int iterations(std::complex<float> z, const std::complex<float> c, const int max_iter) {
    int n = 0;
    while (abs(z) < 2.0 && ++n < max_iter) {
        z = z*z + c;
    }
    return n;
}

Array2D<int> julia_set(const int width, const int height, const int max_iter) {
    Array2D<int> array(width, height);
    const float scale = 1.5f;
    const std::complex<float> center(-0.5f, 0.0f);
    for (int y = 0; y < array.height(); ++y) {
        for (int x = 0; x < array.width(); ++x) {
            std::complex<float> c(x*scale/array.width() - scale/2.0f, y*scale/array.height() - scale/2.0f);
            array(x, y) = iterations(0, c, max_iter);
        }
    }
    return array;
}

auto get_options(int argc, char* argv[]) {
    int width = 80;
    int height = 40;
    if (argc > 1) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[1]);
    }
    if (argc > 2) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }
    return std::make_tuple(width, height);
}

int main(int argc, char* argv[]) {
    auto [width, height] = get_options(argc, argv);
    const int max_iter = 255;
    Array2D<int> array = julia_set(width, height, max_iter);
    std::cout << array;
    return 0;
}
