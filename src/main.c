#include "my_nm.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	MappedFile				   mapped_file;
	ElfFile					   elf;
	SectionHeaderTableIterator shdr_it;
	SymbolTableHeader		   symtab_header;
	StringTableHeader		   strtab_header;
	SymbolTableIterator		   symtab_it;
	StringTable				   strtab;
	SymbolList				  *symbol_list;

	if (argc < 2) {
		printf("Usage: %s <elf_file>\n", argv[0]);
		return (1);
	}

	for (int arg_idx = 1; arg_idx < argc; arg_idx++) {
		// Load the ELF file - this opens, mmaps, and parses
		// everything regarding the elf file
		if (map_file(argv[arg_idx], &mapped_file) != 0) {
			printf("%s failed to map file '%s': %s\n", argv[0], argv[arg_idx],
				   strerror(errno));
			continue;
		}
		if (new_elf_file(&mapped_file, &elf) != 0) {
			printf("%s failed to load ELF file '%s', are you sure it's an "
				   "ELF file?\n",
				   argv[0], argv[arg_idx]);
			unmap_file(&mapped_file);
			continue;
		}
		shdr_it = get_section_header_table_it(&elf);

		switch (sh_table_it_get_symbol_and_associated_string_table(
			&shdr_it, &symtab_header, &strtab_header)) {
		case (0):
			break;
		case (1):
			printf("%s: %s: no symbol table\n", argv[0], argv[arg_idx]);
			continue;
		case (2):
			printf("%s: %s: no string table, that's a stripped binary\n",
				   argv[0], argv[arg_idx]);
			continue;
		}

		strtab = get_string_table(&elf, &strtab_header);
		symtab_it = get_symbol_table_it(&elf, &symtab_header);
		symbol_list = symblst_parse(&shdr_it, &symtab_it, &strtab);
		if (symbol_list == NULL) {
			printf("%s: %s: failed to parse symbol list\n", argv[0],
				   argv[arg_idx]);
			return (-1);
		}

		for (SymbolList *curr_symbol = symbol_list; curr_symbol != NULL;
			 curr_symbol = curr_symbol->next) {
			if (curr_symbol->value == 0) {
				printf("\t\t %c %s\n", curr_symbol->digit, curr_symbol->name);
			} else {
				printf("%016lx %c %s\n", (unsigned long)curr_symbol->value,
					   curr_symbol->digit, curr_symbol->name);
			}
		}
		/* Clean up - unmaps memory and frees allocated resources */
		symblst_clear(&symbol_list);
		elf_file_free(&elf);
		if (argv[arg_idx + 1] != NULL) {
			printf("\n");
		}
	}

	return (0);
}
