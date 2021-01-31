#include "app.h"
#include "timer.h"

/* == FORWARD DECLARATIONS ================ */
void init();
void frame();
void handle_event(const sapp_event *e);
void cleanup();

void resize(int w, int h);
void update_mandelbrot();
void zoom_in();
void zoom_out();

/* == DATA ================================ */
app_t app;

sg_image img{};
sg_image_desc img_desc{};
image img_data{ 500, 500 };
bool updated_this_frame = false;

//mandelbrot_bounds bounds;
mandelbrot mandel{ img_data, 100, 100 };

/* == INIT ================================ */
void init() {
	sg_desc options{};
	options.context = sapp_sgcontext();
	sg_setup(options);
	sgl_desc_t gl_opts{};
	gl_opts.sample_count = sapp_sample_count();
	sgl_setup(gl_opts);

	size_t imgsz = img_data.width * img_data.height;

	img_desc.width = img_data.width;
	img_desc.height = img_data.height;
	img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
	img_desc.usage = SG_USAGE_DYNAMIC;

	img = sg_make_image(img_desc);

	img_desc.content.subimage[0][0].ptr = img_data.data;
	img_desc.content.subimage[0][0].size = imgsz;

	mandel.bg_color = 0xFF000000; // red

	app.pass_action.colors[0] = {
		SG_ACTION_CLEAR,
		{1.f, 0.f, 0.f, 1.f}
	};

	update_mandelbrot();
}

/* == UPDATE ================================ */
void frame() {
	// == UPDATE =======================
	if (app.keys[action::left]) {
		mandel.bounds.move_hor(-1);
		update_mandelbrot();
	}
	else if (app.keys[action::right]) {
		mandel.bounds.move_hor(1);
		update_mandelbrot();
	}
	else if (app.keys[action::up]) {
		mandel.bounds.move_ver(1);
		update_mandelbrot();
	}
	else if (app.keys[action::down]) {
		mandel.bounds.move_ver(-1);
		update_mandelbrot();
	}

	updated_this_frame = false;

	// == RENDER =======================
	sgl_viewport(app.x, app.y, app.w, app.h, true);

	sgl_defaults();

	sgl_enable_texture();
	sgl_texture(img);

	sgl_push_matrix();
	sgl_scale(0.75f, 0.75f, 1.f);
		sgl_begin_quads();
			sgl_v2f_t2f(-1.f, -1.f,  0.0f,  0.0f);
			sgl_v2f_t2f(-1.f,  1.f,  0.0f,  1.0f);
			sgl_v2f_t2f( 1.f,  1.f,  1.0f,  1.0f);
			sgl_v2f_t2f( 1.f, -1.f,  1.0f,  0.0f);
		sgl_end();
	sgl_pop_matrix();

	sg_begin_default_pass(&app.pass_action, sapp_width(), sapp_height());
	sgl_draw();
	sg_end_pass();
	sg_commit();
}

/* == EVENTS ================================ */
void handle_event(const sapp_event *e) {
	switch (e->type) {
	case SAPP_EVENTTYPE_RESIZED:
		resize(e->window_width, e->window_height);
		break;
	case SAPP_EVENTTYPE_MOUSE_ENTER:
		app.mouse_in = true;
		break;
	case SAPP_EVENTTYPE_MOUSE_LEAVE:
		app.mouse_in = false;
		break;
	case SAPP_EVENTTYPE_MOUSE_DOWN:
		app.mouse_click[e->mouse_button] = app.mouse_down[e->mouse_button] ? false : true;
		app.mouse_down[e->mouse_button] = true;
		if (app.mouse_click[e->mouse_button]) {
			if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
				zoom_in();
			else if (e->mouse_button == SAPP_MOUSEBUTTON_RIGHT)
				zoom_out();
		}
		break;
	case SAPP_EVENTTYPE_MOUSE_UP:
		app.mouse_down[e->mouse_button] = false;
		break;
	case SAPP_EVENTTYPE_KEY_DOWN:
		app.keys[e->key_code] = !app.held[e->key_code];
		app.held[e->key_code] = true;
		break;
	case SAPP_EVENTTYPE_KEY_UP:
		app.keys[e->key_code] = false;
		app.held[e->key_code] = false;
		break;
	default:
		break;
	}
}

/* == CLEANUP ================================ */
void cleanup() {
	sgl_shutdown();
	sg_shutdown();
}

/* == OTHER ================================ */
void update_mandelbrot() {
	static timer t;
	if (!updated_this_frame) {
		updated_this_frame = true;
		app.keys[action::left] = false;
		app.keys[action::right] = false;
		app.keys[action::up] = false;
		app.keys[action::down] = false;
		t.start();
		mandel.compute();
		//compute_mandelbrot(bounds, img_data);
		t.end<timer::milli>();
		sg_update_image(img, img_desc.content);
	}
}

void zoom_in() {
	mandel.bounds.zoom(mandel.bounds.get_zoom());
	update_mandelbrot();
}

void zoom_out() {
	mandel.bounds.zoom(-(mandel.bounds.get_zoom() / 2.0));
	update_mandelbrot();
}

void resize(int w, int h) {
	if (w > h) {
		app.w = app.h = h;
		int dif = (w - h) / 2;
		app.x = dif;
		app.y = 0;
	}
	else {
		app.w = app.h = w;
		int dif = (h - w) / 2;
		app.x = 0;
		app.y = dif;
	}
}

/* == MAIN ================================ */
sapp_desc sokol_main(int argc, char *argv[]) {
	(void)argc; (void)argv;
	sapp_desc result{0};

	result.width = 300;
	result.height = 300;
	result.init_cb = init;
	result.frame_cb = frame;
	result.cleanup_cb = cleanup;
	result.event_cb = handle_event;
	result.window_title = "Mandelbrot set";
	result.fullscreen = false;

	return result;
}

/*
int main(int argc, char *argv[])
{
	cout << "Please wait..." << endl;

	// Start timing
	the_clock::time_point start = the_clock::now();

	// This shows the whole set.
	//compute_mandelbrot(-2.0, 1.0, 1.125, -1.125);

	// This zooms in on an interesting bit of detail.
	compute_mandelbrot(-0.751085, -0.734975, 0.118378, 0.134488);

	// Stop timing
	the_clock::time_point end = the_clock::now();

	// Compute the difference between the two times in milliseconds
	auto time_taken = duration_cast<milliseconds>(end - start).count();
	cout << "Computing the Mandelbrot set took " << time_taken << " ms." << endl;

	return 0;
}
*/
