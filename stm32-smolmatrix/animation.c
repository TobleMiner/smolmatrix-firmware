#include "animation.h"
#include "settings.h"
#include "util.h"

#include "animation_sinefield.h"
#include "animation_flashlight.h"
#include "animation_pixelracer.h"
#include "animation_gameoflife_acorn_eternity.h"
#include "animation_gameoflife_glider.h"
#include "animation_gameoflife_hwss.h"
#include "animation_gameoflife_tumbler.h"
#include "animation_gameoflife_p16.h"
#include "animation_hoa.h"
#include "animation_tm.h"

fb_t animation_fb;

static animation_t *animations[] = {
	&animation_sinefield,
	&animation_flashlight,
	&animation_pixelracer,
	&animation_gameoflife_glider,
	&animation_gameoflife_hwss,
	&animation_gameoflife_tumbler,
	&animation_gameoflife_p16,
	&animation_hoa,
	&animation_tm,
	&animation_gameoflife_acorn_eternity,
};

static unsigned animation_id = 0;

static unsigned get_animation_id_safe(void) {
	unsigned id = settings_get_animation_id();

	if (id >= ARRAY_SIZE(animations)) {
		id = 0;
	}

	return id;
}

void animation_init() {
	animation_t *animation;

	animation_id = get_animation_id_safe();
	animation = animations[animation_id];

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

	settings_set_animation_id(animation_id);

	animation->start();

	settings_store_to_flash();
}
