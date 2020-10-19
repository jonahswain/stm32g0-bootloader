/*
STM32G0 Bootloader
Jonah Swain

Bootloader functions (implementation)
Bootloader and update functions
*/

/* DEPENDENCIES */
#include "bootloader.h"

/* CONSTANT DEFINITIONS AND MACROS */


/* GLOBAL VARIABLES */
struct BootloaderFunctions dispatchTable __attribute__((section(".dispatch_table"))) = { // Dispatch table for application accessible bootloader functions
    getBootloaderVersion,
    getBootPriority,
    setBootPriority,
    getVerificationMode,
    setVerificationMode,
    getWatchdogMode,
    setWatchdogMode,
    resetWatchdog,
    enableProgrammingMode,
    disableProgrammingMode,
    app1_getFaultCount,
    app1_resetFaultCount,
    app1_getInfo,
    app1_erase,
    app1_write,
    app1_writeInfo,
    app2_getFaultCount,
    app2_resetFaultCount,
    app2_getInfo,
    app2_erase,
    app2_write,
    app2_writeInfo
};

/* FUNCTIONS */

BootloaderData_T getBootloaderData(){ // Get bootloader data from flash
    BootloaderData_T *bootloaderDataFlash = (BootloaderData_T *)&__FLASH_BL_DATA_START; // Pointer to bootloader data in flash
    BootloaderData_T bootloaderData = *bootloaderDataFlash; // Copy bootloader data to RAM
    return bootloaderData; // Return RAM copy of bootloader data
}

BootloaderStatus_T writeBootloaderData(BootloaderData_T data){ // Write bootloader data to flash
    uint32_t bootloaderDataAddress = (uint32_t) &__FLASH_BL_DATA_START; // Get base address of bootloader data

    HAL_FLASH_Unlock(); // Unlock flash control
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); // Clear flash flags

    // Flash erase procedure
    uint32_t flashPage = (bootloaderDataAddress - FLASH_BASE)/FLASH_PAGE_SIZE; // Calculate the flash page number
    uint32_t pageError; // Page error code (for HAL)
    // Flash erase parameters
    FLASH_EraseInitTypeDef flashErase = {0};
    flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
    flashErase.Page = flashPage;
    flashErase.NbPages = 1;
    if (HAL_FLASHEx_Erase(&flashErase, &pageError) != HAL_OK) { // Erase flash page
        HAL_FLASH_Lock();
        return BL_ERROR_HAL; // Return error if erase fails
    }

    // Flash write procedure
    uint64_t datachunk; // Data double-word to write to flash
    for (uint32_t dw = 0; dw < sizeof(BootloaderData_T); dw += 8) { // Iterate through bootloader data in double-words
        if (sizeof(BootloaderData_T) - dw >= 8){
            datachunk = *((uint64_t*)((uint32_t)&data + dw)); // Get data double word
        } else {
            datachunk = *((uint64_t*)((uint32_t)&data + dw)) | ((uint64_t)0xFFFFFFFFFFFFFFFF >> (sizeof(BootloaderData_T) - dw)*8); // Get data double word and mask unused bytes
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, bootloaderDataAddress + dw, datachunk) != HAL_OK) { // Write double word to flash
            HAL_FLASH_Lock();
            return BL_ERROR_HAL; // Return error if write fails
        }
    }

    HAL_FLASH_Lock(); // Lock flash control

    // Data verification procedure
    uint32_t wdata;
    uint32_t rdata;
    for (uint32_t wd = 0; wd < sizeof(BootloaderData_T); wd += 4) { // Iterate through bootloader data in words
        wdata = *((uint32_t*)((uint32_t)&data + wd)); // Get bootloader data word
        rdata = *((uint32_t*)(bootloaderDataAddress + wd)); // Get word from flash
        if (sizeof(BootloaderData_T) - wd < 4) { // Mask unused data bytes
            wdata |= ((uint32_t)0xFFFFFFFF >> (sizeof(BootloaderData_T) - wd)*8);
            rdata |= ((uint32_t)0xFFFFFFFF >> (sizeof(BootloaderData_T) - wd)*8);
        }
        if (rdata != wdata) {
            return BL_ERROR_WRITE_VERIFICATION;
        }
    }

    return BL_OK;
}


