#pragma once

#include <string>
#include <png++/png.hpp>
#include <zlib.h>

template <typename T>
struct Array
{
private:
    size_t rows_;
    size_t cols_;
    T* internal_array;

public:
    constexpr static size_t BMP_WIDTH { 600 };
    constexpr static size_t BMP_HEIGHT { 600 };

    Array()
        : rows_(BMP_HEIGHT), cols_(BMP_WIDTH), internal_array(new T[rows_ * cols_]())
    {
    }

    Array(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), internal_array(new T[rows_ * cols_]())
    {
    }

    Array(const Array& src)
        : rows_(src.rows_), cols_(src.cols_), internal_array(new T[rows_ * cols_]())
    {
        for (size_t i = 0; i < rows_ * cols_; ++i)
            internal_array[i] = src.internal_array[i];
    }

    Array(Array&& src)
        : rows_(src.rows_), cols_(src.cols_), internal_array(src.internal_array)
    {
        src.rows_ = src.cols_ = 0;
        src.internal_array = nullptr;
    }

    Array& operator=(const Array& src)
    {
        if (this != &src) {
            // release any existing memory
            delete[] internal_array;

            // setup member variables
            rows_ = src.rows_;
            cols_ = src.cols_;
            internal_array = new T[rows_ * cols_];

            // copy src to ourselves
            for (size_t i = 0; i < rows_ * cols_; ++i)
                internal_array[i] = src.internal_array[i];
        }

        return *this;
    }

    Array& operator=(Array&& src)
    {
        if (this != &src) {
            delete[] internal_array;
            rows_ = src.rows_;
            cols_ = src.cols_;
            internal_array = src.internal_array;

            src.rows_ = src.cols_ = 0;
            src.internal_array = nullptr;
        }

        return *this;
    }

    ~Array()
    {
        delete[] internal_array;
    }

    inline size_t rows() const { return rows_; }
    inline size_t cols() const { return cols_; }

    inline T& at(size_t c, size_t r)
    {
        return internal_array[r * cols_ + c];
    }

    inline T& at(int c, int r)
    {
        return at(static_cast<size_t>(c), static_cast<size_t>(r));
    }

    inline const T& at(size_t r, size_t c) const
    {
        return internal_array[r * cols_ + c];
    }

    inline const T& at(int r, int c) const
    {
        return at(static_cast<size_t>(r), static_cast<size_t>(c));
    }

    void fill(T const& value)
    {
        for (size_t i = 0; i < rows_ * cols_; ++i)
            internal_array[i] = value;
    }

    void write(std::string const& file_name) const
    {
        png::image<png::rgb_pixel> image(cols(), rows());

        T pixel;
        png::rgb_pixel png_rgb;

        for (size_t row = 0; row < rows(); ++row) {
            for (size_t col = 0; col < cols(); ++col) {
                pixel = at(row, col);

                png_rgb.red = std::get<0>(pixel.first);
                png_rgb.green = std::get<1>(pixel.first);
                png_rgb.blue = std::get<2>(pixel.first);

                image.set_pixel(col, row, png_rgb);
            }
        }

        image.write(file_name);
    }
};
