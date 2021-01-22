#pragma once

#include <stdbool.h>

#define BUTTON_BUTTON 0

void button_update(void);
bool button_get_state(unsigned button_id);
bool button_pressed(unsigned button_id);
bool button_longpressed(unsigned button_id);
