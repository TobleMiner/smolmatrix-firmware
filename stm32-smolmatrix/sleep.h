#pragma once

static inline void __SEV(void) {
	__asm__ volatile("sev");
}

void sleep_enter_shutdown(void);
