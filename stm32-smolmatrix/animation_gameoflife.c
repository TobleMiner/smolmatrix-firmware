#include <stdint.h>
#include <string.h>

#include "animation_gameoflife.h"
#include "display.h"
#include "framebuffer.h"
#include "os.h"
#include "util.h"

static uint8_t playfield1[FB_WIDTH * FB_HEIGHT] = {
	0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static uint8_t playfield2[FB_WIDTH * FB_HEIGHT];

static uint8_t* playfield_src;
static uint8_t* playfield_dst;

static os_task_t update_task;

#define UPDATE_RATE_HZ 7

static void gameoflife_update(uint8_t *dst, uint8_t *src) {
	int x, y, dx, dy;

	for (y = 0; y < FB_HEIGHT; y++) {
		for (x = 0; x < FB_WIDTH; x++) {
			unsigned live_neighbours = 0;
			bool alive = !!src[y * FB_WIDTH + x];

			for (dy = -1; dy <= 1; dy++) {
				for (dx = -1; dx <= 1; dx++) {
					int neigh_y = y + dy;
					int neigh_x = x + dx;

					if (!dx && !dy) {
						continue;
					}

					if (neigh_x < 0) {
						neigh_x += FB_WIDTH;
					}
					if (neigh_x >= FB_WIDTH) {
						neigh_x -= FB_WIDTH;
					}
					if (neigh_y < 0) {
						neigh_y += FB_HEIGHT;
					}
					if (neigh_y >= FB_HEIGHT) {
						neigh_y -= FB_HEIGHT;
					}

					if (src[neigh_y * FB_WIDTH + neigh_x]) {
						live_neighbours++;
					}
				}
			}

			if (alive && (live_neighbours == 2 || live_neighbours == 3)) {
				dst[y * FB_WIDTH + x] = 0xff;
			} else if (!alive && live_neighbours == 3) {
				dst[y * FB_WIDTH + x] = 0xff;
			} else {
				dst[y * FB_WIDTH + x] = 0;
			}
		}	
	}	
}

static void animation_gameoflife_update(void* ctx);

static void animation_gameoflife_update(void* ctx) {
	int x, y;

	(void)ctx;

	os_schedule_task_relative(&update_task, animation_gameoflife_update, MS_TO_US(1000 / UPDATE_RATE_HZ), NULL);

	display_set_fbmem(playfield_src);
	display_mark_dirty();

	gameoflife_update(playfield_dst, playfield_src);

	SWAP(playfield_src, playfield_dst);
}

static void animation_gameoflife_start(void) {
	playfield_src = playfield1;
	playfield_dst = playfield2;

	animation_gameoflife_update(NULL);
}


static void animation_gameoflife_stop(void) {
	os_abort_task(&update_task);
}

animation_t animation_gameoflife = {
	animation_gameoflife_start,
	animation_gameoflife_stop
};
