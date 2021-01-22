#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>

#include "button.h"
#include "display.h"
#include "framebuffer.h"
#include "gpiod.h"
#include "menu.h"
#include "os.h"
#include "util.h"

static void clock_init(void) {
	// Ensure VOS is set to range 1 (1.2V)
	pwr_set_vos_scale(PWR_SCALE1);
	// Setup flash with 4 WS
	flash_set_ws(4);
	// Switch to HSI
	rcc_osc_on(RCC_HSI16);
	rcc_wait_for_osc_ready(RCC_HSI16);
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI16);
	rcc_wait_for_sysclk_status(RCC_HSI16);
	// Set MSI to 4MHz
	rcc_osc_off(RCC_MSI);
	rcc_set_msi_range(RCC_CR_MSIRANGE_4MHZ);
	rcc_osc_on(RCC_MSI);
	rcc_wait_for_osc_ready(RCC_MSI);
	// Setup pll (fIN=16Mhz, fVCO=160Mhz, fR=80MHz)
	rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16, 7, 80, 0, 0, 0);
	rcc_pll_output_enable(RCC_PLLCFGR_PLLREN);
	rcc_osc_on(RCC_PLL);
	rcc_wait_for_osc_ready(RCC_PLL);
	// Switch to PLL
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	rcc_wait_for_sysclk_status(RCC_PLL);
	// AHB prescaler=1 (80MHz)
	rcc_set_hpre(RCC_CFGR_HPRE_NODIV);
	// APB1 prescaler=16 (5MHz)
	rcc_set_ppre1(RCC_CFGR_PPRE1_DIV16);
	// APB2 prescaler=1 (80MHz)
	rcc_set_ppre2(RCC_CFGR_PPRE2_NODIV);
	rcc_ahb_frequency = MHZ(80);
	rcc_apb1_frequency = MHZ(5);
	rcc_apb2_frequency = MHZ(80);
/*
	// Set MSI to 48MHz
	// Switch to MSI
	rcc_set_sysclk_source(RCC_CFGR_SW_MSI);
	rcc_wait_for_sysclk_status(RCC_MSI);
*/
	
//	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
}

static uint64_t last_adc_update_g = 0;

int main(void) {
	gpiod_init();
	clock_init();
	os_init();
	menu_init();

	while(1) {
		uint8_t x, y, bit;
		const fb_t *fb;
		int brightness;

		os_run();
		button_update();
		menu_update();
		display_update();

		fb = display_get_fb();

		/* 8 loop cycles for 8 bits, exponentially increasing duration */
		for (bit = 0; bit < 8; bit++) {
			/* display is multiplexed over rows */
			for (y = 0; y < 15; y++) {
				int i;
				uint16_t row = 0;

				for (x = 0; x < 15; x++) {
					/* Set output bit if current brightness bit is set, too */
					if ((*fb)[y * 15 + x] & (1 << bit)) {
						row |= (1 << x);
					}	
				}
				/* Anode pin mapped to cathode of this row is on PB15 */
				if (row & (1 << y)) {
					row |= 1 << 15;
				}
				row &= ~(1 << y);
				/* Set all outputs to zero to prevent ghosting */
				gpio_port_write(GPIOB, 0);
				/* Setup hi-z pins first */
				gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, ~(row | (1 << y)) & 0xffff);
				/* Setup outputs */
				gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, row | (1 << y));
				/* Set output values */
				gpio_port_write(GPIOB, row);
				/* Wait for brightness bit to be displayed */
				for (i = 0; i < (16 << bit); i++) __asm("nop;");
			}
		}
	}

	return 0;
}
