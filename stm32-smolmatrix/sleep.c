#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencmsis/core_cm3.h>

#include "sleep.h"

#define WFE() __asm__ volatile("wfe")
#define WFI() __asm__ volatile("wfi")

void sleep_enter_shutdown(void) {
	rcc_periph_clock_enable(RCC_PWR);
	rcc_periph_reset_pulse(RST_PWR);

	SCB_SCR |= SCB_SCR_SLEEPDEEP;

	PWR_CR1 &= ~PWR_CR1_LPMS_MASK;
	PWR_CR1 |= PWR_CR1_LPMS_SHUTDOWN << PWR_CR1_LPMS_SHIFT;
	PWR_CR3 &= ~PWR_CR3_APC;
	PWR_SCR = PWR_SCR_CWUF1 | PWR_SCR_CWUF2 | PWR_SCR_CWUF3 | PWR_SCR_CWUF4 | PWR_SCR_CWUF4;
	while (1) WFI();
}
