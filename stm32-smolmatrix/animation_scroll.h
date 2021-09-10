#pragma once

#include "animation.h"

/* Height must be FB_HEIGHT */
typedef struct {
	const uint8_t *bitmap;
	unsigned int width;
	unsigned int wrap_pixels;
	unsigned int speed_pps;
} animation_scroll_t;

void animation_scroll_start(const animation_scroll_t *scroll);
void animation_scroll_stop(void);
