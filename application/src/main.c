/*
STM32G0 Bootloader
Jonah Swain

Application main (implementation)
Main application code
*/

/* DEPENDENCIES */
#include "main.h"

/* CONSTANT DEFINITIONS AND MACROS */


/* GLOBAL VARIABLES */


/* FUNCTIONS */

#ifdef TEST_BLINK
// Blink LED
// Tests whether the application is correctly loaded
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_SWITCH_AS1
// Switch boot priority to AS1
// Tests whether the boot priority can be updated and whether 2 applicaions can be loaded
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    bootloader->setBootPriority(BOOTPRIO_APP1); // Set the boot priority to application space 1

    while (1) { // Main loop (loop forever)

    }
}
#endif

#ifdef TEST_SWITCH_AS2
// Switch boot priority to AS2
// Tests whether the boot priority can be updated and whether 2 applicaions can be loaded
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    bootloader->setBootPriority(BOOTPRIO_APP2); // Set the boot priority to application space 2

    while (1) { // Main loop (loop forever)

    }
}
#endif

#ifdef TEST_BLINK_SWITCH_AS1
// Blink LED and switch the boot priority to AS1
// Tests whether the boot priority can be updated and whether 2 applicaions can be loaded
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    bootloader->setBootPriority(BOOTPRIO_APP1); // Set the boot priority to application space 1

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_BLINK_SWITCH_AS2
// Blink LED and switch the boot priority to AS2
// Tests whether the boot priority can be updated and whether 2 applicaions can be loaded
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    bootloader->setBootPriority(BOOTPRIO_APP2); // Set the boot priority to application space 2

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_IAP_AS1
// Installs a blink application to AS1 and sets the boot priority to AS1
// Tests in application programming
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app1_erase() == BL_OK) { // Erase application space 1
        if (bootloader->app1_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 1
            if (bootloader->app1_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_APP1); // Update boot priority
            }
        }
    }

    while (1) { // Main loop (loop forever)

    }
}
#endif

#ifdef TEST_IAP_AS2
// Installs a blink application to AS2 and sets the boot priority to AS2
// Tests in application programming
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app2_erase() == BL_OK) { // Erase application space 2        
        if (bootloader->app2_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 2
            if (bootloader->app2_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_APP2); // Update boot priority
            }
        }
    }

    while (1) { // Main loop (loop forever)

    }
}
#endif

#ifdef TEST_IAP_AS2_PRIO_AUTO
// Installs a blink application to AS2 and sets the boot priority to automatic
// Tests in application programming and bootloader automatic application selection
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app2_erase() == BL_OK) { // Erase application space 2        
        if (bootloader->app2_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 2
            if (bootloader->app2_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_AUTOMATIC); // Update boot priority
            }
        }
    }

    while (1) { // Main loop (loop forever)

    }
}
#endif

#ifdef TEST_VT_CHECKSUM_VALID
// Installs a blink application with a valid/correct checksum to AS2 and sets the boot priority to AS2, enables vector table checksum verification, blinks fast if loaded
// Tests if vector table checksum verification works correctly for a valid checksum
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app2_erase() == BL_OK) { // Erase application space 2        
        if (bootloader->app2_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 2
            if (bootloader->app2_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_APP2); // Update boot priority
            }
        }
    }

    // Enable vector table checksum verification
    bootloader->setVerificationMode(VERIFICATION_VECTOR_TABLE);

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_VT_CHECKSUM_INVALID
// Installs a blink application with an invalid/incorrect checksum to AS2 and sets the boot priority to AS2, enables vector table checksum verification, blinks fast if loaded
// Tests if vector table checksum verification works correctly for an invalid checksum
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app2_erase() == BL_OK) { // Erase application space 2        
        if (bootloader->app2_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 2
            if (bootloader->app2_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_APP2); // Update boot priority
            }
        }
    }

    // Enable vector table checksum verification
    bootloader->setVerificationMode(VERIFICATION_VECTOR_TABLE);

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_APP_CHECKSUM_VALID
// Installs a blink application with a valid/correct checksum to AS2 and sets the boot priority to AS2, enables application checksum verification, blinks fast if loaded
// Tests if application checksum verification works correctly for a valid checksum
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app2_erase() == BL_OK) { // Erase application space 2        
        if (bootloader->app2_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 2
            if (bootloader->app2_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_APP2); // Update boot priority
            }
        }
    }

    // Enable vector table checksum verification
    bootloader->setVerificationMode(VERIFICATION_APPLICATION);

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_APP_CHECKSUM_INVALID
// Installs a blink application with an invalid/incorrect checksum to AS2 and sets the boot priority to AS2, enables application checksum verification, blinks fast if loaded
// Tests if application checksum verification works correctly for an invalid checksum
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Install new application
    bootloader->enableProgrammingMode(); // Enable programming mode
    if (bootloader->app2_erase() == BL_OK) { // Erase application space 2        
        if (bootloader->app2_write(0x00000000, (uint64_t *)APP_BINARY, APP_BINARY_SIZE/8) == BL_OK) { // Program dword aligned data to application space 2
            if (bootloader->app2_writeInfo(APP_INFO) == BL_OK) { // Write application info
                bootloader->disableProgrammingMode(); // Disable programming mode
                bootloader->setBootPriority(BOOTPRIO_APP2); // Update boot priority
            }
        }
    }

    // Enable vector table checksum verification
    bootloader->setVerificationMode(VERIFICATION_APPLICATION);

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_WATCHDOG
// Enables the watchdog and sets the boot priority to AS2 if watchdog disabled. Disables the watchdog, resets the AS2 fault count, and sets boot priority to auto if watchdog enabled. Blinks LED.
// Tests if watchdog can be enabled and disabled, if fault count can be reset.
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    if (bootloader->getWatchdogMode() == WATCHDOG_OFF) {
        bootloader->setWatchdogMode(WATCHDOG_MEDIUM); // Set watchdog to medium delay (~5s default)
        bootloader->setBootPriority(BOOTPRIO_APP2); // Set boot priority to AS2
    } else {
        bootloader->setWatchdogMode(WATCHDOG_OFF); // Disable watchdog
        bootloader->app2_resetFaultCount(); // Reset AS2 fault count
        bootloader->setBootPriority(BOOTPRIO_AUTOMATIC); // Set boot priority
    }

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_WATCHDOG_RESET
// Blinks LED and resets watchdog periodically
// Tests if watchdog can be reset
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    struct BootloaderFunctions *bootloader = _BOOTLOADER_FUNCTIONS; // Get pointer to bootloader functions

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        bootloader->resetWatchdog(); // Reset watchdog
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif

#ifdef TEST_WATCHDOG_NORESET
// Blinks LED
// Tests if watchdog works correctly and resets µC after timeout
void main() {
    HAL_Init(); // Initialise HAL (reset peripherals, initialise flash and systick)
    SystemClock_Config(); // Configure system clock

    // Initialise and configure onboard LED (LD4/PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LD4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LD4_Port, &GPIO_InitStruct);

    while (1) { // Main loop (loop forever)
        HAL_GPIO_TogglePin(LD4_Port, LD4_Pin); // Toggle LED
        HAL_Delay(BLINK_DELAY); // Delay
    }
}
#endif




void SystemClock_Config(void) { // Configures the system clock (Auto-generated by STM32CubeIDE)
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 8;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler() { // Loop forever if error encountered
    while (1) {}
}