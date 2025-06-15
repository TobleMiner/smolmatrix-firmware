#include "display.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "settings.h"

static const void *fbmem = NULL;
static fb_t framebuffer_g;
static unsigned display_brightness = 100;
static bool fb_dirty = true;
static uint8_t current_mux_row = 0;
static uint8_t current_bright_bit = 0;

void display_set_fbmem(const void *ptr) {
	fbmem = ptr;
}

void display_set_brightness(unsigned brightness) {
	display_brightness = brightness;
}

unsigned display_get_brightness(void) {
	return display_brightness;
}

void display_update() {
	if (fb_dirty && fbmem) {
		unsigned x ,y;
		const uint8_t *fb = fbmem;

		fb_dirty = false;

		for (y = 0; y < FB_HEIGHT; y++) {
			for (x = 0; x < FB_WIDTH; x++) {
				framebuffer_g[y * FB_WIDTH + x] = (uint16_t)fb[y * FB_WIDTH + x] * display_brightness / 100;
			}
		}
	}
}

void display_mark_dirty(void) {
	fb_dirty = true;
}

void display_init(void) {
	display_brightness = settings_get_brightness();

	rcc_periph_clock_enable(RCC_TIM1);
	nvic_enable_irq(NVIC_TIM1_UP_TIM16_IRQ);
	timer_enable_irq(TIM1, TIM_DIER_UIE);
	timer_direction_down(TIM1);
	// Timer 1 is clocked from APB2, set up to tick at 1us
	timer_set_prescaler(TIM1, rcc_apb2_frequency / 1000000UL);
	TIM1_CNT = 4;
	timer_enable_counter(TIM1);
}

void tim1_up_tim16_isr () {
	uint8_t x;
	uint16_t row = 0;

	timer_disable_counter(TIM1);

	for (x = 0; x < 15; x++) {
		/* Set output bit if current brightness bit is set, too */
		if (framebuffer_g[(14 - current_mux_row) * 15 + x] & (1 << current_bright_bit)) {
			row |= (1 << x);
		}
	}
	/* Anode pin mapped to cathode of this row is on PB15 */
	if (row & (1 << current_mux_row)) {
		row |= 1 << 15;
	}
	row &= ~(1 << current_mux_row);
	/* Set all outputs to zero to prevent ghosting */
	gpio_port_write(GPIOB, 0);
	/* Setup hi-z pins first */
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, ~(row | (1 << current_mux_row)) & 0xffff);
	/* Setup outputs */
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, row | (1 << current_mux_row));
	/* Set output values */
	gpio_port_write(GPIOB, row);

	TIM1_CNT = 4U << current_bright_bit;
	timer_clear_flag(TIM1, TIM_SR_UIF);
	timer_enable_counter(TIM1);

	current_mux_row++;
	if (current_mux_row >= 15) {
		current_mux_row = 0;

		current_bright_bit++;
		if (current_bright_bit >= 8) {
			current_bright_bit = 0;
			// TODO: swap buffers for double buffering
		}
	}
}


