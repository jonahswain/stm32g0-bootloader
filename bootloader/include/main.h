/*
STM32G0 Bootloader
Jonah Swain

Bootloader main (header)
Bootloader logic code
*/

/* INCLUDE GUARD */
#pragma once
#ifndef BL_MAIN_H
#define BL_MAIN_H

/* DEPENDENCIES */
#include <stdint.h>                 // Fixed width integer data types
#include "stm32g0xx_hal.h"          // STM32G0 hardware abstraction layer
#include "bootloader.h"             // Bootloader functions
#include "bootloader_data.h"        // Bootloader data format
#include "bootloader_common.h"      // Bootloader content accessible by applications

/* CONSTANT DEFINITIONS AND MACROS */
#define BOOTLOADER_VERSION 0x00000001

/* TYPE DEFINITIONS AND ENUMERATIONS */


/* GLOBAL VARIABLES */


/* FUNCTIONS */
void main(); // Main function (bootloader logic)

#endif