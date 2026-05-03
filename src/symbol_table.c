#include "my_nm.h"
#include <ctype.h>
#include <elf.h>
#include <stdint.h>
#include <string.h>

static int	symtab_parse_symbol_list(ElfFile *elf, SymbolTable *table);
static char resolve_nm_char(ElfFile *elf, SymbolTableEntry *entry);

/**
 * @brief Parse and initialize a symbol table from a section header.
 *
 * @param elf Pointer to ElfFile
 * @param symtab_section Pointer to the symbol table section header
 * @param table Pointer to SymbolTable structure to initialize
 * @return 0 on success, -1 on error
 */
int symtab_parse(ElfFile *elf, SectionHeaderTableEntry *symtab_section_header,
				 SymbolTable *table) {
	uint64_t offset;
	uint64_t size;
	uint64_t entsize;
	uint32_t strtab_index;

	/* Get symbol table properties */
	offset = shtable_entry_get_offset(symtab_section_header);
	size = shtable_entry_get_size(symtab_section_header);
	entsize = shtable_entry_get_entsize(symtab_section_header);

	if (size == 0 || entsize == 0) {
		return (-1);
	}

	/* Get the associated string table section header */
	strtab_index = shtable_entry_get_link(symtab_section_header);
	sh_table_get_at(elf, strtab_index,
					&table->associated_strtab_section_header);

	/* Verify it's actually a string table */
	if (shtable_entry_get_type(&table->associated_strtab_section_header) !=
		SHT_STRTAB) {
		return (-1);
	}

	/* Initialize the symbol table structure */
	table->entry_size = entsize;
	table->entry_count = size / entsize;
	table->table_start_offset = offset;
	table->current_index = 0;
	table->associated_strtab_offset =
		shtable_entry_get_offset(&table->associated_strtab_section_header);

	symtab_parse_symbol_list(elf, table);
	return (0);
}

/* Table Iteration */

/**
 * @brief Get the current symbol table entry.
 *
 * @param elf Pointer to ElfFile
 * @param table Pointer to SymbolTable
 * @param entry Pointer to SymbolTableEntry to fill
 */
void symtab_get_current(ElfFile *elf, SymbolTable *table,
						SymbolTableEntry *entry) {
	symtab_get_at(elf, table, table->current_index, entry);
}

/**
 * @brief Get a symbol table entry at a specific index.
 *
 * @param elf Pointer to ElfFile
 * @param table Pointer to SymbolTable
 * @param index Index of the symbol entry
 * @param entry Pointer to SymbolTableEntry to fill
 */
void symtab_get_at(ElfFile *elf, SymbolTable *table, size_t index,
				   SymbolTableEntry *entry) {
	if (index >= table->entry_count) {
		return;
	}

	switch (elf_get_arch_type(elf)) {
	case (BITS_32):
		entry->type = BITS_32;
		memcpy(&entry->data.s32,
			   (elf->file_data.data + table->table_start_offset) +
				   (index * table->entry_size),
			   sizeof(Elf32_Sym));
		break;
	case (BITS_64):
		entry->type = BITS_64;
		memcpy(&entry->data.s64,
			   (elf->file_data.data + table->table_start_offset) +
				   (index * table->entry_size),
			   sizeof(Elf64_Sym));
		break;
	}
}

/**
 * @brief Move to the next symbol table entry.
 *
 * @param table Pointer to SymbolTable
 * @return 0 if moved to next, -1 if already at end
 */
int symtab_next(SymbolTable *table) {
	if (table->current_index >= table->entry_count) {
		return (-1);
	}
	table->current_index++;
	return (0);
}

/**
 * @brief Reset the symbol table iterator to the beginning.
 *
 * @param table Pointer to SymbolTable
 */
void symtab_reset(SymbolTable *table) { table->current_index = 0; }

/**
 * @brief Check if there are more symbol table entries to iterate over.
 *
 * @param table Pointer to SymbolTable
 * @return 1 if there are more entries, 0 otherwise
 */
int symtab_has_more(SymbolTable *table) {
	return (table->current_index < table->entry_count);
}

