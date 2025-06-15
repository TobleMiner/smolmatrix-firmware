#include <string.h>

#include "animation_scroll.h"
#include "display.h"
#include "os.h"
#include "util.h"

static unsigned int scroll_progress = 0;

static os_task_t update_task;

static void animation_scroll_update(void* ctx);

static void animation_scroll_update(void* ctx) {
	int y;
	const animation_scroll_t *scroll = ctx;

	os_schedule_task_relative(&update_task, animation_scroll_update, MS_TO_US(1000 / scroll->speed_pps), (void *)ctx);

	for (y = 0; y < FB_HEIGHT; y++) {
		const uint8_t *src_line = scroll->bitmap + scroll->width * y;
		fb_ptr_t dst_line = animation_fb + FB_WIDTH * y;

		/* Copy row data beyond scroll pos in source data to output */
		if (scroll_progress < scroll->width) {
			const void* src = src_line + scroll_progress;

			memcpy(dst_line, src, MIN(scroll->width - scroll_progress, FB_WIDTH));
		}

		/* Fill remaining destination row length with zeros */
		if (scroll_progress + FB_WIDTH >= scroll->width) {
			unsigned int dummy_pixels = MIN(scroll_progress + FB_WIDTH - scroll->width, FB_WIDTH);
			void* dummy_dst = dst_line + FB_WIDTH - dummy_pixels;

			memset(dummy_dst, 0, dummy_pixels);
		}

		/* Copy remaining row data from start of source line if scrolled beyond wrapping offset */
		if (scroll_progress + FB_WIDTH > scroll->width + scroll->wrap_pixels) {
			unsigned int draw_offset = scroll->width + scroll->wrap_pixels - scroll_progress;
			void* line_dst_wrap = dst_line + draw_offset;
			unsigned int pixels_to_draw = FB_WIDTH - draw_offset;

			memcpy(line_dst_wrap, src_line, pixels_to_draw);
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
