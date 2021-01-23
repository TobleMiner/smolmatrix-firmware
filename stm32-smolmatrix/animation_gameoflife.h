#pragma once

#include "animation.h"
#include "framebuffer.h"

typedef fb_t gameoflife_playfield_t;

void animation_gameoflife_start(const gameoflife_playfield_t *playfield);
void animation_gameoflife_stop(void);
