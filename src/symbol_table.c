#include "my_nm.h"
#include <elf.h>

/**
 * @brief Parse and initialize a symbol table from a section header.
 *
 * @param elf Pointer to ElfFile
 * @param symtab_section Pointer to the symbol table section header
 * @param table Pointer to SymbolTable structure to initialize
 * @return 0 on success, -1 on error
 */
int symtab_parse(ElfFile *elf, SectionHeaderTableEntry *symtab_section_header, SymbolTable *table) {
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
	sh_table_get_at(elf, strtab_index, &table->associated_strtab_section_header);

	/* Verify it's actually a string table */
	if (shtable_entry_get_type(&table->associated_strtab_section_header) != SHT_STRTAB) {
		return (-1);
	}

	/* Initialize the symbol table structure */
	table->entry_size = entsize;
	table->entry_count = size / entsize;
	table->table_start_offset = offset;
	table->current_index = 0;
	table->associated_strtab_offset = shtable_entry_get_offset(&table->associated_strtab_section_header);

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
void symtab_get_current(ElfFile *elf, SymbolTable *table, SymbolTableEntry *entry) {
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
void symtab_get_at(ElfFile *elf, SymbolTable *table, size_t index, SymbolTableEntry *entry) {
	if (index >= table->entry_count) {
		return;
	}

	switch (elf_get_arch_type(elf)) {
	case (BITS_32):
		entry->type = BITS_32;
		memcpy(
		    &entry->data.s32,
		    (elf->file_data.data + table->table_start_offset) + (index * table->entry_size), sizeof(Elf32_Sym));
		break;
	case (BITS_64):
		entry->type = BITS_64;
		memcpy(
		    &entry->data.s64,
		    (elf->file_data.data + table->table_start_offset) + (index * table->entry_size), sizeof(Elf64_Sym));
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
void symtab_reset(SymbolTable *table) {
	table->current_index = 0;
}

/**
 * @brief Check if there are more symbol table entries to iterate over.
 *
 * @param table Pointer to SymbolTable
 * @return 1 if there are more entries, 0 otherwise
 */
int symtab_has_more(SymbolTable *table) {
	return (table->current_index < table->entry_count);
}
