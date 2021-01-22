#include <math.h>

#include "animation_sinefield.h"
#include "display.h"
#include "gamma.h"
#include "os.h"

#define M_PI 3.14159265358979323846264338328

#define FIELD_SIZE 32
static uint8_t sine_field_g[FIELD_SIZE * FIELD_SIZE];

#define FIELD_COMPRESSION 1

static int off_x = 0;
static int off_y = 0;

static os_task_t update_task;

#define UPDATE_RATE_HZ 20

static void animation_sinefiled_update(void* ctx) {
	int x, y;

	(void)ctx;

	os_schedule_task_relative(&update_task, animation_sinefiled_update, MS_TO_US(1000 / UPDATE_RATE_HZ), NULL);

	for (y = 0; y < 15; y++) {
		for (x = 0; x < 15; x++) {
			int x_wrapped = (off_x + x) % FIELD_SIZE;
			int y_wrapped = (off_y + y) % FIELD_SIZE;
			animation_fb[y * 15 + x] = sine_field_g[y_wrapped * FIELD_SIZE + x_wrapped];
		}
	}
	off_x++;
	off_y++;
	off_x %= FIELD_SIZE;
	off_y %= FIELD_SIZE;

	display_mark_dirty();
}

static void animation_sinefield_start(void) {
	int x, y;

	/* Generate sine field */
	for (y = 0; y < FIELD_SIZE; y++) {
		for (x = 0; x < FIELD_SIZE; x++) {
			float sinx = sinf(x * M_PI * FIELD_COMPRESSION / (FIELD_SIZE - 1));
			float siny = sinf(y * M_PI * FIELD_COMPRESSION / (FIELD_SIZE - 1));
			float prod = sinx * siny;

			sine_field_g[y * FIELD_SIZE + x] = gamma8_g[(uint8_t)roundf(fabs(prod * 255))];
		}
	}

	off_x = 0;
	off_y = 0;

	display_set_fbmem(animation_fb);

	animation_sinefiled_update(NULL);
}


static void animation_sinefield_stop(void) {
	os_abort_task(&update_task);
}

animation_t animation_sinefield = {
	animation_sinefield_start,
	animation_sinefield_stop
};
