/*
STM32G0 Bootloader
Jonah Swain

Application info (header)
Application information struct
*/

/* INCLUDE GUARD */
#pragma once
#ifndef APP_INFO_H
#define APP_INFO_H

/* DEPENDENCIES */
#include <stdint.h>                 // Fixed width integer data types

/* CONSTANT DEFINITIONS AND MACROS */


/* TYPE DEFINITIONS AND ENUMERATIONS */

typedef struct { // Type definition for application information struct
    uint32_t ID;                    // Developer specified application ID (different versions of the same application should have the same ID) (0x00000000 and 0xFFFFFFFF are prohibited values)
    uint32_t version;               // Developer specified application version
    uint32_t size;                  // Size of the application (bytes)
    uint32_t vectblChecksum;        // CRC32 checksum of the application vector table
    uint32_t appChecksum;           // CRC32 checksum of the entire application
} AppInfo_T;

/* GLOBAL VARIABLES */


/* FUNCTIONS */


#endif