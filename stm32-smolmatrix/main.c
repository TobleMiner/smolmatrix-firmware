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
#include "rng.h"
#include "settings.h"
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
	// Disable MSI
	rcc_osc_off(RCC_MSI);
/*
	rcc_set_msi_range(RCC_CR_MSIRANGE_4MHZ);
	rcc_osc_on(RCC_MSI);
	rcc_wait_for_osc_ready(RCC_MSI);
*/
	// Setup pll (fIN=16Mhz, fVCO=160Mhz, fR=80MHz)
	rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16, 3, 40, 0, 0, 0);
	rcc_pll_output_enable(RCC_PLLCFGR_PLLREN);
	rcc_osc_on(RCC_PLL);
	rcc_wait_for_osc_ready(RCC_PLL);
	// Switch to PLL
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	rcc_wait_for_sysclk_status(RCC_PLL);
/*
	// Disable HSI
	rcc_osc_off(RCC_HSI16);
*/
	// AHB prescaler=1 (80MHz)
	rcc_set_hpre(RCC_CFGR_HPRE_NODIV);
	// APB1 prescaler=16 (5MHz)
	rcc_set_ppre1(RCC_CFGR_PPRE_DIV16);
	// APB2 prescaler=1 (80MHz)
	rcc_set_ppre2(RCC_CFGR_PPRE_NODIV);
	rcc_ahb_frequency = MHZ(80);
	rcc_apb1_frequency = MHZ(5);
	rcc_apb2_frequency = MHZ(80);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	// Enable HSI14 (required by RNG)
	rcc_osc_on(RCC_HSI48);
	rcc_wait_for_osc_ready(RCC_HSI48);
}

static uint64_t last_adc_update_g = 0;

int main(void) {
	gpiod_init();
	clock_init();
	rng_init();
	settings_load_from_flash();
	os_init();
	display_init();
	menu_init();

	while(1) {
		os_run();
		button_update();
		menu_update();
		display_update();
	}

	return 0;
}
