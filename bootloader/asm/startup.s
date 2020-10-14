/*
STM32G0 Bootloader
Jonah Swain [SWNJON003]

Bootloader vector table and startup code

Adapted in part from the startup file auto-generated by STM32CubeIDE for the STM32G071RB
ST's startup file is licensed by ST under the BSD 3-clause license (opensource.org/licenses/BSD-3-Clause)
*/

/* Initial assembler directives */
.syntax unified @ use ARM/Thumb unified syntax
.cpu cortex-m0
.fpu softvfp
.thumb

.global g_pfnVectors @ make vector table globally accessible
.global Default_Handler @ make default interrupt handler globally accessible

/* Global symbols from linker script */
.word __STACK_END           @ End (start) of stack
.word __BL_TEXT_END         @ End of .text (code) section
.word __BL_RODATA_END       @ End of .rodata (read-only data) section
.word __BL_DATA_ADDR        @ Address of initialisation values for .data (data/variables) section
.word __BL_DATA_START       @ Start of .data (initialised data/variables) section
.word __BL_DATA_END         @ End of .data (initialised data/variables) section
.word __BL_BSS_START        @ Start of .bss (uninitialised data/variables) section
.word __BL_BSS_END          @ End of .bss (uninitialised data/variables) section

/* Startup code (Reset_Handler) */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    @ initialise stack pointer
    ldr r0, =__STACK_END
    mov sp, r0
CopyData:
    @ initialise .data SRAM from flash
    ldr r4, =__BL_DATA_ADDR
    ldr r2, =__BL_DATA_START
    ldr r3, =__BL_DATA_END
    cmp r2, r3
    beq ZeroBSS
    movs r1, #0
LoopCopyData:
    ldr r0, [r4, r1]
    str r0, [r2, r1]
    adds r1, r1, #4
    adds r0, r2, r1
    cmp r0, r3
    bcc LoopCopyData
    movs r4, #0
ZeroBSS:
    @ initialise .bss SRAM to zero
    ldr r2, =__BL_BSS_START
    ldr r3, =__BL_BSS_END
    cmp r2, r3
    beq InitFunctions
    movs r0, #0
LoopZeroBSS:
    str r0, [r2]
    adds r2, r2, #4
    cmp r2, r3
    bcc LoopZeroBSS
    @ clear registers used above
    movs r1, #0
    movs r2, #0
    movs r3, #0
InitFunctions:
    bl SystemInit @ sets/resets vector table offset register (C function in system_stm32g0xx.c)
    @ bl __libc_init_array @ calls C++ static constructors and initialisation code (NOT REQUIRED FOR C PROJECTS)
    bl main @ branches to C code and bootloader entry point
LoopForever:
    @ loops infinitely if bootloader main() returns
    b LoopForever

.size Reset_Handler, .-Reset_Handler

/* Default interrupt handler (infinite loop) */
.section .text.Default_Handler,"ax",%progbits
.weak Default_Handler
Default_Handler:
    b LoopForever

.size Default_Handler, .-Default_Handler

/* Vector table */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors
g_pfnVectors:
    .word  __STACK_END
    .word  Reset_Handler
    .word  NMI_Handler
    .word  HardFault_Handler
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  0
    .word  SVC_Handler
    .word  0
    .word  0
    .word  PendSV_Handler
    .word  SysTick_Handler
    .word  WWDG_IRQHandler
    .word  PVD_IRQHandler
    .word  RTC_TAMP_IRQHandler
    .word  FLASH_IRQHandler
    .word  RCC_IRQHandler
    .word  EXTI0_1_IRQHandler
    .word  EXTI2_3_IRQHandler
    .word  EXTI4_15_IRQHandler
    .word  UCPD1_2_IRQHandler
    .word  DMA1_Channel1_IRQHandler
    .word  DMA1_Channel2_3_IRQHandler
    .word  DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler
    .word  ADC1_COMP_IRQHandler
    .word  TIM1_BRK_UP_TRG_COM_IRQHandler
    .word  TIM1_CC_IRQHandler
    .word  TIM2_IRQHandler
    .word  TIM3_IRQHandler
    .word  TIM6_DAC_LPTIM1_IRQHandler
    .word  TIM7_LPTIM2_IRQHandler
    .word  TIM14_IRQHandler
    .word  TIM15_IRQHandler
    .word  TIM16_IRQHandler
    .word  TIM17_IRQHandler
    .word  I2C1_IRQHandler
    .word  I2C2_IRQHandler
    .word  SPI1_IRQHandler
    .word  SPI2_IRQHandler
    .word  USART1_IRQHandler
    .word  USART2_IRQHandler
    .word  USART3_4_LPUART1_IRQHandler
    .word  CEC_IRQHandler

