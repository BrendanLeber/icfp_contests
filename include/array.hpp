#pragma once

//Choose 0 or 1 for below options
#if !defined(ARRAY_BOUNDS_CHECK)
#define ARRAY_BOUNDS_CHECK 0
#endif

#include <stdexcept>
#include <vector>
#include <sstream>


namespace bml {

template <class T>
class Array
{
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

	Array& operator=(const Array& src)
	{
		if (this == &src)
			return *this;

		// release any existing memory
		delete[] internal_array;

		// setup member variables
		rows_ = src.rows_;
		cols_ = src.cols_;
		internal_array = new T[rows_ * cols_];

		// copy src to ourselves
		for (size_t i = 0; i < rows_ * cols_; ++i)
			internal_array[i] = src.internal_array[i];

		return *this;
	}

	~Array()
	{
		delete[] internal_array;
	}

	inline size_t rows() const { return rows_; }
	inline size_t cols() const { return cols_; }

	inline T& at(size_t r, size_t c)
	{
#if ARRAY_BOUNDS_CHECK == 1
		if (r >= rows_) {
			std::stringstream ss;
			ss << "Array::at: r (which is " << r << ") >= this->rows() (which is " << rows_ << ")";
			throw std::out_of_range(ss.str());
		}

		if (c >= cols_) {
			std::stringstream ss;
			ss << "Array::at: c (which is " << c << ") >= this->cols() (which is " << cols << ")";
			throw std::out_of_range(ss.str());
		}
#endif

		return internal_array[r * cols_ + c];
	}

	inline const T& at(size_t r, size_t c) const
	{
#if ARRAY_BOUNDS_CHECK == 1
		if (r >= rows_) {
			std::stringstream ss;
			ss << "Array::at: r (which is " << r << ") >= this->rows() (which is " << rows_ << ")";
			throw std::out_of_range(ss.str());
		}

		if (c >= cols_) {
			std::stringstream ss;
			ss << "Array::at: c (which is " << c << ") >= this->cols() (which is " << cols << ")";
			throw std::out_of_range(ss.str());
		}
#endif

		return internal_array[r * cols_ + c];
	}

	void fill(T const &value)
	{
		for (size_t i = 0; i < rows_ * cols_; ++i)
			internal_array[i] = value;
	}

private:
};
}
