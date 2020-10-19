/*
STM32G0 Bootloader
Jonah Swain

Bootloader common (header)
Bootloader content accessible to applications
*/

/* INCLUDE GUARD */
#pragma once
#ifndef BOOTLOADER_COMMON_H
#define BOOTLOADER_COMMON_H

/* DEPENDENCIES */
#include <stdint.h>                 // Fixed width integer data types
#include "app_info.h"               // Application information format
#include "memory_map.h"             // Device memory map

/* CONSTANT DEFINITIONS AND MACROS */
#define _BOOTLOADER_FUNCTIONS (struct BootloaderFunctions *) ((uint32_t) &__FLASH_BL_CORE_START + (uint32_t) &__FLASH_BL_CORE_LEN - 0x100) // Paste "struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS;" into main() or wherever needed

/* TYPE DEFINITIONS AND ENUMERATIONS */

typedef enum { // Bootloader function status return type
    BL_OK,                                  // OK (completed without error)
    BL_ERROR,                               // Unspecified error
    BL_ERROR_NOT_IMPLEMENTED,               // Feature not yet implemented
    BL_ERROR_HAL,                           // STM32 HAL returned an error
    BL_ERROR_WRITE_VERIFICATION,            // Data write verification failed
    BL_ERROR_DATA_ALIGNMENT,                // Data/address alignment is not correct
    BL_ERROR_OUT_OF_RANGE                   // Value/address out of permitted range
} BootloaderStatus_T;

typedef enum __attribute__((__packed__)) { // Boot priority enum type
    BOOTPRIO_AUTOMATIC,                     // Choose application automatically based on ID and version
    BOOTPRIO_APP1,                          // Priority application 1
    BOOTPRIO_APP2                           // Priority application 2
} BootPriority_T;

typedef enum __attribute__((__packed__)) { // Application verification mode enum type
    VERIFICATION_OFF,                       // No checksum verification
    VERIFICATION_APP_INFO,                  // Verify application info in bootloader data
    VERIFICATION_VECTOR_TABLE,              // Verify application vector table 
    VERIFICATION_APPLICATION,               // Verify entire application
    VERIFICATION_FULL                       // Verify application info in bootloader data and entire application
} VerificationMode_T;

typedef enum __attribute__((__packed__)) { // Watchdog mode enum type
    WATCHDOG_OFF,                           // Watchdog off
    WATCHDOG_LONG,                          // Watchdog on with long timer
    WATCHDOG_MEDIUM,                        // Watchdog on with medium timer
    WATCHDOG_SHORT                          // Watchdog on with short timer
} WatchdogMode_T;

struct BootloaderFunctions { // Externally (application) accessible bootloader functions
    uint32_t (*getVersion)(void);                                                               // Get the bootloader version number
    BootPriority_T (*getBootPriority)(void);                                                    // Get the current boot priority
    BootloaderStatus_T (*setBootPriority)(BootPriority_T priority);                             // Set the boot priority
    VerificationMode_T (*getVerificationMode)(void);                                            // Get the current verification mode
    BootloaderStatus_T (*setVerificationMode)(VerificationMode_T mode);                         // Set the verification mode
    WatchdogMode_T (*getWatchdogMode)(void);                                                    // Get the current watchdog mode
    BootloaderStatus_T (*setWatchdogMode)(WatchdogMode_T mode);                                 // Set the watchdog mode and configure the watchdog
    BootloaderStatus_T (*resetWatchdog)(void);                                                  // Reset the watchdog timer
    BootloaderStatus_T (*enableProgrammingMode)(void);                                          // Enable programming mode (call before installing an application)
    BootloaderStatus_T (*disableProgrammingMode)(void);                                         // Disable programming mode (call after installing an application)
    uint8_t (*app1_getFaultCount)(void);                                                        // Get the current fault count for application 1
    BootloaderStatus_T (*app1_resetFaultCount)(void);                                           // Reset the fault count for application 1
    AppInfo_T (*app1_getInfo)(void);                                                            // Get the app info for application 1
    BootloaderStatus_T (*app1_erase)(void);                                                     // Erase application space 1
    BootloaderStatus_T (*app1_write)(uint32_t address, uint64_t *data, uint32_t length);        // Write data to application space 1
    BootloaderStatus_T (*app1_writeInfo)(AppInfo_T info);                                       // Write the app info for application 1
    uint8_t (*app2_getFaultCount)(void);                                                        // Get the current fault count for application 2
    BootloaderStatus_T (*app2_resetFaultCount)(void);                                           // Reset the fault count for application 2
    AppInfo_T (*app2_getInfo)(void);                                                            // Get the app info for application 2
    BootloaderStatus_T (*app2_erase)(void);                                                     // Erase application space 2
    BootloaderStatus_T (*app2_write)(uint32_t address, uint64_t *data, uint32_t length);        // Write data to application space 2
    BootloaderStatus_T (*app2_writeInfo)(AppInfo_T info);                                       // Write the app info for application 2
};

/* GLOBAL VARIABLES */


/* FUNCTIONS */


#endif