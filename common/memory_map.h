/*
STM32G0 Bootloader
Jonah Swain

Memory map (header)
C-accessible memory map from memory_map.ld symbols
*/

/* INCLUDE GUARD */
#pragma once
#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

/* DEPENDENCIES */


/* CONSTANT DEFINITIONS AND MACROS */


/* TYPE DEFINITIONS AND ENUMERATIONS */


/* GLOBAL VARIABLES */
extern int __FLASH_BL_CORE_START;
extern int __FLASH_BL_CORE_LEN;
extern int __FLASH_BL_DATA_START;
extern int __FLASH_BL_DATA_LEN;
extern int __FLASH_APP1_START;
extern int __FLASH_APP1_LEN;
extern int __FLASH_APP2_START;
extern int __FLASH_APP2_LEN;
extern int __SRAM_START;
extern int __SRAM_LEN;

/* FUNCTIONS */


#endif