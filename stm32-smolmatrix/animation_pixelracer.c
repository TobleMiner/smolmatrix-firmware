#include <string.h>

#include "animation_pixelracer.h"
#include "display.h"
#include "os.h"
#include "util.h"

static int pixel_idx = 0;

static os_task_t update_task;

#define UPDATE_RATE_HZ 10

static void animation_pixelracer_update(void* ctx);

static void animation_pixelracer_update(void* ctx) {
	int x, y;

	(void)ctx;

	os_schedule_task_relative(&update_task, animation_pixelracer_update, MS_TO_US(1000 / UPDATE_RATE_HZ), NULL);

	animation_fb[pixel_idx] = 0;
	pixel_idx++;
	pixel_idx %= ARRAY_SIZE(animation_fb);
	animation_fb[pixel_idx] = 0xff;	
	display_mark_dirty();
}

static void animation_pixelracer_start(void) {
	pixel_idx = 0;
	animation_fb[pixel_idx] = 0xff;	

	memset(animation_fb, 0, sizeof(animation_fb));
	display_set_fbmem(animation_fb);

	os_schedule_task_relative(&update_task, animation_pixelracer_update, MS_TO_US(1000 / UPDATE_RATE_HZ), NULL);
	display_mark_dirty();
}


static void animation_pixelracer_stop(void) {
	os_abort_task(&update_task);
}

animation_t animation_pixelracer = {
	animation_pixelracer_start,
	animation_pixelracer_stop
};
