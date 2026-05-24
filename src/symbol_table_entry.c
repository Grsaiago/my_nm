#include "my_nm.h"

/* Symbol Table Entry Getters */

/**
 * @brief Get the symbol name index from a symbol table entry.
 *
 * Returns the index into the string table that gives the name of this symbol.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol name index (st_name field)
 */
uint32_t symtab_entry_get_name_index(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.st_name);
	case BITS_64:
		return (entry->data.s64.st_name);
	default:
		return (0);
	}
}

/**
 * @brief Get the symbol value from a symbol table entry.
 *
 * This gives the value of the associated symbol (usually an address).
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol value (st_value field)
 */
uint64_t symtab_entry_get_value(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.st_value);
	case BITS_64:
		return (entry->data.s64.st_value);
	default:
		return (0);
	}
}

/**
 * @brief Get the symbol info from a symbol table entry.
 *
 * This specifies the symbol's type and binding attributes.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol info (st_info field)
 */
uint8_t symtab_entry_get_info(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.st_info);
	case BITS_64:
		return (entry->data.s64.st_info);
	default:
		return (0);
	}
}

/**
 * @brief Get the symbol other/visibility from a symbol table entry.
 *
 * This defines the symbol visibility.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol other field (st_other field)
 */
uint8_t symtab_entry_get_other(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.st_other);
	case BITS_64:
		return (entry->data.s64.st_other);
	default:
		return (0);
	}
}

/**
 * @brief Get the section index from a symbol table entry.
 *
 * Returns the section header table index that this symbol is defined in.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The section index (st_shndx field)
 */
uint16_t symtab_entry_get_shndx(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.st_shndx);
	case BITS_64:
		return (entry->data.s64.st_shndx);
	default:
		return (0);
	}
}

/* Symbol Table Entry Helper Functions */

/**
 * @brief Get the symbol type from a symbol table entry.
 *
 * Extracts the type (STT_NOTYPE, STT_OBJECT, STT_FUNC, etc.) from st_info.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol type
 */
uint8_t symtab_entry_get_type(SymbolTableEntry *entry) {
	uint8_t info = symtab_entry_get_info(entry);
	switch (entry->type) {
	case BITS_32:
		return (ELF32_ST_TYPE(info));
	case BITS_64:
		return (ELF64_ST_TYPE(info));
	default:
		return (0);
	}
}

/**
 * @brief Get the symbol name string from a symbol table entry.
 *
 * Returns a pointer to the null-terminated symbol name string by looking
 * up the name in the associated string table.
 *
 * @param elf Pointer to ElfFile
 * @param table Pointer to SymbolTable (contains string table offset)
 * @param entry Pointer to SymbolTableEntry
 * @return Pointer to the null-terminated symbol name string
 */
const char *symtab_entry_get_name_string(SymbolTableEntry *entry,
										 StringTable	  *strtab) {
	return (&strtab->data[symtab_entry_get_name_index(entry)]);
}

unsigned char symtab_entry_extract_st_bind(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return ELF64_ST_BIND(entry->data.s32.st_info);
	case BITS_64:
		return ELF32_ST_BIND(entry->data.s64.st_info);
	}
	return (0);
}

unsigned char symtab_entry_extract_st_type(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return ELF64_ST_TYPE(entry->data.s32.st_info);
	case BITS_64:
		return ELF32_ST_TYPE(entry->data.s64.st_info);
	}
	return (0);
}
