#include "my_nm.h"
#include <stdio.h>

int main(int argc, char **argv) {
	ElfFile					elf;
	SymbolTable				symtab;
	SectionHeaderTableEntry symtab_header;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
		return (1);
	}

	// Load the ELF file - this opens, mmaps, and parses
	// everything regarding the elf file
	if (elf_file_load(argv[1], &elf) != 0) {
		fprintf(stderr, "Error: Failed to load ELF file '%s'\n", argv[1]);
		return (1);
	}

	if (sh_table_get_symtab_header(&elf, &symtab_header) != 0) {
		return (-1);
	}
	if (symtab_parse(&elf, &symtab_header, &symtab) != 0) {
		printf("There is no symbol table in this ELF file");
		return (-1);
	}

	for (SymbolList *curr_symbol = symtab.symlist; curr_symbol != NULL;
		 curr_symbol = curr_symbol->next) {
		if (curr_symbol->value == 0) {
			printf("\t\t %c %s\n", curr_symbol->digit, curr_symbol->name);
		} else {
			printf("%016lx %c %s\n", (unsigned long)curr_symbol->value,
				   curr_symbol->digit, curr_symbol->name);
		}
	}
	/* Clean up - unmaps memory and frees allocated resources */
	elf_file_free(&elf);

	return (0);
}
