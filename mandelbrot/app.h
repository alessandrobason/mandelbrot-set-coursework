#pragma once
#include "sokol.h"
#include "image.h"
#include "mandelbrot.h"

struct app_t {
	sg_pipeline pip{};
	sg_bindings bind{};
	sg_pass_action pass_action{};

	bool keys[SAPP_MAX_KEYCODES]{ false };
	bool held[SAPP_MAX_KEYCODES]{ false };
	bool mouse_down[SAPP_MOUSEBUTTON_INVALID]{ false };
	bool mouse_click[SAPP_MOUSEBUTTON_INVALID]{ false };
	bool mouse_in = false;

	int x = 0;
	int y = 0;
	int w = 300;
	int h = 300;
};

enum action {
	left = SAPP_KEYCODE_LEFT,
	right = SAPP_KEYCODE_RIGHT,
	up = SAPP_KEYCODE_UP,
	down = SAPP_KEYCODE_DOWN,
};