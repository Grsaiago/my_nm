#include "my_nm.h"
#include <stdio.h>

int main(int argc, char **argv) {
	ElfFile			  elf;
	SymbolTable		  symtab;
	SymbolTableHeader symtab_header;

	if (argc < 2) {
		printf("Usage: %s <elf_file>\n", argv[0]);
		return (1);
	}

	for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
		// Load the ELF file - this opens, mmaps, and parses
		// everything regarding the elf file
		if (elf_file_load(argv[arg_idx], &elf) != 0) {
			printf("%s failed to load ELF file '%s', are you sure it's an "
				   "ELF file?\n",
				   argv[0], argv[arg_idx]);
			continue;
		}

		if (sh_table_get_symtab_header(&elf, &symtab_header) != 0) {
			printf("%s: %s: no symbol table\n", argv[0], argv[arg_idx]);
			continue;
		}
		if (symtab_parse(&elf, &symtab_header, &symtab) != 0) {
			printf("%s: %s: no string table, that's a stripped binary\n",
				   argv[0], argv[arg_idx]);
			continue;
		}

		printf("%s:\n", argv[arg_idx]);
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
		symblst_clear(&symtab.symlist);
		elf_file_free(&elf);
		if (argv[arg_idx + 1] != NULL) {
			printf("\n");
		}
	}

	return (0);
}
