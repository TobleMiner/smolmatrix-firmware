#include "display.h"
#include "settings.h"

static const void *fbmem = NULL;

static fb_t framebuffer_g;

static unsigned display_brightness = 100;

static bool fb_dirty = true;

const fb_t *display_get_fb(void) {
	return &framebuffer_g;
}

void display_set_fbmem(const void *ptr) {
	fbmem = ptr;
}

void display_set_brightness(unsigned brightness) {
	display_brightness = brightness;
}

unsigned display_get_brightness(void) {
	return display_brightness;
}

void display_update() {
	if (fb_dirty && fbmem) {
		unsigned x ,y;
		const uint8_t *fb = fbmem;

		fb_dirty = false;
		
		for (y = 0; y < FB_HEIGHT; y++) {
			for (x = 0; x < FB_WIDTH; x++) {
				framebuffer_g[y * FB_WIDTH + x] = (uint16_t)fb[y * FB_WIDTH + x] * display_brightness / 100;
			}
		}
	}
}

void display_mark_dirty(void) {
	fb_dirty = true;
}

void display_init(void) {
	display_brightness = settings_get_brightness();
}