__attribute__((naked)) void startApplication(uint32_t stackPointer, uint32_t startupAddress){ // Starts an application (sets the main stack pointer to stackPointer and jumps to startupAddress)
    __ASM("msr msp, r0"); // Set stack pointer to application stack pointer
    __ASM("bx r1"); // Branch to application startup code
}


BootloaderStatus_T configureWatchdog(WatchdogMode_T mode){ // Configure the watchdog
    IWDG_HandleTypeDef iwdgHandle;
    iwdgHandle.Instance = IWDG;
    iwdgHandle.Init.Window = IWDG_WINDOW_DISABLE;

    if (mode != WATCHDOG_OFF) {
        if (mode == WATCHDOG_LONG) {
            iwdgHandle.Init.Prescaler = WDG_LONG_PRESC;
            iwdgHandle.Init.Reload = WDG_LONG_RELOAD;
        } else if (mode == WATCHDOG_MEDIUM) {
            iwdgHandle.Init.Prescaler = WDG_MED_PRESC;
            iwdgHandle.Init.Reload = WDG_MED_RELOAD;
        } else if (mode == WATCHDOG_SHORT) {
            iwdgHandle.Init.Prescaler = WDG_SHORT_PRESC;
            iwdgHandle.Init.Reload = WDG_SHORT_RELOAD;
        }

        if (HAL_IWDG_Init(&iwdgHandle) != HAL_OK) { // Initialise watchdog
            return BL_ERROR_HAL;
        }
    }
    return BL_OK;
}

BootloaderStatus_T resetWatchdog(){ // Reset the watchdog
    IWDG_HandleTypeDef iwdgHandle;
    iwdgHandle.Instance = IWDG;

    if (HAL_IWDG_Refresh(&iwdgHandle) != HAL_OK) { // Reset watchdog
        return BL_ERROR_HAL;
    }

    return BL_OK;
}


uint32_t getBootloaderVersion(){ // Get the bootloader version number
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.blVersion;
}

BootPriority_T getBootPriority(){ // Get the current boot priority
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.bootPriority;
}

BootloaderStatus_T setBootPriority(BootPriority_T priority){ // Set the boot priority
    BootloaderData_T bootloaderData = getBootloaderData();
    bootloaderData.bootPriority = priority;
    return writeBootloaderData(bootloaderData);
}

VerificationMode_T getVerificationMode(){ // Get the current application verification mode
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.verificationMode;
}

BootloaderStatus_T setVerificationMode(VerificationMode_T mode){ // Set the application verification mode
    BootloaderData_T bootloaderData = getBootloaderData();
    bootloaderData.verificationMode = mode;
    return writeBootloaderData(bootloaderData);
}

WatchdogMode_T getWatchdogMode(){ // Get the current watchdog mode
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.watchdogMode;
}

BootloaderStatus_T setWatchdogMode(WatchdogMode_T mode){ // Set the watchdog mode
    BootloaderData_T bootloaderData = getBootloaderData();
    bootloaderData.verificationMode = mode;
    BootloaderStatus_T status;
    status = writeBootloaderData(bootloaderData);
    if (status != BL_OK) {return status;}
    return configureWatchdog(mode);
}



BootloaderStatus_T enableProgrammingMode(){ // Enable programming mode (to write new application)
    BootloaderData_T bootloaderData = getBootloaderData(); // Get bootloader data
    if (bootloaderData.watchdogMode != WATCHDOG_OFF) { // If watchdog is enabled, set long interval and reset before programming
        configureWatchdog(WATCHDOG_LONG);
        resetWatchdog();
    }

    HAL_FLASH_Unlock(); // Unlock flash for programming
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); // Clear flash errors

    return BL_OK;
}

BootloaderStatus_T disableProgrammingMode(){ // Disable programming mode (after writing application)
    HAL_FLASH_Lock(); // Lock flash
    BootloaderData_T bootloaderData = getBootloaderData();
    configureWatchdog(bootloaderData.watchdogMode); // Reconfigure watchdog

    return BL_OK;
}



