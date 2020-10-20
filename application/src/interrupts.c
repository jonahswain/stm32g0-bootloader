/*
STM32G0 Bootloader
Jonah Swain

Interrupts (implementation)
Interrupt request handlers
*/

/* DEPENDENCIES */
#include "interrupts.h"

/* CONSTANT DEFINITIONS AND MACROS */


/* GLOBAL VARIABLES */


/* FUNCTIONS */

void SysTick_Handler() {
    HAL_IncTick();
}