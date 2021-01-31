#pragma once
#include <chrono>
#include <iostream>

struct timer {
	using milli = std::chrono::milliseconds;
	using micro = std::chrono::microseconds;

	void start();
	template<typename T>
	long long end();

private:
	std::chrono::steady_clock::time_point begin_;
};

void timer::start() {
	begin_ = std::chrono::steady_clock::now();
}

template<typename T>
long long timer::end() {
	auto fin = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<T>(fin - begin_).count();

	std::cout << "time taken: " << time << "\n";
	return time;
}