uint8_t app1_getFaultCount(){ // Get the fault count of application 1
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.app1_faultCount;
}

BootloaderStatus_T app1_resetFaultCount(){ // Reset the fault count of application 1
    BootloaderData_T bootloaderData = getBootloaderData();
    bootloaderData.app1_faultCount = 0;
    return writeBootloaderData(bootloaderData);
}

AppInfo_T app1_getInfo(){ // Get the app info of application 1
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.app1_info;
}

BootloaderStatus_T app1_erase(){ // Erase application space 1
    uint32_t pageError; // Page error code (for HAL)
    // Flash erase parameters
    FLASH_EraseInitTypeDef flashErase = {0};
    flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
    flashErase.Page = ((uint32_t) &__FLASH_APP1_START - FLASH_BASE)/FLASH_PAGE_SIZE;
    flashErase.NbPages = ((uint32_t) &__FLASH_APP1_LEN)/FLASH_PAGE_SIZE;
    if (HAL_FLASHEx_Erase(&flashErase, &pageError) != HAL_OK) { // Erase flash pages
        return BL_ERROR_HAL; // Return HAL error if erase fails
    }
    return BL_OK;
}

BootloaderStatus_T app1_write(uint32_t address, uint64_t *data, uint32_t length){ // Write data to application space 1 (in 64-bit/double-word pages)
    if (address % 8) {return BL_ERROR_DATA_ALIGNMENT;} // Check for correct data alignment (double-word aligned)
    if ((address + 8*length) > (uint32_t) &__FLASH_APP1_LEN) {return BL_ERROR_OUT_OF_RANGE;} // Check that write lies within application space

    for (uint32_t i = 0; i < length; i++) { // Iterate through data
        uint64_t ddw = data[i];
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ((uint32_t) &__FLASH_APP1_START + address + 8*i), ddw) != HAL_OK) { // Write data double-word to flash
            return BL_ERROR_HAL; // Return HAL error if write fails
        }
    }

    // Verify written data
    uint64_t flashData;
    uint64_t correctData;
    for (uint32_t i = 0; i < length; i++) {
        flashData = *((uint64_t*)((uint32_t) &__FLASH_APP1_START + address + 8*i));
        correctData = data[i];
        if (flashData != correctData){
            return BL_ERROR_WRITE_VERIFICATION;
        }
    }
    return BL_OK;
}

