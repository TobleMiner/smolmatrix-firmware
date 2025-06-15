#pragma once

#include <stdint.h>
#include <string.h>

#define FB_WIDTH  15
#define FB_HEIGHT 15

typedef uint8_t fb_t[FB_WIDTH * FB_HEIGHT];
typedef uint8_t *fb_ptr_t;
