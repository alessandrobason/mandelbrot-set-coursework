#include "mandelbrot.h"
#include <iostream>
#include <complex>
#include <cmath>
#include <cassert>

#define print(val) std::cout << #val ": " << val << "\n"
#define printv(vec) std::cout << #vec ": " << vec.x << " " << vec.y << "\n"

using u32 = uint32_t;
constexpr u32 bgcol = 0xFF'00'00'00;

template<typename T>
T lerp(T v0, T v1, T t) {
	return (1 - t) * v0 + t * v1;
}

mandelbrot::mandelbrot(image &imgref, int max_iter, int preferred_divident)
	: imgdata(imgref), max_iterations(max_iter),
	  bg_color(0xFFFFFF00)	{
	int divident = preferred_divident;
	while (divident > 0 && imgdata.width % divident != 0)
		--divident;
	while (divident > 0 && imgdata.height % divident != 0)
		--divident;

	if (divident <= 0) {
		std::cerr << "couldn't find a divident between 1 and " << preferred_divident << "\n";
		return;
	}

	std::cout << "divident: " << divident << "\n";

	step = divident;
	ncells.x = imgdata.width / divident;
	ncells.y = imgdata.height / divident;
}

void mandelbrot::compute() {
	assert(step != 0 && "step value not valid");

	std::vector<std::future<void>> results;

	for (size_t y = 0; y < ncells.y; ++y) {
		for (size_t x = 0; x < ncells.x; ++x) {
			vec2 bx{ x * step, x * step + step };
			vec2 by{ y * step, y * step + step };
			results.emplace_back(pool.enqueue(
				[this](const vec2 &hr, const vec2 &vr) {
					compute_range(hr, vr);
				},
				bx, by)
			);
			//compute_range(bx, by);
		}
	}

	for (auto &r : results)
		r.get();
}

void mandelbrot::compute_range(const vec2 &hor_range, const vec2 &ver_range) {
	assert(ver_range.x >= 0 && ver_range.y <= imgdata.height && "vertical range isn't valid");
	assert(hor_range.x >= 0 && hor_range.y <= imgdata.width && "horizontal range isn't valid");

	double left   = bounds.left();
	double right  = bounds.right();
	double top    = bounds.top();
	double bottom = bounds.bottom();

	for (int y = ver_range.x; y < ver_range.y; ++y) {
		for (int x = hor_range.x; x < hor_range.y; ++x) {
			// Work out the point in the complex plane that
			// corresponds to this pixel in the output image.
			std::complex<double> c(
				left + (x * (right - left) / imgdata.width),
				top + (y * (bottom - top) / imgdata.height)
			);

			// Start off z at (0, 0).
			std::complex<double> z(0.0, 0.0);

			// Iterate z = z^2 + c until z moves more than 2 units
			// away from (0, 0), or we've iterated too many times.
			int iterations = 0;
			while (abs(z) < 2.0 && iterations < max_iterations)
			{
				z = (z * z) + c;

				++iterations;
			}

			if (iterations == max_iterations) {
				// z didn't escape from the circle.
				// This point is in the Mandelbrot set.
				imgdata[y * imgdata.width + x] = 0x00'00'00'00;
			}
			else {
				// z escaped within less than MAX_ITERATIONS
				// iterations. This point isn't in the set.
				imgdata[y * imgdata.width + x] = convert(iterations);
			}
		}
	}
}

uint32_t mandelbrot::convert(int value) {
	// min : max
	// val : max_iterations = lerp : bg_color.x
	struct pixel {
		uint8_t r, g, b
	};

	pixel *p = (pixel*)&bg_color;
	// 0 : 1
	lerp(0, 255, value);
	return bgcol - (value * bg_color / max_iterations);
}