#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <iostream>
#include <vector>

template <typename T>
class Array2D {
    public:
        Array2D(int width, int height) : width_(width), height_(height), data_(width * height) {}
        T& operator()(int x, int y) { return data_[y * width_ + x]; }
        const T& operator()(int x, int y) const { return data_[y * width_ + x]; }
        int width() const { return width_; }
        int height() const { return height_; }

    private:
        int width_;
        int height_;
        std::vector<T> data_;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Array2D<T>& array) {
    for (int y = 0; y < array.height(); ++y) {
        for (int x = 0; x < array.width(); ++x) {
            os << array(x, y) << " ";
        }
        os << std::endl;
    }
    return os;
}

#endif // ARRAY2D_H