/* Weak aliases from interrupt handlers to Default_Handler (overridden by function with the same name) */
.weak      NMI_Handler
.thumb_set NMI_Handler,Default_Handler

.weak      HardFault_Handler
.thumb_set HardFault_Handler,Default_Handler

.weak      SVC_Handler
.thumb_set SVC_Handler,Default_Handler

.weak      PendSV_Handler
.thumb_set PendSV_Handler,Default_Handler

.weak      SysTick_Handler
.thumb_set SysTick_Handler,Default_Handler

.weak      WWDG_IRQHandler
.thumb_set WWDG_IRQHandler,Default_Handler

.weak      PVD_IRQHandler
.thumb_set PVD_IRQHandler,Default_Handler

.weak      RTC_TAMP_IRQHandler
.thumb_set RTC_TAMP_IRQHandler,Default_Handler

.weak      FLASH_IRQHandler
.thumb_set FLASH_IRQHandler,Default_Handler

.weak      RCC_IRQHandler
.thumb_set RCC_IRQHandler,Default_Handler

.weak      EXTI0_1_IRQHandler
.thumb_set EXTI0_1_IRQHandler,Default_Handler

.weak      EXTI2_3_IRQHandler
.thumb_set EXTI2_3_IRQHandler,Default_Handler

.weak      EXTI4_15_IRQHandler
.thumb_set EXTI4_15_IRQHandler,Default_Handler

.weak      UCPD1_2_IRQHandler
.thumb_set UCPD1_2_IRQHandler,Default_Handler

.weak      DMA1_Channel1_IRQHandler
.thumb_set DMA1_Channel1_IRQHandler,Default_Handler

.weak      DMA1_Channel2_3_IRQHandler
.thumb_set DMA1_Channel2_3_IRQHandler,Default_Handler

.weak      DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler
.thumb_set DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler,Default_Handler

.weak      ADC1_COMP_IRQHandler
.thumb_set ADC1_COMP_IRQHandler,Default_Handler

.weak      TIM1_BRK_UP_TRG_COM_IRQHandler
.thumb_set TIM1_BRK_UP_TRG_COM_IRQHandler,Default_Handler

.weak      TIM1_CC_IRQHandler
.thumb_set TIM1_CC_IRQHandler,Default_Handler

.weak      TIM2_IRQHandler
.thumb_set TIM2_IRQHandler,Default_Handler

.weak      TIM3_IRQHandler
.thumb_set TIM3_IRQHandler,Default_Handler

.weak      TIM6_DAC_LPTIM1_IRQHandler
.thumb_set TIM6_DAC_LPTIM1_IRQHandler,Default_Handler

.weak      TIM7_LPTIM2_IRQHandler
.thumb_set TIM7_LPTIM2_IRQHandler,Default_Handler

.weak      TIM14_IRQHandler
.thumb_set TIM14_IRQHandler,Default_Handler

.weak      TIM15_IRQHandler
.thumb_set TIM15_IRQHandler,Default_Handler

.weak      TIM16_IRQHandler
.thumb_set TIM16_IRQHandler,Default_Handler

.weak      TIM17_IRQHandler
.thumb_set TIM17_IRQHandler,Default_Handler

.weak      I2C1_IRQHandler
.thumb_set I2C1_IRQHandler,Default_Handler

.weak      I2C2_IRQHandler
.thumb_set I2C2_IRQHandler,Default_Handler

.weak      SPI1_IRQHandler
.thumb_set SPI1_IRQHandler,Default_Handler

.weak      SPI2_IRQHandler
.thumb_set SPI2_IRQHandler,Default_Handler

.weak      USART1_IRQHandler
.thumb_set USART1_IRQHandler,Default_Handler

.weak      USART2_IRQHandler
.thumb_set USART2_IRQHandler,Default_Handler

.weak      USART3_4_LPUART1_IRQHandler
.thumb_set USART3_4_LPUART1_IRQHandler,Default_Handler

.weak      CEC_IRQHandler
.thumb_set CEC_IRQHandler,Default_Handler