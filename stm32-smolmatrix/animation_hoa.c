#include <string.h>

#include "animation_hoa.h"
#include "animation_scroll.h"
#include "display.h"
#include "os.h"
#include "util.h"

static const uint8_t hoa_graphics[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xa4, 0xdd, 0x40, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xdd, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0xdb, 0xf5, 0xf6, 0xdf, 0xa6, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0xdd, 0xb5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcb, 0xff, 0xe2, 0xb4, 0xb2, 0xda, 0xff, 0xd6, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0xff, 0xf8, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xae, 0xff, 0xb6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0xff, 0xc0, 0xff, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0xee, 0xeb, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdf, 0xf6, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb2, 0xfb, 0x45, 0xe3, 0xdb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x63, 0xff, 0xba, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xeb, 0xd6, 0x00, 0xad, 0xfe, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0xf1, 0xf0, 0xf0, 0xf0, 0xf0, 0xf6, 0xff, 0x81, 0x00, 0x00, 0x00, 0x88, 0xff, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xff, 0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xff, 0x94, 0x00, 0x4f, 0xfc, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0xb4, 0xad, 0xad, 0xad, 0xad, 0xd0, 0xff, 0x81, 0x00, 0x00, 0x00, 0x8b, 0xff, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xff, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc7, 0xf6, 0x2e, 0x00, 0x00, 0xda, 0xeb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x79, 0xff, 0xac, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0xff, 0x8f, 0x00, 0x00, 0x00, 0x00, 0x16, 0xf6, 0xe2, 0x9d, 0x9d, 0x9d, 0xca, 0xff, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x22, 0xfa, 0xd7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc9, 0xfe, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x97, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0xc9, 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x6d, 0xfa, 0xd6, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd9, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0xce, 0xf5, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0x49, 0xf1, 0xf6, 0xa4, 0x49, 0x42, 0x99, 0xf1, 0xf5, 0x63, 0x00, 0x00, 0x00, 0x00, 0x51, 0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8a, 0xff, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xbe, 0xff, 0x4d, 0x00, 0x00, 0x00, 0x00, 0xa3, 0xff, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0xd9, 0xff, 0xff, 0xff, 0xff, 0xdf, 0x6a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaf, 0xff, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0xf5, 0xd6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3d, 0x56, 0x16, 0x00, 0x00, 0x00, 0x00, 0x35, 0x56, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x91, 0x94, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x56, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#define ANIMATION_WIDTH (sizeof(hoa_graphics) / FB_HEIGHT)

static const animation_scroll_t hoa = {
	.bitmap = hoa_graphics,
	.width = ANIMATION_WIDTH,
	.wrap_pixels = 10,
	.speed_pps = 10,
};

static void animation_hoa_start(void) {
	animation_scroll_start(&hoa);
}


animation_t animation_hoa = {
	animation_hoa_start,
	animation_scroll_stop
};
