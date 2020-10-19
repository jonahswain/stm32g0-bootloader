/*
STM32G0 Bootloader
Jonah Swain [SWNJON003]

STM32G071RB memory map
*/

MEMORY
{
    FLASH_BL_CORE   (rx)    : ORIGIN = 0x08000000, LENGTH = 14K         /* Bootloader core (application loading stuff and libary functions) */
    FLASH_BL_DATA   (rx)    : ORIGIN = 0x08003800, LENGTH = 2K          /* Bootloader preferences (application info and shared function dispatch table) */
    FLASH_APP1      (rx)    : ORIGIN = 0x08004000, LENGTH = 56K         /* Application 1 code */
    FLASH_APP2      (rx)    : ORIGIN = 0x08012000, LENGTH = 56K         /* Application 2 code */
    SRAM            (rwx)   : ORIGIN = 0x20000000, LENGTH = 0x7F80      /* Data memory/RAM (32K - 128 bytes) */
    SRAM_BL_STATIC  (rwx)   : ORIGIN = 0x20007F80, LENGTH = 128         /* Data memory/RAM for bootloader static allocation/.data section (128 bytes/32 words) */
}

/* Export memory map variables (accessible in C) */
__FLASH_BL_CORE_START = ORIGIN(FLASH_BL_CORE);
__FLASH_BL_CORE_LEN = LENGTH(FLASH_BL_CORE);
__FLASH_BL_DATA_START = ORIGIN(FLASH_BL_DATA);
__FLASH_BL_DATA_LEN = LENGTH(FLASH_BL_DATA);
__FLASH_APP1_START = ORIGIN(FLASH_APP1);
__FLASH_APP1_LEN = LENGTH(FLASH_APP1);
__FLASH_APP2_START = ORIGIN(FLASH_APP2);
__FLASH_APP2_LEN = LENGTH(FLASH_APP2);
__SRAM_START = ORIGIN(SRAM);
__SRAM_LEN = LENGTH(SRAM);
__SRAM_BL_STATIC_START = ORIGIN(SRAM_BL_STATIC);
__SRAM_BL_STATIC_LEN = LENGTH(SRAM_BL_STATIC);