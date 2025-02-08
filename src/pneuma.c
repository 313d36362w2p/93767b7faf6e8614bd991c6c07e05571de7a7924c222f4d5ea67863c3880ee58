#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "elf_parser.c"

#include "../headers/pneuma.h"

CRC_Entry *parse_versions_table_64(ELF_File *elf, ELF64_SH section_header) {
    // Creates a CRC_Entry array on The Heap and returns the pointer to it.
    return NULL;
}

CRC_Entry *parse_versions_table_32(ELF_File *elf, ELF32_SH section_header) {
    // Creates a CRC_Entry array on The Heap and returns the pointer to it.
    return NULL;
}

MI_Entry *parse_modinfo_table_64(ELF_File *elf, ELF64_SH section_header) {
    // Creates a MI_Entry array on The Heap and returns the pointer to it.
    return NULL;
}

MI_Entry *parse_modinfo_table_32(ELF_File *elf, ELF32_SH section_header) {
    // Creates a MI_Entry array on The Heap and returns the pointer to it.
    return NULL;
}

int main(int argc, char *argv[]) {
    ELF_File *elf = NULL;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf_fd>\n", argv[0]);
        return 1;
    }

    const char *elf_file_path = argv[1];

    elf = elf_open(elf_file_path);

    if (elf == NULL)
        return 1;

    ELF_SH section;
    elf_yank_section(elf, ".modinfo", &section);
    elf_print_section(elf, section, ".modinfo");
    
    elf_close(elf);
    return 0;
}