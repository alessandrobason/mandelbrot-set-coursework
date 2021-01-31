#pragma once

#include <cstdint>

struct image {
	size_t width = 0;
	size_t height = 0;
	uint32_t *data = nullptr;

	image() = default;
	image(size_t w, size_t h);
	~image();

	void create(size_t w, size_t h);

	uint32_t &operator[](size_t index);
};

inline image::image(size_t w, size_t h) {
	create(w, h);
}
inline image::~image() {
	if(data)
		delete[] data;
}

inline void image::create(size_t w, size_t h) {
	data = new uint32_t[w * h];
	width = w;
	height = h;
}

inline uint32_t &image::operator[](size_t index) {
	return data[index];
}