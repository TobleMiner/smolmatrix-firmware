#include "animation.h"
#include "util.h"

#include "animation_sinefield.h"
#include "animation_flashlight.h"
#include "animation_pixelracer.h"
#include "animation_gameoflife_glider.h"
#include "animation_gameoflife_hwss.h"
#include "animation_gameoflife_tumbler.h"
#include "animation_gameoflife_p16.h"

fb_t animation_fb;

static unsigned animation_id = 0;

static animation_t *animations[] = {
	&animation_sinefield,
	&animation_flashlight,
	&animation_pixelracer,
	&animation_gameoflife_glider,
	&animation_gameoflife_hwss,
	&animation_gameoflife_tumbler,
	&animation_gameoflife_p16,
};

void animation_init() {
	animation_t *animation = animations[animation_id];

	animation->start();
}

void animation_stop() {
	animation_t *animation = animations[animation_id];

	if (animation->stop) {
		animation->stop();
	}
}

void animation_next() {
	animation_t *animation; 

	animation_stop();

	animation_id++;
	animation_id %= ARRAY_SIZE(animations);

	animation = animations[animation_id];

	animation->start();
}
