struct sections {
	long long ksymtab_size;
	long long ksymtab_offset;
	long long kcrctab_offset;
	long long ksymtab_size_gpl;
};

struct symbol {
	int value_offset;
	int name_offset;
	int namespace_offset;
};

void get_sections(FILE *, struct sections *);

int len_string(FILE *, long);

void get_sections(FILE *, struct sections *);

void parse_symbols(FILE *, struct sections *);
