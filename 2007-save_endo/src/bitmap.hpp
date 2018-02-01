#pragma once

#include <cerrno>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <png.h>
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
		auto fp = std::fopen(file_name.c_str(), "wb");
		if (fp == nullptr) {
			throw std::runtime_error(std::strerror(errno));
		}

		auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (png_ptr == nullptr) {
			throw std::runtime_error("could not allocate PNG write struct");
		}

		auto info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == nullptr) {
			throw std::runtime_error("could not allocate PNG info struct");
		}

		if (setjmp(png_jmpbuf(png_ptr))) { /* NOLINT */
			throw std::runtime_error("error during PNG creation");
		}

		png_init_io(png_ptr, fp);

		png_set_IHDR(png_ptr, info_ptr, cols(), rows(), 8, PNG_COLOR_TYPE_RGB,
		    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);

		auto png_row = new png_byte[3 * cols()];

		for (size_t row = 0; row < rows(); ++row) {
			png_bytep png;
			size_t col;
			for (col = 0, png = png_row; col < cols(); ++col, png += 3) {
				auto pixel = at(row, col);
				int r, g, b;
				std::tie(r, g, b) = pixel.first;
				png[0] = static_cast<png_byte>(r);
				png[1] = static_cast<png_byte>(g);
				png[2] = static_cast<png_byte>(b);
			}

			png_write_row(png_ptr, png_row);
		}

		png_write_end(png_ptr, nullptr);

		std::fclose(fp);
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_write_struct(&png_ptr, nullptr);
		delete[] png_row;
	}		
};
