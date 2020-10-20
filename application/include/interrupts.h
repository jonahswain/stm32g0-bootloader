/*
STM32G0 Bootloader
Jonah Swain

Interrupts (header)
Interrupt request handlers
*/

/* INCLUDE GUARD */
#pragma once
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/* DEPENDENCIES */
#include "stm32g0xx_hal.h"          // STM32G0 hardware abstraction layer

/* CONSTANT DEFINITIONS AND MACROS */


/* TYPE DEFINITIONS AND ENUMERATIONS */


/* GLOBAL VARIABLES */


/* FUNCTIONS */
void SysTick_Handler();

#endif