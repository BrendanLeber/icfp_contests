#pragma once

#include <sstream>
#include <stdexcept>
#include <vector>

namespace bml {

template <class T>
class Array {
private:
    size_t rows_;
    size_t cols_;
    T* internal_array;

public:
    Array(size_t rows, size_t cols)
        : rows_(rows), cols_(cols), internal_array(new T[rows_ * cols_])
    {
        // rows_ = rows;
        // cols_ = cols;
        // internal_array = new T[rows_ * cols_];
    }

    Array(const Array& src)
        : rows_(src.rows_), cols_(src.cols_), internal_array(new T[rows_ * cols_])
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

    inline const T& at(size_t r, size_t c) const
    {
        return internal_array[r * cols_ + c];
    }

    void fill(T const& value)
    {
        for (size_t i = 0; i < rows_ * cols_; ++i)
            internal_array[i] = value;
    }

private:
};
}
