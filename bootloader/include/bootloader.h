/*
STM32G0 Bootloader
Jonah Swain

Bootloader functions (header)
Bootloader and update functions
*/

/* INCLUDE GUARD */
#pragma once
#ifndef BOOTLOADER_H
#define BOOTLOADER_H

/* DEPENDENCIES */
#include <stdint.h>                 // Fixed width integer data types
#include "stm32g0xx_hal.h"          // STM32G0 hardware abstraction layer
#include "stm32g071xx.h"            // STM32G071 device registers
#include "memory_map.h"             // Device memory map
#include "bootloader_common.h"      // Bootloader content accessible by applications
#include "bootloader_data.h"        // Bootloader data format

/* CONSTANT DEFINITIONS AND MACROS */
#define FAULT_THRESHOLD 3           // Number of recorded application faults for application to be considered faulty and not used
#define VECTOR_TABLE_SIZE 47        // Size of the vector table (words/entries) (STM32G071: 16 Cortex-M entries + 31 peripheral entries)

// Watchdog long interval (~30s)
#define WDG_LONG_PRESC IWDG_PRESCALER_256
#define WDG_LONG_RELOAD 3840
// Watchdog medium interval (~5s)
#define WDG_MED_PRESC IWDG_PRESCALER_256
#define WDG_MED_RELOAD 640
// Watchdog short interval (~500ms)
#define WDG_SHORT_PRESC IWDG_PRESCALER_32
#define WDG_SHORT_RELOAD 512
/* TYPE DEFINITIONS AND ENUMERATIONS */


/* GLOBAL VARIABLES */


/* FUNCTIONS */

BootloaderData_T getBootloaderData(); // Get bootloader data from flash
BootloaderStatus_T writeBootloaderData(BootloaderData_T data); // Write bootloader data to flash

__attribute__((naked)) void startApplication(uint32_t stackPointer, uint32_t startupAddress); // Starts an application (sets the main stack pointer to stackPointer and jumps to startupAddress)

BootloaderStatus_T configureWatchdog(WatchdogMode_T mode); // Configure the watchdog
BootloaderStatus_T resetWatchdog(); // Reset the watchdog

uint32_t getBootloaderVersion(); // Get the bootloader version number
BootPriority_T getBootPriority(); // Get the current boot priority
BootloaderStatus_T setBootPriority(BootPriority_T priority); // Set the boot priority
VerificationMode_T getVerificationMode(); // Get the current application verification mode
BootloaderStatus_T setVerificationMode(VerificationMode_T mode); // Set the application verification mode
WatchdogMode_T getWatchdogMode(); // Get the current watchdog mode
BootloaderStatus_T setWatchdogMode(WatchdogMode_T mode); // Set the watchdog mode

BootloaderStatus_T enableProgrammingMode(); // Enable programming mode (to write new application)
BootloaderStatus_T disableProgrammingMode(); // Disable programming mode (after writing application)

uint8_t app1_getFaultCount(); // Get the fault count of application 1
BootloaderStatus_T app1_resetFaultCount(); // Reset the fault count of application 1
AppInfo_T app1_getInfo(); // Get the app info of application 1
BootloaderStatus_T app1_erase(); // Erase application space 1
BootloaderStatus_T app1_write(uint32_t address, uint64_t *data, uint32_t length); // Write data to application space 1 (in 64-bit/double-word pages)
BootloaderStatus_T app1_writeInfo(AppInfo_T info); // Write app 1 info to bootloader data

uint8_t app2_getFaultCount(); // Get the fault count of application 2
BootloaderStatus_T app2_resetFaultCount(); // Reset the fault count of application 2
AppInfo_T app2_getInfo(); // Get the app info of application 2
BootloaderStatus_T app2_erase(); // Erase application space 2
BootloaderStatus_T app2_write(uint32_t address, uint64_t *data, uint32_t length); // Write data to application space 2 (in 64-bit/double-word pages)
BootloaderStatus_T app2_writeInfo(AppInfo_T info); // Write app 2 info to bootloader data

#endif