#pragma once

#include "animation.h"
#include "framebuffer.h"

#define ANIMATION_GAMEOFLIFE_MAX_SCALE 8
#define ANIMATION_GAMEOFLIFE_MAX_WIDHT (FB_WIDTH * ANIMATION_GAMEOFLIFE_MAX_SCALE)
#define ANIMATION_GAMEOFLIFE_MAX_HEIGHT (FB_HEIGHT * ANIMATION_GAMEOFLIFE_MAX_SCALE)
#define ANIMATION_GAMEOFLIFE_MAX_HISTORY 8

typedef uint8_t gameoflife_playfield_t[ANIMATION_GAMEOFLIFE_MAX_WIDHT * ANIMATION_GAMEOFLIFE_MAX_HEIGHT];
typedef uint8_t gameoflife_native_playfield_t[FB_WIDTH * FB_HEIGHT];
typedef void (*gameoflife_post_update_cb_f)(unsigned int population);

#define GAMEOFLIFE_PLAYFIELD(name_, width_, height_) uint8_t name_[width_ * height_]

typedef struct gameoflife_config {
	unsigned int width;
	unsigned int height;
	unsigned int update_rate_hz;
	unsigned int lowpass_filter;
	gameoflife_post_update_cb_f post_update_cb;
} gameoflife_config_t;

void animation_gameoflife_start(const uint8_t *playfield, const gameoflife_config_t *cfg);
void animation_gameoflife_stop(void);
uint8_t *animation_gameoflife_get_playfield(void);
unsigned int animation_gameoflife_count_population(void);
