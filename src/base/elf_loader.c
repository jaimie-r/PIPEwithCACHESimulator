/**************************************************************************
 * STUDENTS: DO NOT MODIFY.
 * 
 * C S 429 system emulator
 * 
 * elf_loader.c - Module for loading an ELF executable into emulated memory.
 * 
 * Copyright (c) 2022, 2023.
 * Authors: S. Chatterjee, Z. Leeper.
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "err_handler.h"
#include "mem.h"
#include "machine.h"
#include "ptable.h"

extern machine_t guest;

uint64_t loadElf(const char *fileName) {
    logging(LOG_INFO, "Loading ELF executable");
    // Open the file.
    int fd = open(fileName, O_RDONLY);
    if (fd < 0) {
        perror(fileName);
        exit(-1);
    }
    
    // Get file stats.
    struct stat statBuffer;
    int rc = fstat(fd, &statBuffer);
    if (rc != 0) {
        perror("stat");
        exit(-1);
    }
    
    // Mmap the file for quick access.
    uintptr_t ptr = (uintptr_t) mmap(0, statBuffer.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ((void *)ptr == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }
    
    // Get ELF header information.
    Elf64_Ehdr *header = (Elf64_Ehdr *) ptr;
    assert(header->e_type == ET_EXEC); // Check that it's an executable.
    uint64_t entry = header->e_entry; // Entry point of ELF executable.
    uint64_t entry_size = header->e_phentsize;
    uint64_t entry_count = header->e_phnum;
    
    // Get ELF program header and load segments.
    Elf64_Phdr *progHeader = (Elf64_Phdr *)(ptr + header->e_phoff);
    for (unsigned i = 0; i < entry_count; i++) {
        if (progHeader->p_type == PT_LOAD) {
            uint8_t *dataPtr = (uint8_t *)(ptr + progHeader->p_offset);
            uint64_t vaddr = progHeader->p_vaddr;
            uint64_t filesz = progHeader->p_filesz;
            uint64_t memsz = progHeader->p_memsz;

            unsigned long v_align = (vaddr % PAGESIZE);
            unsigned long f_align = filesz + (PAGESIZE - ((filesz + vaddr) % PAGESIZE));

            int read = !!(progHeader->p_flags & 0x4);
            int write = !!(progHeader->p_flags & 0x2);
            int exec = !!(progHeader->p_flags & 0x1);

            // Map data from the file for this segment
            for (uint64_t j = 0; j < filesz + v_align; j++) {
                uint64_t addr = vaddr + j;
                uint8_t byte = dataPtr[j];
                uint64_t pnum = addr / PAGESIZE;
                uint64_t poff = addr % PAGESIZE;
                pte_ptr_t page = get_page(pnum);
                if (NULL == page)
                    page = add_page(pnum, read | write << 1 | exec << 2);
                page->p_data[poff] = byte;
            }
            // Map bss address space
            // Data initialization to 0 happens indirectly through calloc() call in add_page().
            if (memsz > filesz) {
                for (uint64_t j = 0; j < memsz - filesz; j++) {
                    uint64_t addr = vaddr + f_align + j;
                    uint64_t pnum = addr / PAGESIZE;
                    pte_ptr_t page = get_page(pnum);
                    if (NULL == page)
                        page = add_page(pnum, read | write << 1 | exec << 2);
                }
            }       
        }
        progHeader = (Elf64_Phdr *) (((uintptr_t) progHeader) + entry_size);
    }

    // Read section header to fill in machine memory segments
    Elf64_Shdr *sectionHeader = (Elf64_Shdr *)(ptr + header->e_shoff);
    entry_size = header->e_shentsize;
    entry_count = header->e_shnum;
    Elf64_Shdr *sectionStrings = (Elf64_Shdr *)((char *)sectionHeader + (header->e_shstrndx*entry_size));
    char *strings = (char *)ptr + sectionStrings->sh_offset;
    for (unsigned i = 0; i < entry_count; i++) {
        char *name = strings + sectionHeader->sh_name;
        if (!strcmp(name, ".text")) {
            guest.mem->seg_start_addr[TEXT_SEG] = sectionHeader->sh_addr;
        }
        if (!strcmp(name, ".data")) {
            guest.mem->seg_start_addr[DATA_SEG] = sectionHeader->sh_addr;
        }
        sectionHeader = (Elf64_Shdr *) (((uintptr_t) sectionHeader) + entry_size);
    }

    return entry;
}
