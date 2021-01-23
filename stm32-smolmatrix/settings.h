#include <stdint.h>

void settings_load_from_flash(void);
void settings_store_to_flash(void);

void settings_set_brightness(uint8_t brightness);
uint8_t settings_get_brightness(void);
void settings_set_animation_id(uint16_t id);
uint16_t settings_get_animation_id(void);
