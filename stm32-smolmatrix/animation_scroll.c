#include <string.h>

#include "animation_scroll.h"
#include "display.h"
#include "os.h"
#include "util.h"

static int scroll_progress = 0;

static os_task_t update_task;

#define UPDATE_RATE_HZ 10

static void animation_scroll_update(void* ctx);

static void animation_scroll_update(void* ctx) {
	int y;

	const animation_scroll_t *scroll = ctx;

	os_schedule_task_relative(&update_task, animation_scroll_update, MS_TO_US(1000 / scroll->speed_pps), (void *)ctx);

	for (y = 0; y < FB_HEIGHT; y++) {

		if (scroll_progress < scroll->width) {
			const void* line_src = scroll->bitmap + scroll->width * y + scroll_progress;
			void* line_dst = animation_fb + FB_WIDTH * y;

			memcpy(line_dst, line_src, MIN(scroll->width - scroll_progress, FB_WIDTH));
		}

		if (scroll_progress + FB_WIDTH >= scroll->width && scroll_progress + FB_WIDTH < scroll->width + scroll->wrap_pixels) {
			unsigned int dummy_pixels = MIN(scroll->width + scroll->wrap_pixels - scroll_progress, FB_WIDTH);
			void* line_dst = animation_fb + FB_WIDTH * y + scroll->width - scroll_progress;

			memset(line_dst, 0, dummy_pixels);
		}

		if (scroll_progress + FB_WIDTH >= scroll->width + scroll->wrap_pixels) {
			const void* line_src_wrap = scroll->bitmap + scroll->width * y;
			void* line_dst_wrap = animation_fb + FB_WIDTH * y + scroll->width + scroll->wrap_pixels - scroll_progress;

			memcpy(line_dst_wrap, line_src_wrap, FB_WIDTH - (scroll->width + scroll->wrap_pixels - scroll_progress));
		}
	}
	
	display_mark_dirty();
	scroll_progress++;
	scroll_progress %= scroll->width + scroll->wrap_pixels;
}

void animation_scroll_start(const animation_scroll_t *scroll) {
	scroll_progress = 0;
	display_set_fbmem(animation_fb);
	animation_scroll_update((void*)scroll);
}

void animation_scroll_stop(void) {
	os_abort_task(&update_task);
}
