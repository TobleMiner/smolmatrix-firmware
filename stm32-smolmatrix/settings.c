#include <libopencm3/stm32/flash.h>

#include "settings.h"
#include "util.h"

#define ID_EMPTY  0xFF
#define ID_SET    0x00
#define ID_ERASED 0xAA

/*
 * The ERASED marker is required for flash with ECC. With ECC flash
 * erased pages an pages set to 0xFF are not the same!
 */

#define SECTOR_SIZE 2048

#define DWORD_EMPTY 0xFFFFFFFFFFFFFFFF

#define SETTINGS_PAGE 63

typedef union {
	struct __attribute__((packed)) {
		uint8_t id;
		uint16_t animation_id;
		uint8_t brightness;
	} settings;
	uint64_t dword;
} __attribute__((packed)) flash_settings_entry_t;

flash_settings_entry_t settings_log[SECTOR_SIZE / sizeof(flash_settings_entry_t)] __attribute__((section(".settings"))) = { [0 ... 255] = { .dword = DWORD_EMPTY } };

static flash_settings_entry_t settings = {
	.settings = {
		.id = ID_SET,
		.animation_id = 0,
		.brightness = 100
	}
};

// First entry is reserved for ERASED marker
static unsigned settings_idx = 1;

void settings_load_from_flash() {
	for (settings_idx = 1; settings_idx < ARRAY_SIZE(settings_log); settings_idx++) {
		flash_settings_entry_t *entry = &settings_log[settings_idx];

		if (entry->settings.id == ID_SET) {
			settings.settings.animation_id = entry->settings.animation_id;
			settings.settings.brightness = entry->settings.brightness;
		} else {
			break;
		}
	}
}

static void erase_settings_page_unlocked(void) {
	flash_erase_page(SETTINGS_PAGE);
}

void settings_store_to_flash() {
	flash_dcache_disable();
	flash_icache_disable();
	FLASH_ACR &= ~FLASH_ACR_PRFTEN;
	flash_unlock();

	// Check for "ERASED" marker
	if (settings_log[0].settings.id != ID_ERASED) {
		flash_settings_entry_t erased_entry = { .settings = { .id = ID_ERASED } };

		erase_settings_page_unlocked();	
		flash_program_double_word((uint32_t)settings_log, erased_entry.dword);
	}

	if (settings_idx >= ARRAY_SIZE(settings_log)) {
		erase_settings_page_unlocked();
		settings_idx = 1;
	}

	flash_program_double_word((uint32_t)&settings_log[settings_idx], settings.dword);
	settings_idx++;

	flash_lock();
	FLASH_ACR |= FLASH_ACR_PRFTEN;
	flash_icache_enable();
	flash_dcache_reset();
	flash_dcache_enable();
}

void settings_set_brightness(uint8_t brightness) {
	settings.settings.brightness = brightness;
}

uint8_t settings_get_brightness() {
	return settings.settings.brightness;
}

void settings_set_animation_id(uint16_t id) {
	settings.settings.animation_id = id;
}

uint16_t settings_get_animation_id() {
	return settings.settings.animation_id;
}
