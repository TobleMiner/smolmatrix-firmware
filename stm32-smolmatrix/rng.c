#include "rng.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rng.h>

void rng_init(void) {
	rcc_periph_clock_enable(RCC_RNG);
	rng_enable();
}

uint32_t rng_get_u32(void) {
	return rng_get_random_blocking();
}
