#include <stdint.h>
#include <string.h>

#include "animation_gameoflife.h"
#include "display.h"
#include "framebuffer.h"
#include "os.h"
#include "util.h"

static gameoflife_playfield_t playfield1;
/*
 = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
*/

static gameoflife_playfield_t playfield2;
static fb_t render_fb;
static fb_t history_fbs[ANIMATION_GAMEOFLIFE_MAX_HISTORY];
unsigned int history_idx = 0;

static uint8_t* playfield_src;
static uint8_t* playfield_dst;

static os_task_t update_task;

static int playfield_width = FB_WIDTH;
static int playfield_height = FB_HEIGHT;
static unsigned int lowpass_filter = 0;

#define DEFAULT_UPDATE_RATE_HZ 7

static unsigned int update_rate_hz = DEFAULT_UPDATE_RATE_HZ;

static gameoflife_post_update_cb_f post_update_cb = NULL;

static void gameoflife_update(uint8_t *dst, uint8_t *src) {
	int x, y, dx, dy;

	for (y = 0; y < playfield_height; y++) {
		for (x = 0; x < playfield_width; x++) {
			unsigned live_neighbours = 0;
			bool alive = !!src[y * playfield_width + x];

			for (dy = -1; dy <= 1; dy++) {
				for (dx = -1; dx <= 1; dx++) {
					int neigh_y = y + dy;
					int neigh_x = x + dx;

					if (!dx && !dy) {
						continue;
					}

					if (neigh_x < 0) {
						neigh_x += playfield_width;
					}
					if (neigh_x >= playfield_width) {
						neigh_x -= playfield_width;
					}
					if (neigh_y < 0) {
						neigh_y += playfield_height;
					}
					if (neigh_y >= playfield_height) {
						neigh_y -= playfield_height;
					}

					if (src[neigh_y * playfield_width + neigh_x]) {
						live_neighbours++;
					}
				}
			}

			if (alive && (live_neighbours == 2 || live_neighbours == 3)) {
				dst[y * playfield_width + x] = 0xff;
			} else if (!alive && live_neighbours == 3) {
				dst[y * playfield_width + x] = 0xff;
			} else {
				dst[y * playfield_width + x] = 0;
			}
		}
	}
}

static unsigned int animation_gameoflife_count_population_in_area(uint8_t *playfield, unsigned int x_start, unsigned int y_start, unsigned int x_end, unsigned int y_end) {
	unsigned int population = 0;
	unsigned int x, y;

	for (y = y_start; y < y_end; y++) {
		for (x = x_start; x < x_end; x++) {
			if (playfield[y * playfield_width + x]) {
				population++;
			}
		}
	}

	return population;
}

static unsigned int animation_gameoflife_render_to_fb(void) {
	unsigned int scale_x = playfield_width / FB_WIDTH;
	unsigned int scale_y = playfield_height / FB_HEIGHT;
	unsigned int scale_bright = 256 / (scale_x * scale_y);
	unsigned int x, y;
	unsigned int total_popultaion = 0;

	for (y = 0; y < FB_HEIGHT; y++) {
		for (x = 0; x < FB_WIDTH; x++) {
			unsigned int pixel_idx = y * FB_WIDTH + x;
			unsigned int population =
				animation_gameoflife_count_population_in_area(playfield_src,
									      x * scale_x,
									      y * scale_y,
									      (x + 1) * scale_x,
									      (y + 1) * scale_y);
			total_popultaion += population;

			if (lowpass_filter) {
				history_fbs[history_idx][pixel_idx] = population;

				unsigned int population_sum = 0;
				for (unsigned int i = 0; i < lowpass_filter; i++) {
					population_sum += history_fbs[i][pixel_idx];
				}
				population = population_sum / lowpass_filter;
			}
			if (population) {
				render_fb[pixel_idx] = population * scale_bright - 1;
			} else {
				render_fb[pixel_idx] = 0;
			}
		}
	}

	if (lowpass_filter) {
		history_idx++;
		if (history_idx >= lowpass_filter) {
			history_idx = 0;
		}
	}

	return total_popultaion;
}

static void animation_gameoflife_update(void* ctx);
static void animation_gameoflife_update(void* ctx) {
	int x, y;
	unsigned int total_population;

	(void)ctx;

	os_schedule_task_relative(&update_task, animation_gameoflife_update, MS_TO_US(1000 / update_rate_hz), NULL);

	if (playfield_width == FB_WIDTH && playfield_height == FB_HEIGHT) {
		total_population =
			animation_gameoflife_count_population_in_area(playfield_src,
								      0,
								      0,
								      playfield_width,
								      playfield_height);
		display_set_fbmem(playfield_src);
	} else {
		total_population = animation_gameoflife_render_to_fb();
		display_set_fbmem(render_fb);
	}
	display_mark_dirty();

	gameoflife_update(playfield_dst, playfield_src);

	SWAP(playfield_src, playfield_dst);

	if (post_update_cb) {
		post_update_cb(total_population);
	}
}

void animation_gameoflife_start(const uint8_t *playfield, const gameoflife_config_t *cfg) {
	playfield_src = playfield1;
	playfield_dst = playfield2;

	if (cfg) {
		if (playfield) {
			memcpy(playfield1, playfield, cfg->width * cfg->height);
		} else {
			memset(playfield1, 0, cfg->width * cfg->height);
		}
		playfield_width = cfg->width;
		playfield_height = cfg->height;
		update_rate_hz = cfg->update_rate_hz;
		lowpass_filter = cfg->lowpass_filter;
		post_update_cb = cfg->post_update_cb;
		history_idx = 0;
		memset(history_fbs, 0, sizeof(history_fbs));
	} else {
		memcpy(playfield1, playfield, sizeof(gameoflife_native_playfield_t));
		playfield_width = FB_WIDTH;
		playfield_height = FB_HEIGHT;
		update_rate_hz = DEFAULT_UPDATE_RATE_HZ;
		lowpass_filter = 0;
		post_update_cb = NULL;
	}

	animation_gameoflife_update(NULL);
}


void animation_gameoflife_stop(void) {
	os_abort_task(&update_task);
}

uint8_t *animation_gameoflife_get_playfield(void) {
	return playfield_src;
}

unsigned int animation_gameoflife_count_population(void) {
	return animation_gameoflife_count_population_in_area(playfield_src,
							     0,
							     0,
							     playfield_width,
							     playfield_height);
}