/**
 * @brief Initializa the [SymbolList] for/from a [SymbolTable]
 *
 * @param elf the associated elf file
 * @param symtab the symbol table to read from and attribute the resulting
 [SymbolList] to
 * @return 1 error
 * @return 0 no error
*/
static int symtab_parse_symbol_list(ElfFile *elf, SymbolTable *symtab) {
	SymbolTableEntry symtab_entry;
	SymbolList		*new = NULL;
	unsigned char	 nm_char;

	symtab->symlist = NULL;
	for (; symtab_has_more(symtab); symtab_next(symtab)) {
		symtab_get_current(elf, symtab, &symtab_entry);
		// skip empty symbol or file symbol names
		if (symtab_entry_get_name_index(&symtab_entry) == 0 ||
			symtab_entry_get_type(&symtab_entry) == STT_FILE) {
			continue;
		}
		nm_char = resolve_nm_char(elf, &symtab_entry);
		new = symblst_new(
			symtab_entry_get_value(&symtab_entry),
			(char *)symtab_entry_get_name_string(elf, symtab, &symtab_entry),
			false, nm_char);
		// TODO: Proteção contra nulo aqui em
		symblst_add_back(&symtab->symlist, new);
	}
	symblst_sort(&symtab->symlist, symblst_cmp_lexicographic);
	return (0);
}

static char resolve_nm_char(ElfFile *elf, SymbolTableEntry *entry) {
	SectionHeaderTableEntry associated_section;
	unsigned char			chr = '?';
	unsigned char			symbol_attribute_bind;
	unsigned char			symbol_attribute_type;

	sh_table_get_at(elf, symtab_entry_get_shndx(entry), &associated_section);
	symbol_attribute_bind = symtab_entry_extract_st_bind(entry);
	symbol_attribute_type = symtab_entry_extract_st_type(entry);

	// Let's rule out the GNU extensions
	if (symbol_attribute_bind == STB_GNU_UNIQUE) {
		return 'u';
	} else if (symbol_attribute_type == STT_GNU_IFUNC) {
		return 'i';
	}
	// weak symbols are globals that provide a "default value"
	// think of a library that has a default implementation of some func
	// that the user can overwrite.
	//
	// We'll rule out all the STB_WEAK + SHN_UNDEF
	else if (symbol_attribute_bind == STB_WEAK) {
		chr = 'W'; // the weak symbol was defined
		if (symtab_entry_get_shndx(entry) == SHN_UNDEF) {
			chr = 'w'; // the symbol was not defined
		}
	} else if (symbol_attribute_bind == STB_WEAK &&
			   symbol_attribute_type == STT_OBJECT) {
		chr = 'V';
		if (symtab_entry_get_shndx(entry) == SHN_UNDEF) {
			chr = 'v';
		}
	} // any other SHN_UNDEF without a STB_WEAK is just an undefined symbol
	else if (symtab_entry_get_shndx(entry) == SHN_UNDEF) {
		chr = 'U';
	} // absolute symbols
	else if (symtab_entry_get_shndx(entry) == SHN_ABS) {
		chr = 'A';
	} // the symbol is in the .bss section
	else if (shtable_entry_get_type(&associated_section) == SHT_NOBITS &&
			 (shtable_entry_get_flags(&associated_section) &
			  (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE)) {
		chr = 'b';
	} // the symbol is a common symbol
	else if (symtab_entry_get_shndx(entry) == SHN_COMMON) {
		chr = 'C';
		if (symtab_entry_get_shndx(entry) == SHN_MIPS_SCOMMON) {
			chr = 'c';
		}
	}
	// same as the weak symbol logic, we'll eliminate all 't's
	// before continuing to check for other SHT_PROGBITS combinations
	else if (shtable_entry_get_type(&associated_section) == SHT_PROGBITS &&
			 (shtable_entry_get_flags(&associated_section) &
			  (SHF_ALLOC | SHF_EXECINSTR)) == (SHF_ALLOC | SHF_EXECINSTR)) {
		chr = 't';
	}
	// initialized data section (.ctors, .data, .data1, .dtors).
	// I don't check the SHT_PROGBITS because on
	// trial and error testing that's what worked
	else if ((shtable_entry_get_flags(&associated_section) &
			  (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE)) {
		chr = 'd';
	}
	// TODO 'N': Tem que reestruturar as structs pra poder
	// cascatear a string table até aqui pra poder fazer um
	// cmp com '.debug' e '.line'.
	else if ((shtable_entry_get_flags(&associated_section) & SHF_ALLOC) ==
			 SHF_ALLOC) {
		chr = 'r';
	}

	if (symbol_attribute_bind == STB_GLOBAL && chr != '?') {
		chr = toupper(chr);
	}
	return (chr);
}
