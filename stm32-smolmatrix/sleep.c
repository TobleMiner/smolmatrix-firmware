#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencmsis/core_cm3.h>

#include "sleep.h"

#define WFE() __asm__ volatile("wfe")
#define WFI() __asm__ volatile("wfi")

void sleep_enter_shutdown(void) {
	SCB_SCR |= SCB_SCR_SLEEPDEEP;
	PWR_CR1 &= ~PWR_CR1_LPMS_MASK;
	PWR_CR1 |= PWR_CR1_LPMS_SHUTDOWN << PWR_CR1_LPMS_SHIFT;
	PWR_SR1 &= ~(PWR_SR1_WUF1 | PWR_SR1_WUF2 | PWR_SR1_WUF3 | PWR_SR1_WUF4 | PWR_SR1_WUF5);
	WFI();	
}
