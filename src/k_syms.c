#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../headers/k_syms.h"

int len_string(FILE * file, long offset) {
	char * c = malloc(1);
	int len = 1;
	fseek(file, offset, SEEK_SET);
	fread(c, 1, 1, file);
	while (c[0] != 0) {
		fread(c, 1, 1, file);
		len++;
	}
	free(c);
	return len;
}

void get_bytes(FILE * file, long offset, char * data, long buffer_size) {
	fseek(file, offset, SEEK_SET);
	fread(data, buffer_size, 1, file);
}

void parse_symbols(FILE * f, struct sections * s) { // int64_t ksymtab_offset, int64_t ksymtab_size, int64_t kcrctab_offset) {
	char * symbol_buf = malloc(sizeof(struct symbol));
	char * crc_buf = malloc(4);
	for (int i = 0; i < s->ksymtab_size + s->ksymtab_size_gpl; i += sizeof(struct symbol)) {
		get_bytes(f, s->ksymtab_offset + i, symbol_buf, sizeof(struct symbol));
		struct symbol curr_sym = *(struct symbol*)symbol_buf;

		int len = len_string(f, s->ksymtab_offset + 4 + curr_sym.name_offset + i);
		char * str_buf = malloc(len);
		get_bytes(f, s->ksymtab_offset + 4 + curr_sym.name_offset + i, str_buf, len);
		get_bytes(f, s->kcrctab_offset + 4 * (i / sizeof(struct symbol)), crc_buf, 4);
		printf("0x%08x %s\n", *(int32_t*)crc_buf, str_buf);
		free(str_buf);
	}
	free(symbol_buf);
	free(crc_buf);
}

void get_sections(FILE * f, struct sections * s) {
	char * buffer = malloc(8);

	get_bytes(f, 0x28, buffer, 4);
	int32_t section_off = *(int32_t*)buffer;

	get_bytes(f, 0x3E, buffer, 2);
	int16_t shstr_index = *(int16_t*)buffer;

	get_bytes(f, 0x3A, buffer, 2);
	int16_t section_size = *(int16_t*)buffer;

	get_bytes(f, 0x3C, buffer, 2);
	int16_t section_num = *(int16_t*)buffer;

	get_bytes(f, section_off + section_size * shstr_index + 0x18, buffer, 8);
	int64_t shstr_off = *(int64_t*)buffer;

	for (int i = 1; i < section_num; i++) {
		long curr_section_offset = section_off + section_size * i;
		get_bytes(f, curr_section_offset, buffer, 4);
		int32_t string_offset = *(int32_t*)buffer;
		int len = len_string(f, shstr_off + string_offset);

		char * str_buf = malloc(len);
		get_bytes(f, shstr_off + string_offset, str_buf, len);
		if (strcmp(str_buf, "__kcrctab") == 0) {
			get_bytes(f, curr_section_offset + 0x18, buffer, 8);
			s->kcrctab_offset = *(int64_t*)buffer;

		}
		else if (strcmp(str_buf, "__ksymtab") == 0) {
			get_bytes(f, curr_section_offset + 0x20, buffer, 8);
			s->ksymtab_size = *(int64_t*)buffer;

			get_bytes(f, curr_section_offset + 0x18, buffer, 8);
			s->ksymtab_offset = *(int64_t*)buffer;
		}
		else if (strcmp(str_buf, "__ksymtab_gpl") == 0) {
			get_bytes(f, curr_section_offset + 0x20, buffer, 8);
			s->ksymtab_size_gpl = *(int64_t*)buffer;

		}
		free(str_buf);
	}
	free(buffer);
}

int main(int argc, char ** argv) {
	if (argc != 2) {
		printf("usage: crc_parser [KERNEL]");
		return 1;
	}
	FILE *f = fopen(argv[1], "rb");
	struct sections s;
	if (f == NULL) {
		printf("Failed to load file\n");
		return 1;
	}
	// TODO: Add ELF file checking and 32/64bit checking
	get_sections(f, &s);
	parse_symbols(f, &s);
	fclose(f);
}
