#pragma once

#include "framebuffer.h"

typedef void (*animation_cb_f)(void);

typedef struct  {
	animation_cb_f start;
	animation_cb_f stop;
} animation_t;

extern fb_t animation_fb;

void animation_init(void);
void animation_next(void);
void animation_stop(void);
