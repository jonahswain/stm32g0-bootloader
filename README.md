# STM32G0 Bootloader
Bootloader for OTA updates for STM32G0 microcontrollers.

The bootloader on this repository is configured for the STM32G071RB µC and tested on the NUCLEO-G071RB development board. It should be easily portable to other STM32G0 µCs, and other STM32 µCs with a vector table offset register (VTOR) (STM32 F1/F2/F3/F4/F7/L0/L1/L4/L4+/G0/G4/H7).

Prepared by Jonah Swain in partial fulfillment of the requirements for the EEE4022S final year engineering project at the University of Cape Town.

## Features
- [x] Dual configurable application spaces
- [x] Dual-boot
- [x] In application programming
- [x] Application checksum verification
- [x] Error handling and fail-over (independent watchdog)

## Project organisation
```
stm32g0-bootloader
│
├── application
│   ├── asm                     (application assembly sources - startup file)
│   ├── include                 (application header files, HAL configuration & some test application binaries as C headers)
│   ├── src                     (application source files and test code)
│   ├── appspace_1.ld           (linker script for application space 1)
│   ├── appspace_2.ld           (linker script for application space 2)
│
├── bootloader
│   ├── asm                     (bootloader assembly sources - startup file)
│   ├── include                 (bootloader header files & HAL configuration)
│   ├── src                     (bootloader source files)
│   ├── bootloader.ld           (bootloader linker script)
│
├── common
│   ├── app_info.h              (application info struct)
│   ├── bootloader_common.h     (application-accessible bootloader content)
│   ├── memory_map.h            (linker memory map symbols in C header)
│   ├── stm32g0xx_hal_conf.h    (HAL configuration header for libraries)
│
├── drivers
│   ├── CMSIS                   (CMSIS Cortex-M libraries)
│   ├── STM32G0xx_HAL_Driver    (STM32G0 HAL libraries)
│
├── make_update_header.py       (Python script to convert an update binary (.bin) into an array in a C header)
├── makefile                    (Project makefile to build the bootloader and application for both application spaces)
├── memory_map.ld               (Device memory map linker script)
```

## System requirements
In order to build and test this project, you will need the following installed on your computer:
- The `arm-none-eabi-` toolchain
- Make (install using [Cygwin](https://www.cygwin.com/) for Windows))
- A way of uploading code to your µC ([OpenOCD](http://openocd.org/), [STM32 ST-LINK utility](https://www.st.com/en/development-tools/stsw-link004.html), or [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html))
- Python 3 if you want to use the make_update_header.py script

## Porting to other µCs
The following considerations apply to porting this project to other µCs:

### STM32G0 series µCs
Change the memory map (`memory_map.ld`) to match your µC's flash and SRAM sizes.  
Change the DEVICE parameter in the makefile to match your µC type.

### Other STM32 families
Change the memory map (`memory_map.ld`) to match your µC's flash and SRAM sizes.  
Put the relevant CMSIS and HAL libraries in the drivers folder, and change the library config section in the makefile to locate the libraries.  
Replace the `stm32g0xx_hal_conf.h` and `system_stm32g0xx.c` files in the application and bootloader directories with those relevant to your µC.  
Change the `#include` statements in the header files to include your µC's HAL libraries.  
Change the DEVICE and CPU parameter in the makefile to match your µC type.  
Modify the `startup.s` files in the application and bootloader directories if required.  
You may have to change the functions that write to flash in `bootloader/src/bootloader.c` if your µC has different write alignment requirements.
