#include "my_nm.h"
#include <elf.h>

int main(int argc, char **argv) {
	ElfFile elf;
	int		result;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
		return (1);
	}

	/* Load the ELF file - this opens, mmaps, and parses everything */
	result = elf_file_load(argv[1], &elf);
	if (result != 0) {
		fprintf(stderr, "Error: Failed to load ELF file '%s'\n", argv[1]);
		return (1);
	}

	/* Print ELF header information */
	// elf_hdr_debug_print(&elf);
	// printf("\n");

	/* Example: Iterate through section headers */
	// printf("Section Headers:\n");
	sh_table_reset(&elf);

	size_t					symtab_index = 0;
	SectionHeaderTableEntry symtab_header;
	SymbolTableEntry		symtab_entry;
	SymbolTable				symtab;
	if (sh_table_get_symtab_header(&elf, &symtab_header) != 0) {
		return (-1);
	}
	if (symtab_parse(&elf, &symtab_header, &symtab) != 0) {
		printf("There is no symbol table in this ELF file");
		return (-1);
	}
	for (; symtab_has_more(&symtab); symtab_next(&symtab), symtab_index++) {
		symtab_get_current(&elf, &symtab, &symtab_entry);
		// skip empty symbol or file symbol names
		if (symtab_entry_get_name_index(&symtab_entry) == 0 ||
			symtab_entry_get_type(&symtab_entry) == STT_FILE) {
			continue;
		}
		printf("symbol: %s\n",
			   symtab_entry_get_name_string(&elf, &symtab, &symtab_entry));
	}
	/* Clean up - unmaps memory and frees allocated resources */
	elf_file_free(&elf);

	return (0);
}
