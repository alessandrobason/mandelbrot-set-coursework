#pragma once
#include <cstdint>
#include <cstdio>
#include "image.h"
#include "thread_pool.hpp"

struct mandelbrot_bounds {
	double left();
	double right();
	double top();
	double bottom();

	void move_hor(double amount);
	void move_ver(double amount);
	void zoom(double amount);

	double get_zoom();

private:
	double zoom_ = 1.0;
	double left_ = -2.0;
	double top_ = -1.0;
	double width_ = 3.0;
	double height_ = 2.0;
};

union vec2 {
	size_t data[2]{ 0, 0 };
	struct {
		size_t  x, y;
	};
	size_t &operator[](size_t index) {
		return data[index];
	}
};

struct mandelbrot {
	mandelbrot_bounds bounds;
	image &imgdata;
	const int max_iterations;
	uint32_t bg_color;

	mandelbrot(image &imgref, int max_iter, int preferred_divident = 64);
	
	void compute();
	void compute_range(const vec2 &hor_range, const vec2 &ver_range);

private:
	vec2 ncells{};
	int step = 0;
	threads::threadpool pool{threads::max_threads()};

	uint32_t convert(int value);
};

inline double mandelbrot_bounds::left() {
	return left_ / zoom_;
}

inline double mandelbrot_bounds::right() {
	return (left_ + width_) / zoom_;
}

inline double mandelbrot_bounds::top() {
	return top_ / zoom_;
}

inline double mandelbrot_bounds::bottom() {
	return (top_ + height_) / zoom_;
}

inline double mandelbrot_bounds::get_zoom() {
	return zoom_;
}

inline void mandelbrot_bounds::move_hor(double amount) {
	left_ += amount;
}
inline void mandelbrot_bounds::move_ver(double amount) {
	top_ += amount;
}
inline void mandelbrot_bounds::zoom(double amount) {
	zoom_ += amount;
}