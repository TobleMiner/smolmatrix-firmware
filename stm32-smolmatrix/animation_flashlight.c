#include <stddef.h>
#include <string.h>

#include "animation_flashlight.h"
#include "display.h"

static void animation_start(void) {
	memset(animation_fb, 0xff, sizeof(animation_fb));
	display_set_fbmem(animation_fb);
	display_mark_dirty();
}

animation_t animation_flashlight = {
	animation_start,
	NULL
};
