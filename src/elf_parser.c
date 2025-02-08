#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/elf_parser.h"

void get_bytes(FILE *fd, long offset, char *data, long buffer_size) {
	fseek(fd, offset, SEEK_SET);
	fread(data, buffer_size, 1, fd);
}

ELF_File *elf_open(const char *elf_file_path) {
    ELF_File *elf = malloc(sizeof(ELF_File));

    if (elf == NULL) {
        perror("Error allocating memory for ELF_File");
        return NULL;
    }

    elf->fd = fopen(elf_file_path, "rb");

    if (elf->fd == NULL) {
        perror("Error opening ELF fd");
        free(elf);
        return NULL;
    }

    // Read the ELF header
    if (fread(&elf->eh, 1, sizeof(ELF32_EH), elf->fd) != sizeof(ELF32_EH)) {
        perror("Error reading ELF header");
        fclose(elf->fd);
        free(elf);
        return NULL;
    }

    // Check if it is ELF64
    if (elf->eh.eh32.eh_ident[EI_CLASS] == ELFCLASS64) {
        elf->is_elf64 = 1;

        // Rewind and read the ELF64 header
        rewind(elf->fd);
        if (fread(&elf->eh, 1, sizeof(ELF64_EH), elf->fd) != sizeof(ELF64_EH)) {
            perror("Error reading ELF64 header");
            fclose(elf->fd);
            free(elf);
            return NULL;
        }
    } else {
        elf->is_elf64 = 0;
    }

    return elf;
}

void elf_close(ELF_File *elf) {
    if (elf == NULL) {
        return;
    }

    if (elf->fd != NULL) {
        fclose(elf->fd);
    }
    free(elf);
}

void elf_read_header(ELF_File *elf, ELF_EH *elf_header_generic) {
    size_t elf_header_size;
    void *elf_header;

    if (elf->is_elf64) {
        elf_header_size = sizeof(ELF64_EH);
        elf_header = &(*elf_header_generic).eh64;
    } else {
        elf_header_size = sizeof(ELF32_EH);
        elf_header = &(*elf_header_generic).eh32;
    }

    fseek(elf->fd, 0, SEEK_SET);
    fread(elf_header, elf_header_size, 1, elf->fd);
}
 
const char *elf_get_section_name(ELF_File *elf, ELF_SH section_header_generic) {
    uint64_t elf_header_entry_size;
    uint64_t elf_header_string_index;
    uint64_t elf_header_offset;

    if (elf->is_elf64) {
        elf_header_entry_size = elf->eh.eh64.eh_shentsize;
        elf_header_string_index = elf->eh.eh64.eh_shstrndx;
        elf_header_offset = elf->eh.eh64.eh_shoff;
    } else {
        elf_header_entry_size = elf->eh.eh32.eh_shentsize;
        elf_header_string_index = elf->eh.eh32.eh_shstrndx;
        elf_header_offset = elf->eh.eh32.eh_shoff;
    }


    fseek(elf->fd, elf_header_offset + elf_header_string_index * elf_header_entry_size, SEEK_SET);
    
    ELF_SH section_string_table_header;

    if (elf->is_elf64) {
        fread(&section_string_table_header.sh64, sizeof(ELF64_SH), 1, elf->fd);
        char *section_names = (char *)malloc(section_string_table_header.sh64.sh_size);
        fseek(elf->fd, section_string_table_header.sh64.sh_offset, SEEK_SET);
        fread(section_names, section_string_table_header.sh64.sh_size, 1, elf->fd);
        return section_names + section_header_generic.sh64.sh_name;
        
    } else {
        fread(&section_string_table_header.sh32, sizeof(ELF32_SH), 1, elf->fd);
        char *section_names = (char *)malloc(section_string_table_header.sh32.sh_size);
        fseek(elf->fd, section_string_table_header.sh32.sh_offset, SEEK_SET);
        fread(section_names, section_string_table_header.sh32.sh_size, 1, elf->fd);
        return section_names + section_header_generic.sh32.sh_name;
    }
}

void elf_print_section(ELF_File *elf, ELF_SH section_header_generic, const char *section_name) {
    uint64_t buffer_size;
    uint64_t offset = 0;

    if (elf->is_elf64) {
        buffer_size = section_header_generic.sh64.sh_size;
        offset = section_header_generic.sh64.sh_offset;
    } else {
        buffer_size = section_header_generic.sh32.sh_size;
        offset = section_header_generic.sh32.sh_offset;
    }

    char *buffer = malloc(buffer_size);
    
    get_bytes(elf->fd, offset, buffer, buffer_size);
    printf("Printing Contents of \"%s\" Section\n", section_name);

    for (size_t i = 0; i < buffer_size; i += 16) {
        printf("%08zx ", i);

        // Print hex values
        size_t j;
        for (j = 0; j < 16 && i + j < buffer_size; j++) {
            if (j % 4 == 0) {
                printf(" ");
            }
            printf("%02x", (unsigned char)buffer[i + j]);
        }

        // Print spaces for alignment if needed
        for (size_t k = 16 - j; k > 0; k--) {
            printf("  ");
        }

        printf("  ");

        // Print ASCII values
        for (size_t k = 0; k < j; k++) {
            char ch = buffer[i + k];
            printf("%c", (ch >= 32 && ch <= 126) ? ch : '.');
        }

        printf("\n");
    }
    free(buffer);
}

void elf_read_section_header_64(ELF_File* elf, ELF64_EH elf_header, uint64_t section_index, ELF64_SH *section_header) {
    uint64_t section_offset = elf_header.eh_shoff;
    uint64_t section_entry_size = elf_header.eh_shentsize;

    fseek(elf->fd, section_offset + section_index * section_entry_size, SEEK_SET);

    fread(section_header, sizeof(ELF64_SH), 1, elf->fd);

}

void elf_read_section_header_32(ELF_File* elf, ELF32_EH elf_header, uint64_t section_index, ELF32_SH *section_header) {
    uint64_t section_offset = elf_header.eh_shoff;
    uint64_t section_entry_size = elf_header.eh_shentsize;

    fseek(elf->fd, section_offset + section_index * section_entry_size, SEEK_SET);

    fread(section_header, sizeof(ELF32_SH), 1, elf->fd);
}

void elf_yank_section(ELF_File *elf, char *target_section, ELF_SH *found_section) {
    if (elf->is_elf64) {
        ELF_EH elf_header;
        elf_header.eh64 = elf->eh.eh64;
        elf_read_header(elf, &elf_header);
        for (size_t section_index = 0; section_index < elf->eh.eh64.eh_shnum; section_index++) {

            ELF_SH section_header;
            elf_read_section_header_64(elf, elf->eh.eh64, section_index, &section_header.sh64);

            const char *section_name = elf_get_section_name(elf, section_header);

            if (strcmp(section_name, target_section) == 0) {
                printf("Notable Section Found: %s\n", section_name);
                *found_section = (ELF_SH)section_header;
            }
        }
    } else {
        ELF_EH elf_header;
        elf_header.eh32 = elf->eh.eh32;
        elf_read_header(elf, &elf_header);
        for (size_t section_index = 0; section_index < elf->eh.eh32.eh_shnum; section_index++) {

            ELF_SH section_header;
            elf_read_section_header_32(elf, elf->eh.eh32, section_index, &section_header.sh32);

            const char *section_name = elf_get_section_name(elf, section_header);

            if (strcmp(section_name, target_section) == 0) {
                printf("Notable Section Found: %s\n", section_name);
                *found_section = (ELF_SH)section_header;
            }
        }
    }
}