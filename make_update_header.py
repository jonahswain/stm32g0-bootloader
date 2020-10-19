# STM32G0 Bootloader
# Jonah Swain
# Python script to convert application binaries into C header files

# === DEPENDENCIES ===
import sys
import binascii
from string import Template

# === GLOBAL VARIABLES ===
vector_table_size = 47 # Application vector table length (words/entries) (STM32G071: 16 Cortex-M entries + 31 peripheral entries)

header_content = """/*
STM32G0 Bootloader
Jonah Swain

Application binary (header)
C header representation of the application binary ${binname}
*/

/* INCLUDE GUARD */
#pragma once
#ifndef APP_BINARY_H
#define APP_BINARY_H

/* DEPENDENCIES */
#include <stdint.h>
#include "app_info.h"

/* CONSTANT DEFINITIONS AND MACROS */
#define APP_BINARY_SIZE ${binsize}

/* APPLICATION BINARY */
uint8_t APP_BINARY[APP_BINARY_SIZE] = {${binstr}};

/* APPLICATION INFO */
AppInfo_T APP_INFO = {${appid}, ${appver}, APP_BINARY_SIZE, ${vtcrc32}, ${appcrc32}};

#endif"""

# === FUNCTIONS ====

def main(): # Main function
    # Check command line arguments
    if (len(sys.argv) != 5):
        print("Error: Incorrect command line arguments. Call the program as follows:\n python make_update_header.py <input_binary_file> <output_header_file> <app_id> <app_verion>")
        return
    
    binfile = open(sys.argv[1], 'rb') # Open binary file
    binary = binfile.read() # Read bytes from file
    binfile.close() # Close binary file

    if (len(binary) % 8): # Pad binary for double-word alignment
        for i in range(8 - (len(binary) % 8)):
            binary += b'\xff'

    binary_string = "".join("0x%02X, "%byte for byte in binary) # Get string representation of application binary
    binary_checksum = "0x%08X"%(binascii.crc32(binary) & 0xFFFFFFFF) # Calculate CRC32 checksum of application binary
    vectbl_checksum = "0x%08X"%(binascii.crc32(binary[0:4*vector_table_size]) & 0xFFFFFFFF) # Calculate CRC32 checksum of application vector table

    app_header = Template(header_content).substitute(binname=sys.argv[1], binsize=len(binary), binstr=binary_string[0:len(binary_string) - 2], appid="0x%08X"%int(sys.argv[3]), appver="0x%08X"%int(sys.argv[4]), vtcrc32=vectbl_checksum, appcrc32=binary_checksum) # Format header content with values

    hdrfile = open(sys.argv[2], 'w') # Open/create header file
    hdrfile.write(app_header) # Write data to header file
    hdrfile.close() # Close header file

# === RUN ===
if (__name__ == "__main__"):
    main() # Run main function if file is being run as main