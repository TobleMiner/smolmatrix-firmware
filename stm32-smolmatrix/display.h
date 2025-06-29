#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "framebuffer.h"

void display_set_fbmem(const void *ptr);
void display_set_brightness(unsigned brightness);
unsigned display_get_brightness(void);
void display_mark_dirty(void);
void display_update(void);
void display_init(void);
