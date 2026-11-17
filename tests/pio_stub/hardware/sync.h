#pragma once
#include <stdint.h>
// Host stub: the bus test checks that every masked section is restored.
uint32_t save_and_disable_interrupts(void);
void restore_interrupts(uint32_t status);
