#include "my_nm.h"
#include <elf.h>

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
 * @brief Get the symbol size from a symbol table entry.
 *
 * Many symbols have associated sizes. Returns zero if the symbol has
 * no size or an unknown size.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol size (st_size field)
 */
uint64_t symtab_entry_get_size(SymbolTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.st_size);
	case BITS_64:
		return (entry->data.s64.st_size);
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
 * @brief Get the symbol binding from a symbol table entry.
 *
 * Extracts the binding (STB_LOCAL, STB_GLOBAL, STB_WEAK) from st_info.
 *
 * @param entry Pointer to SymbolTableEntry
 * @return The symbol binding
 */
uint8_t symtab_entry_get_bind(SymbolTableEntry *entry) {
	uint8_t info = symtab_entry_get_info(entry);
	switch (entry->type) {
	case BITS_32:
		return (ELF32_ST_BIND(info));
	case BITS_64:
		return (ELF64_ST_BIND(info));
	default:
		return (0);
	}
}

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
const char *symtab_entry_get_name_string(ElfFile *elf, SymbolTable *table, SymbolTableEntry *entry) {
	return (elf->file_data.data + table->associated_strtab_offset + symtab_entry_get_name_index(entry));
}

/**
 * @brief Get the nm-style type character for a symbol.
 *
 * Returns the character that nm displays for this symbol type.
 * Uppercase = global/weak, lowercase = local (with some exceptions).
 *
 * @param elf Pointer to ElfFile
 * @param entry Pointer to SymbolTableEntry
 * @return The nm type character
 */
char symtab_entry_get_nm_type(ElfFile *elf, SymbolTableEntry *entry) {
	uint16_t                shndx = symtab_entry_get_shndx(entry);
	uint8_t                 bind = symtab_entry_get_bind(entry);
	uint8_t                 type = symtab_entry_get_type(entry);
	SectionHeaderTableEntry section;
	char                    c;

	/* Undefined symbol */
	if (shndx == SHN_UNDEF) {
		return ('U');
	}

	/* Absolute symbol */
	if (shndx == SHN_ABS) {
		return (bind == STB_GLOBAL ? 'A' : 'a');
	}

	/* Common symbol */
	if (shndx == SHN_COMMON) {
		return (bind == STB_GLOBAL ? 'C' : 'c');
	}

	/* Weak symbols */
	if (bind == STB_WEAK) {
		if (type == STT_OBJECT) {
			return (shndx == SHN_UNDEF ? 'v' : 'V');
		}
		return (shndx == SHN_UNDEF ? 'w' : 'W');
	}

	/* Get the section this symbol is in */
	sh_table_get_at(elf, shndx, &section);
	uint32_t sh_type = shtable_entry_get_type(&section);
	uint64_t sh_flags = shtable_entry_get_flags(&section);

	/* Determine type based on section */
	if (sh_type == SHT_NOBITS) {
		/* BSS section (uninitialized data) */
		c = 'B';
	} else if (sh_flags & SHF_EXECINSTR) {
		/* Text/code section */
		c = 'T';
	} else if (sh_flags & SHF_WRITE) {
		/* Writable data section */
		c = 'D';
	} else if (sh_flags & SHF_ALLOC) {
		/* Read-only data section */
		c = 'R';
	} else {
		/* Debug or other non-alloc section */
		c = 'N';
	}

	/* Convert to lowercase for local symbols */
	if (bind == STB_LOCAL) {
		c = c + ('a' - 'A');
	}

	return (c);
}
