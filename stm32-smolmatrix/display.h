#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "framebuffer.h"

const fb_t *display_get_fb(void);
void display_set_fbmem(const void *ptr);
void display_set_brightness(unsigned brightness);
unsigned display_get_brightness(void);
void display_mark_dirty(void);
void display_update(void);
