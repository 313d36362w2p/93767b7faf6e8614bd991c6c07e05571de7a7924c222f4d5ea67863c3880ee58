/* File class */
#define EI_CLASS 4

/* Invalid class */
#define ELFCLASSNONE 0

/* 32-bit objects */
#define ELFCLASS32 1

/* 64-bit objects */
#define ELFCLASS64 2

// ELF header for 32-bit architecture (x86)
typedef struct {
    uint8_t eh_ident[16];
    uint16_t eh_type;
    uint16_t eh_machine;
    uint32_t eh_version;
    uint32_t eh_entry;
    uint32_t eh_phoff;
    uint32_t eh_shoff;
    uint32_t eh_flags;
    uint16_t eh_ehsize;
    uint16_t eh_phentsize;
    uint16_t eh_phnum;
    uint16_t eh_shentsize;
    uint16_t eh_shnum;
    uint16_t eh_shstrndx;
} ELF32_EH;

// Section header for 32-bit architecture (x86)
typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} ELF32_SH;

// ELF header for 64-bit architecture (x86_64)
typedef struct {
    uint8_t eh_ident[16];
    uint16_t eh_type;
    uint16_t eh_machine;
    uint32_t eh_version;
    uint64_t eh_entry;
    uint64_t eh_phoff;
    uint64_t eh_shoff;
    uint32_t eh_flags;
    uint16_t eh_ehsize;
    uint16_t eh_phentsize;
    uint16_t eh_phnum;
    uint16_t eh_shentsize;
    uint16_t eh_shnum;
    uint16_t eh_shstrndx;
} ELF64_EH;

// Section header for 64-bit architecture (x86_64)
typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} ELF64_SH;

// ELF file descriptor
typedef struct {
    FILE *fd;
    uint8_t is_elf64;
    union {
        ELF32_EH eh32;
        ELF64_EH eh64;
    } eh;
} ELF_File;

// ELF Section Header Union
typedef union {
    ELF64_SH sh64;
    ELF32_SH sh32;
} ELF_SH;

// ELF Header Union
typedef union {
    ELF64_EH eh64;
    ELF32_EH eh32;
} ELF_EH;


ELF_File* elf_open(const char *elf_file_path);
void elf_close(ELF_File *elf);
void get_bytes(FILE *fd, long offset, char *data, long buffer_size);
void elf_read_section_header(ELF_File *elf, ELF64_SH *section_header, int index, ELF64_EH elf_header);
void elf_read_header(ELF_File *elf, ELF_EH *elf_header_generic);
const char *elf_get_section_name(ELF_File *elf, ELF_SH section_header_generic);
void elf_print_section(ELF_File *elf, ELF_SH section_header_generic, const char *section_name);
void elf_yank_section(ELF_File *elf, char *target_section, ELF_SH *found_section);