BootloaderStatus_T app1_writeInfo(AppInfo_T info){ // Write app 1 info to bootloader data

    __HAL_RCC_CRC_CLK_ENABLE(); // Enable CRC module clock
    // Configure CRC handle
    CRC_HandleTypeDef crcHandle;
    crcHandle.Instance = CRC;
    crcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    crcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    crcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    crcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    HAL_CRC_Init(&crcHandle); // Initialise CRC module

    uint32_t appInfoChecksum = HAL_CRC_Calculate(&crcHandle, (uint32_t *) &info, sizeof(info)/4);

    HAL_CRC_DeInit(&crcHandle); // De-initialise CRC module
    __HAL_RCC_CRC_CLK_DISABLE(); // Disable CRC module clock


    BootloaderData_T bootloaderData = getBootloaderData(); // Fetch existing bootloader data
    bootloaderData.app1_info = info; // Replace application 1 info with new info
    bootloaderData.app1_faultCount = 0; // Reset application 1 fault count
    bootloaderData.app1_infoChecksum = appInfoChecksum; // Set application 1 info checksum

    uint32_t bootloaderDataAddress = (uint32_t) &__FLASH_BL_DATA_START; // Get base address of bootloader data

    // Flash erase procedure
    uint32_t flashPage = (bootloaderDataAddress - FLASH_BASE)/FLASH_PAGE_SIZE; // Calculate the flash page number
    uint32_t pageError; // Page error code (for HAL)
    // Flash erase parameters
    FLASH_EraseInitTypeDef flashErase = {0};
    flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
    flashErase.Page = flashPage;
    flashErase.NbPages = 1;
    if (HAL_FLASHEx_Erase(&flashErase, &pageError) != HAL_OK) { // Erase flash page
        HAL_FLASH_Lock();
        return BL_ERROR_HAL; // Return error if erase fails
    }

    // Flash write procedure
    uint64_t datachunk; // Data double-word to write to flash
    for (uint32_t dw = 0; dw < sizeof(BootloaderData_T); dw += 8) { // Iterate through bootloader data in double-words
        if (sizeof(BootloaderData_T) - dw >= 8){
            datachunk = *((uint64_t*)((uint32_t)&bootloaderData + dw)); // Get data double word
        } else {
            datachunk = *((uint64_t*)((uint32_t)&bootloaderData + dw)) | ((uint64_t)0xFFFFFFFFFFFFFFFF >> (sizeof(BootloaderData_T) - dw)*8); // Get data double word and mask unused bytes
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, bootloaderDataAddress + dw, datachunk) != HAL_OK) { // Write double word to flash
            HAL_FLASH_Lock();
            return BL_ERROR_HAL; // Return error if write fails
        }
    }

    // Data verification procedure
    uint32_t wdata;
    uint32_t rdata;
    for (uint32_t wd = 0; wd < sizeof(BootloaderData_T); wd += 4) { // Iterate through bootloader data in words
        wdata = *((uint32_t*)((uint32_t)&bootloaderData + wd)); // Get bootloader data word
        rdata = *((uint32_t*)(bootloaderDataAddress + wd)); // Get word from flash
        if (sizeof(BootloaderData_T) - wd < 4) { // Mask unused data bytes
            wdata |= ((uint32_t)0xFFFFFFFF >> (sizeof(BootloaderData_T) - wd)*8);
            rdata |= ((uint32_t)0xFFFFFFFF >> (sizeof(BootloaderData_T) - wd)*8);
        }
        if (rdata != wdata) {
            return BL_ERROR_WRITE_VERIFICATION;
        }
    }

    return BL_OK;
}



uint8_t app2_getFaultCount(){ // Get the fault count of application 2
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.app2_faultCount;
}

BootloaderStatus_T app2_resetFaultCount(){ // Reset the fault count of application 2
    BootloaderData_T bootloaderData = getBootloaderData();
    bootloaderData.app2_faultCount = 0;
    return writeBootloaderData(bootloaderData);
}

AppInfo_T app2_getInfo(){ // Get the app info of application 2
    BootloaderData_T bootloaderData = getBootloaderData();
    return bootloaderData.app2_info;
}

BootloaderStatus_T app2_erase(){ // Erase application space 2
    uint32_t pageError; // Page error code (for HAL)
    // Flash erase parameters
    FLASH_EraseInitTypeDef flashErase = {0};
    flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
    flashErase.Page = ((uint32_t) &__FLASH_APP2_START - FLASH_BASE)/FLASH_PAGE_SIZE;
    flashErase.NbPages = ((uint32_t) &__FLASH_APP2_LEN)/FLASH_PAGE_SIZE;
    if (HAL_FLASHEx_Erase(&flashErase, &pageError) != HAL_OK) { // Erase flash pages
        return BL_ERROR_HAL; // Return HAL error if erase fails
    }
    return BL_OK;
}

BootloaderStatus_T app2_write(uint32_t address, uint64_t *data, uint32_t length){ // Write data to application space 2 (in 64-bit/double-word pages)
    if (address % 8) {return BL_ERROR_DATA_ALIGNMENT;} // Check for correct data alignment (double-word aligned)
    if ((address + 8*length) > (uint32_t) &__FLASH_APP2_LEN) {return BL_ERROR_OUT_OF_RANGE;} // Check that write lies within application space

    for (uint32_t i = 0; i < length; i++) { // Iterate through data
        uint64_t ddw = data[i];
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ((uint32_t) &__FLASH_APP2_START + address + 8*i), ddw) != HAL_OK) { // Write data double-word to flash
            return BL_ERROR_HAL; // Return HAL error if write fails
        }
    }

    // Verify written data
    uint64_t flashData;
    uint64_t correctData;
    for (uint32_t i = 0; i < length; i++) {
        flashData = *((uint64_t*)((uint32_t) &__FLASH_APP2_START + address + 8*i));
        correctData = data[i];
        if (flashData != correctData){
            return BL_ERROR_WRITE_VERIFICATION;
        }
    }
    return BL_OK;
}

