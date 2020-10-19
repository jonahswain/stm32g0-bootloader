/*
STM32G0 Bootloader
Jonah Swain

Bootloader data (header)
Format for bootloader data (struct)
*/

/* INCLUDE GUARD */
#pragma once
#ifndef BOOTLOADER_DATA_H
#define BOOTLOADER_DATA_H

/* DEPENDENCIES */
#include <stdint.h>                 // Fixed width integer data types
#include "app_info.h"               // Application information format
#include "bootloader_common.h"      // Bootloader content accessible by applications

/* CONSTANT DEFINITIONS AND MACROS */


/* TYPE DEFINITIONS AND ENUMERATIONS */

typedef struct __attribute__((packed)) { // Struct type definition for bootloader data
    // Bootloader information
    uint32_t blVersion; // Bootloader version number

    // Bootloader configuration
    BootPriority_T bootPriority; // Boot priority
    VerificationMode_T verificationMode; // Application verification mode
    WatchdogMode_T watchdogMode; // Watchdog mode
    uint8_t _PADDING1[1]; // Padding (1 bytes)

    // Application data
    uint32_t app1_infoChecksum; // Application 1 information section checksum
    uint8_t app1_faultCount; // Application 1 fault count (hard faults and watchdog resets, if enabled)
    uint8_t _PADDING2[3]; // Padding (3 bytes)
    AppInfo_T app1_info; // Application 1 information

    uint32_t app2_infoChecksum; // Application 2 information section checksum
    uint8_t app2_faultCount; // Application 2 fault count (hard faults and watchdog resets, if enabled)
    uint8_t _PADDING3[3]; // Padding (3 bytes)
    AppInfo_T app2_info; // Application 2 information
    
} BootloaderData_T;


/* GLOBAL VARIABLES */


/* FUNCTIONS */


#endif