#include <stdint.h>
#include <stdbool.h>

#include "button.h"
#include "gpiod.h"
#include "os.h"
#include "util.h"

#define LONGPRESS_MS 500

#define DEBOUNCE_MS 10

typedef struct {
	uint8_t gpio;
	bool state;
	bool last_state;
	bool debounce;
	bool pressed;
	bool longpressed;
	bool ignore_release;
	os_task_t debounce_task;
	os_task_t longpress_task;
} button_t;

#define BUTTON(gpio) { gpio, false, false, false, false, false, false, OS_TASK_INITIALIZER, OS_TASK_INITIALIZER}

static button_t buttons_g[] = {
	BUTTON(GPIO_BUTTON),
};

static void debounce_cb(void *ctx) {
	button_t *button = ctx;

	button->debounce = false;
}

static void longpress_cb(void *ctx) {
	button_t *button = ctx;

	button->longpressed = true;
	button->ignore_release = true;
}

void button_update(void) {
	unsigned i;

	for (i = 0; i < ARRAY_SIZE(buttons_g); i++) {
		button_t *button = &buttons_g[i];
		bool state = gpiod_get(button->gpio);

		if (state != button->last_state) {
			if (!button->debounce) {
				button->state = state;
				if (state) {
					button->ignore_release = false;
					os_schedule_task_relative(&button->longpress_task, longpress_cb, MS_TO_US(LONGPRESS_MS), button);
				} else {
					if (!button->ignore_release) {
						button->pressed = true;
					}
					os_abort_task(&button->longpress_task);
				}
			}
			button->debounce = true;
			os_schedule_task_relative(&button->debounce_task, debounce_cb, MS_TO_US(DEBOUNCE_MS), button);
		} else {
			if (!button->debounce) {
				button->state = state;
			}
		}
		button->last_state = state;
	}
}

bool button_get_state(unsigned button_id) {
	button_t *button = &buttons_g[button_id];

	return button->state;
}

bool button_pressed(unsigned button_id) {
	bool pressed;
	button_t *button = &buttons_g[button_id];

	pressed = button->pressed || button->longpressed;
	button->pressed = false;
	return pressed;
}

bool button_longpressed(unsigned button_id) {
	bool longpressed;
	button_t *button = &buttons_g[button_id];

	longpressed = button->longpressed;
	button->longpressed = false;
	return longpressed;
}
