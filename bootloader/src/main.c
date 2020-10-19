/*
STM32G0 Bootloader
Jonah Swain

Bootloader main (implementation)
Bootloader logic code
*/

/* DEPENDENCIES */
#include "main.h"

/* CONSTANT DEFINITIONS AND MACROS */


/* GLOBAL VARIABLES */


/* FUNCTIONS */

void main() { // Main function (bootloader logic)
    __HAL_RCC_SYSCFG_CLK_ENABLE(); // Enable sysconfig module clock
    __HAL_RCC_PWR_CLK_ENABLE(); // Enable PWR module clock
    
    // Check if reset was triggered by watchdog
    uint8_t errorCondition = 0;
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
        errorCondition = 1;
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }

    BootloaderData_T bootloaderData = getBootloaderData(); // Get bootloader data from flash
    
    if ((bootloaderData.blVersion == 0x00000000) || (bootloaderData.blVersion == 0xFFFFFFFF)) { // Check for bootloader data initialisation
        // Initialise default values
        bootloaderData.blVersion = BOOTLOADER_VERSION;
        bootloaderData.bootPriority = BOOTPRIO_AUTOMATIC;
        bootloaderData.verificationMode = VERIFICATION_OFF;
        bootloaderData.watchdogMode = WATCHDOG_OFF;
        bootloaderData.app1_infoChecksum = 0xFFFFFFFF;
        bootloaderData.app1_faultCount = 0;
        bootloaderData.app1_info.ID = 1;
        bootloaderData.app1_info.version = 1;
        bootloaderData.app1_info.size = 1;
        bootloaderData.app1_info.vectblChecksum = 0xFFFFFFFF;
        bootloaderData.app1_info.appChecksum = 0xFFFFFFFF;
        bootloaderData.app2_infoChecksum = 0xFFFFFFFF;
        bootloaderData.app2_faultCount = 0;
        bootloaderData.app2_info.ID = 1;
        bootloaderData.app2_info.version = 1;
        bootloaderData.app2_info.size = 1;
        bootloaderData.app2_info.vectblChecksum = 0xFFFFFFFF;
        bootloaderData.app2_info.appChecksum = 0xFFFFFFFF;
        writeBootloaderData(bootloaderData);
    }

    if (bootloaderData.blVersion != BOOTLOADER_VERSION) { // Check if stored version number matches current version number
        bootloaderData.blVersion = BOOTLOADER_VERSION; // Update version number
        writeBootloaderData(bootloaderData); // Write back to flash
    }

    // Check for application exclusion factors
    uint8_t app1Exclusion = 0;
    uint8_t app2Exclusion = 0;

    // Check for app not installed
    if (bootloaderData.app1_info.ID == 0 || bootloaderData.app1_info.ID == 0xFFFFFFFF || bootloaderData.app1_info.size == 0 || bootloaderData.app1_info.size == 0xFFFFFFFF) {
        app1Exclusion = 1;
    }
    if (bootloaderData.app2_info.ID == 0 || bootloaderData.app2_info.ID == 0xFFFFFFFF || bootloaderData.app2_info.size == 0 || bootloaderData.app2_info.size == 0xFFFFFFFF) {
        app2Exclusion = 1;
    }

    // Check for fault threshold exceeded
    if (bootloaderData.app1_faultCount >= FAULT_THRESHOLD) {
        app1Exclusion = 1;
    }
    if (bootloaderData.app2_faultCount >= FAULT_THRESHOLD) {
        app2Exclusion = 1;
    }

    // Verify application if appropriate
    if (bootloaderData.verificationMode != VERIFICATION_OFF) {
        __HAL_RCC_CRC_CLK_ENABLE(); // Enable CRC module clock
        // Configure CRC handle
        CRC_HandleTypeDef crcHandle;
        crcHandle.Instance = CRC;
        crcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
        crcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
        crcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
        crcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
        crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
        HAL_CRC_Init(&crcHandle); // Initialise CRC module

        if (bootloaderData.verificationMode == VERIFICATION_APP_INFO || bootloaderData.verificationMode == VERIFICATION_FULL) {
            // Verify app info
            if (~HAL_CRC_Calculate(&crcHandle, (uint32_t *) &bootloaderData.app1_info, sizeof(bootloaderData.app1_info)) != bootloaderData.app1_infoChecksum) {
                app1Exclusion = 1;
            }
            if (~HAL_CRC_Calculate(&crcHandle, (uint32_t *) &bootloaderData.app2_info, sizeof(bootloaderData.app2_info)) != bootloaderData.app2_infoChecksum) {
                app2Exclusion = 1;
            }
        }

        if (bootloaderData.verificationMode == VERIFICATION_VECTOR_TABLE) {
            // Verify app vector table
            if (~HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP1_START, VECTOR_TABLE_SIZE*4) != bootloaderData.app1_info.vectblChecksum) {
                app1Exclusion = 1;
            }
            if (~HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP2_START, VECTOR_TABLE_SIZE*4) != bootloaderData.app2_info.vectblChecksum) {
                app2Exclusion = 1;
            }
        }

        if (bootloaderData.verificationMode == VERIFICATION_APPLICATION || bootloaderData.verificationMode == VERIFICATION_FULL) {
            // Verify application
            if (~HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP1_START, bootloaderData.app1_info.size) != bootloaderData.app1_info.appChecksum) {
                app1Exclusion = 1;
            }
            if (~HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP2_START, bootloaderData.app2_info.size) != bootloaderData.app2_info.appChecksum) {
                app2Exclusion = 1;
            }
        }

        HAL_CRC_DeInit(&crcHandle); // De-initialise CRC module
        __HAL_RCC_CRC_CLK_DISABLE(); // Disable CRC module clock
    }

    // if (bootloaderData.verificationMode != VERIFICATION_OFF) {
    //     __HAL_RCC_CRC_CLK_ENABLE(); // Enable CRC module clock
    //     // Configure CRC handle
    //     CRC_HandleTypeDef crcHandle;
    //     crcHandle.Instance = CRC;
    //     crcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    //     crcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    //     crcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    //     crcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    //     crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    //     HAL_CRC_Init(&crcHandle); // Initialise CRC module

    //     if (bootloaderData.verificationMode == VERIFICATION_APP_INFO || bootloaderData.verificationMode == VERIFICATION_FULL) {
    //         // Verify app info
    //         if (HAL_CRC_Calculate(&crcHandle, (uint32_t *) &bootloaderData.app1_info, sizeof(bootloaderData.app1_info)/4) != bootloaderData.app1_infoChecksum) {
    //             app1Exclusion = 1;
    //         }
    //         if (HAL_CRC_Calculate(&crcHandle, (uint32_t *) &bootloaderData.app2_info, sizeof(bootloaderData.app2_info)/4) != bootloaderData.app2_infoChecksum) {
    //             app2Exclusion = 1;
    //         }
    //     }

    //     if (bootloaderData.verificationMode == VERIFICATION_VECTOR_TABLE) {
    //         // Verify app vector table
    //         if (HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP1_START, VECTOR_TABLE_SIZE) != bootloaderData.app1_info.vectblChecksum) {
    //             app1Exclusion = 1;
    //         }
    //         if (HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP2_START, VECTOR_TABLE_SIZE) != bootloaderData.app2_info.vectblChecksum) {
    //             app2Exclusion = 1;
    //         }
    //     }

    //     if (bootloaderData.verificationMode == VERIFICATION_APPLICATION || bootloaderData.verificationMode == VERIFICATION_FULL) {
    //         // Verify application
    //         if (bootloaderData.app1_info.size % 4) { // If application size is not word aligned then CRC has to be calculated for word aligned part and remaining bytes
    //             HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP1_START, bootloaderData.app1_info.size/4);
    //             crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    //             if (HAL_CRC_Accumulate(&crcHandle, (uint32_t *)(&__FLASH_APP1_START + bootloaderData.app1_info.size - (bootloaderData.app1_info.size % 4)), bootloaderData.app1_info.size % 4) != bootloaderData.app1_info.appChecksum) {
    //                 app1Exclusion = 1;
    //             }
    //             crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    //         } else {
    //             if (HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP1_START, bootloaderData.app1_info.size/4) != bootloaderData.app1_info.appChecksum) {
    //                 app1Exclusion = 1;
    //             }
    //         }
    //         if (bootloaderData.app2_info.size % 4) { // If application size is not word aligned then CRC has to be calculated for word aligned part and remaining bytes
    //             HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP2_START, bootloaderData.app2_info.size/4);
    //             crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    //             if (HAL_CRC_Accumulate(&crcHandle, (uint32_t *)(&__FLASH_APP2_START + bootloaderData.app2_info.size - (bootloaderData.app2_info.size % 4)), bootloaderData.app2_info.size % 4) != bootloaderData.app2_info.appChecksum) {
    //                 app2Exclusion = 1;
    //             }
    //             crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    //         } else {
    //             if (HAL_CRC_Calculate(&crcHandle, (uint32_t *)&__FLASH_APP2_START, bootloaderData.app2_info.size/4) != bootloaderData.app2_info.appChecksum) {
    //                 app2Exclusion = 1;
    //             }
    //         }
    //     }

    //     HAL_CRC_DeInit(&crcHandle); // De-initialise CRC module
    //     __HAL_RCC_CRC_CLK_DISABLE(); // Disable CRC module clock
    // }

    // Select application
    uint8_t appSelection;

    if (app1Exclusion && app2Exclusion) {
        appSelection = 0; // If both applications are excluded then no application is selected
    } else if (app1Exclusion) {
        appSelection = 2; // If app 1 is excluded choose app 2
    } else if (app2Exclusion) {
        appSelection = 1; // If app 2 is excluded choose app 1
    } else { // If neither app is excluded, choose based on priority
        if (bootloaderData.bootPriority == BOOTPRIO_APP1) {
            appSelection = 1; // Choose app 1
        } else if (bootloaderData.bootPriority == BOOTPRIO_APP2) {
            appSelection = 2; // Choose app 2
        } else if (bootloaderData.bootPriority == BOOTPRIO_AUTOMATIC) {
            // Select based on ID and version
            if (bootloaderData.app1_info.ID == bootloaderData.app2_info.ID) {
                // If IDs are the same, select the app with the highest version number
                if (bootloaderData.app1_info.version >= bootloaderData.app2_info.version) {
                    appSelection = 1;
                } else {
                    appSelection = 2;
                }
            } else {
                // If IDs are different default to app 1
                appSelection = 1;
            }
        } else {
            // If bootloader boot priority is invalid then no application is selected
            appSelection = 0;
        }
    }

    // If an error condition was detected earlier, increment the fault count of the selected application
    if (errorCondition) {
        if (appSelection == 1) {
            bootloaderData.app1_faultCount += 1;
            writeBootloaderData(bootloaderData);
        } else if (appSelection == 2) {
            bootloaderData.app2_faultCount += 1;
            writeBootloaderData(bootloaderData);
        }
    }

    // Enable watchdog if appropriate
    configureWatchdog(bootloaderData.watchdogMode);

    // Load selected application
    uint32_t appAddr;
    if (appSelection == 1) {
        appAddr = (uint32_t) &__FLASH_APP1_START;
        uint32_t *appSpace = (uint32_t *) appAddr;
        uint32_t appStackPointer = appSpace[0]; // Get application stack pointer
        uint32_t appStartup = appSpace[1]; // Get application startup code pointer
        SCB->VTOR = appAddr; // Set VTOR
        startApplication(appStackPointer, appStartup); // Start application
    } else if (appSelection == 2) {
        appAddr = (uint32_t) &__FLASH_APP2_START;
        uint32_t *appSpace = (uint32_t *) appAddr;
        uint32_t appStackPointer = appSpace[0]; // Get application stack pointer
        uint32_t appStartup = appSpace[1]; // Get application startup code pointer
        SCB->VTOR = appAddr; // Set VTOR
        startApplication(appStackPointer, appStartup); // Start application
    } else {
        while (1) {}; // Stall if no app is selected
    }

    while (1) {}; // Loop forever (not reached)
}