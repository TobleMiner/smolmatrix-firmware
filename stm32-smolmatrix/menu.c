#include <stddef.h>

#include "animation.h"
#include "button.h"
#include "display.h"
#include "icons.h"
#include "menu.h"
#include "menu_internal.h"
#include "settings.h"
#include "os.h"

static void go_back(const struct menu_t *menu);
static void change_brightness(const struct menu_t *menu);
static void change_animation(const struct menu_t *menu);
static void enter_app(const struct menu_t *menu);
static void leave_app(const struct menu_t *menu);
static void shutdown(const struct menu_t *menu);

const menu_t menu_back = {
	.icon = &icon_back,
	.entries = NULL,
	.next = &menu_brightness,
	.cb = go_back,
};

const menu_t menu_app = {
	.icon = NULL,
	.entries = NULL,
	.next = &menu_brightness,
	.cb = change_animation,
	.enter = enter_app,
	.leave = leave_app,
};

const menu_t menu_brightness = {
	.icon = &icon_brightness,
	.entries = NULL,
	.next = &menu_poweroff,
	.cb = change_brightness,
};

const menu_t menu_poweroff = {
	.icon = &icon_poweroff,
	.entries = NULL,
	.next = &menu_app,
	.cb = shutdown,
};

static const menu_t *menu_root = &menu_app;
static const menu_t *current_menu = NULL;
static const menu_t *parent = NULL;

static void menu_change(const menu_t *entry) {
	if (current_menu && current_menu->leave) {
		current_menu->leave(current_menu);
	}
	current_menu = entry;
	if (entry) {
		if (entry->icon) {
			display_set_fbmem(entry->icon);
			display_mark_dirty();
		}
		if (entry->enter) {
			entry->enter(entry);
		}
	}
}


static void go_back(const struct menu_t *menu) {
	if (parent) {
		menu_change(parent);
	}
}

static void change_brightness(const struct menu_t *menu) {
	int brightness = display_get_brightness();

	brightness -= 10;
	if (brightness <= 0) {
		brightness += 100;
	}

	display_set_brightness(brightness);
	display_mark_dirty();
	settings_set_brightness(brightness);
	settings_store_to_flash();
}

static void change_animation(const struct menu_t *menu) {
	animation_next();
}

static void enter_app(const struct menu_t *menu) {
	animation_init();
}

static void leave_app(const struct menu_t *menu) {
	animation_stop();
}

void menu_init() {
	menu_change(menu_root);
}

void menu_update() {
	if (button_longpressed(BUTTON_BUTTON)) {
		// next
		if (current_menu && current_menu->next) {
			menu_change(current_menu->next);
		}
	} else if (button_pressed(BUTTON_BUTTON)) {
		// callback
		if (current_menu && current_menu->cb) {
			current_menu->cb(current_menu);
		}
	}
}

static void shutdown(const struct menu_t *menu) {
	(void)menu;

	os_shutdown();
}
