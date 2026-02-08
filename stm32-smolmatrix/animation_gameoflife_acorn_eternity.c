#include "animation_gameoflife_acorn_eternity.h"

#include <string.h>

#include "rng.h"
#include "util.h"

#define UPDATE_RATE_HZ 20

static uint16_t population_history[UPDATE_RATE_HZ * 3] = { 0 };
static unsigned int population_history_idx = 0;
static unsigned int acorn_randomization_cnt = 0;

static void set_cell_wrappep(unsigned int x, unsigned int y) {
	uint8_t *playfield = animation_gameoflife_get_playfield();
	playfield[(y % ANIMATION_GAMEOFLIFE_MAX_HEIGHT) * ANIMATION_GAMEOFLIFE_MAX_WIDHT + x % ANIMATION_GAMEOFLIFE_MAX_WIDHT] = 0xff;
}

static void place_acorn(unsigned int x, unsigned int y) {
	set_cell_wrappep(x + 1, y + 0);
	set_cell_wrappep(x + 3, y + 1);
	set_cell_wrappep(x + 0, y + 2);
	set_cell_wrappep(x + 1, y + 2);
	set_cell_wrappep(x + 4, y + 2);
	set_cell_wrappep(x + 5, y + 2);
	set_cell_wrappep(x + 6, y + 2);
}

static void spawn_random_acorn(void) {
	uint32_t pos = rng_get_u32();
	uint16_t x = pos & 0xffff;
	uint16_t y = pos >> 16;

	place_acorn(x % ANIMATION_GAMEOFLIFE_MAX_WIDHT, y % ANIMATION_GAMEOFLIFE_MAX_HEIGHT);
}

static void post_update(unsigned int total_population) {
	population_history[population_history_idx++] = total_population;
	if (population_history_idx >= ARRAY_SIZE(population_history)) {
		unsigned int i;
		unsigned int population_change = 0;

		population_history_idx = 0;

		for (i = 0; i < ARRAY_SIZE(population_history) - 1; i++) {
			population_change += ABS((int)population_history[i + 1] - (int)population_history[i]);
		}

		if (population_change < ANIMATION_GAMEOFLIFE_MAX_WIDHT * ANIMATION_GAMEOFLIFE_MAX_HEIGHT / 50) {
			spawn_random_acorn();
		}
	}

	acorn_randomization_cnt++;
	if (acorn_randomization_cnt >= UPDATE_RATE_HZ * 30) {
		acorn_randomization_cnt = 0;
		spawn_random_acorn();
	}
}

static void animation_gameoflife_acorn_eternity_start(void) {
	const gameoflife_config_t cfg =  {
		.width = ANIMATION_GAMEOFLIFE_MAX_WIDHT,
		.height = ANIMATION_GAMEOFLIFE_MAX_HEIGHT,
		.update_rate_hz = UPDATE_RATE_HZ,
		.lowpass_filter = 4,
		.post_update_cb = post_update
	};

	memset(population_history, 0, sizeof(population_history));
	population_history_idx = 0;
	animation_gameoflife_start(NULL, &cfg);
	place_acorn(ANIMATION_GAMEOFLIFE_MAX_WIDHT / 2 - 3, ANIMATION_GAMEOFLIFE_MAX_HEIGHT / 2 - 1);
}

static void animation_gameoflife_acorn_eternity_stop(void) {
	animation_gameoflife_stop();
}

animation_t animation_gameoflife_acorn_eternity = {
	animation_gameoflife_acorn_eternity_start,
	animation_gameoflife_acorn_eternity_stop
};