BootloaderStatus_T app2_writeInfo(AppInfo_T info){ // Write app 2 info to bootloader data

    __HAL_RCC_CRC_CLK_ENABLE(); // Enable CRC module clock
    // Configure CRC handle
    CRC_HandleTypeDef crcHandle;
    crcHandle.Instance = CRC;
    crcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    crcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    crcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
    crcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    crcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
    HAL_CRC_Init(&crcHandle); // Initialise CRC module

    uint32_t appInfoChecksum = HAL_CRC_Calculate(&crcHandle, (uint32_t *) &info, sizeof(info)/4);

    HAL_CRC_DeInit(&crcHandle); // De-initialise CRC module
    __HAL_RCC_CRC_CLK_DISABLE(); // Disable CRC module clock

    BootloaderData_T bootloaderData = getBootloaderData(); // Fetch existing bootloader data
    bootloaderData.app2_info = info; // Replace application 1 info with new info
    bootloaderData.app2_faultCount = 0; // Reset application 1 fault count
    bootloaderData.app2_infoChecksum = appInfoChecksum; // Set application info checksum

    uint32_t bootloaderDataAddress = (uint32_t) &__FLASH_BL_DATA_START; // Get base address of bootloader data

    // Flash erase procedure
    uint32_t flashPage = (bootloaderDataAddress - FLASH_BASE)/FLASH_PAGE_SIZE; // Calculate the flash page number
    uint32_t pageError; // Page error code (for HAL)
    // Flash erase parameters
    FLASH_EraseInitTypeDef flashErase = {0};
    flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
    flashErase.Page = flashPage;
    flashErase.NbPages = 1;
    if (HAL_FLASHEx_Erase(&flashErase, &pageError) != HAL_OK) { // Erase flash page
        HAL_FLASH_Lock();
        return BL_ERROR_HAL; // Return error if erase fails
    }

    // Flash write procedure
    uint64_t datachunk; // Data double-word to write to flash
    for (uint32_t dw = 0; dw < sizeof(BootloaderData_T); dw += 8) { // Iterate through bootloader data in double-words
        if (sizeof(BootloaderData_T) - dw >= 8){
            datachunk = *((uint64_t*)((uint32_t)&bootloaderData + dw)); // Get data double word
        } else {
            datachunk = *((uint64_t*)((uint32_t)&bootloaderData + dw)) | ((uint64_t)0xFFFFFFFFFFFFFFFF >> (sizeof(BootloaderData_T) - dw)*8); // Get data double word and mask unused bytes
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, bootloaderDataAddress + dw, datachunk) != HAL_OK) { // Write double word to flash
            HAL_FLASH_Lock();
            return BL_ERROR_HAL; // Return error if write fails
        }
    }

    // Data verification procedure
    uint32_t wdata;
    uint32_t rdata;
    for (uint32_t wd = 0; wd < sizeof(BootloaderData_T); wd += 4) { // Iterate through bootloader data in words
        wdata = *((uint32_t*)((uint32_t)&bootloaderData + wd)); // Get bootloader data word
        rdata = *((uint32_t*)(bootloaderDataAddress + wd)); // Get word from flash
        if (sizeof(BootloaderData_T) - wd < 4) { // Mask unused data bytes
            wdata |= ((uint32_t)0xFFFFFFFF >> (sizeof(BootloaderData_T) - wd)*8);
            rdata |= ((uint32_t)0xFFFFFFFF >> (sizeof(BootloaderData_T) - wd)*8);
        }
        if (rdata != wdata) {
            return BL_ERROR_WRITE_VERIFICATION;
        }
    }

    return BL_OK;
}
