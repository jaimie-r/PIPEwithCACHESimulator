/**************************************************************************
 * C S 429 system emulator
 * 
 * elf_loader.h - A header file defining the function prototype for loading
 * an AArch64/Linux ELF image into the emulator.
 * 
 * Copyright (c) 2022, 2023. 
 * Author: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _ELF_LOADER_H_
#define _ELF_LOADER_H_
#include <stdint.h>

extern uint64_t loadElf(const char *file);
#endif