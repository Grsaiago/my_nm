#include "my_nm.h"

/**
 * @brief Get the section name index from a section header entry.
 *
 * Returns the index into the section header string table that gives
 * the name of this section.
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section name index (sh_name field)
 */
uint32_t shtable_entry_get_name(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_name);
	case BITS_64:
		return (entry->data.s64.sh_name);
	default:
		return (0);
	}
}

/**
 * @brief Get the section type from a section header entry.
 *
 * Categorizes the section's contents and semantics (e.g., SHT_PROGBITS,
 * SHT_SYMTAB, SHT_STRTAB, SHT_NOBITS, etc.).
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section type (sh_type field)
 */
uint32_t shtable_entry_get_type(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_type);
	case BITS_64:
		return (entry->data.s64.sh_type);
	default:
		return (0);
	}
}

/**
 * @brief Get the section flags from a section header entry.
 *
 * Returns 1-bit flags that describe miscellaneous attributes
 * (e.g., SHF_WRITE, SHF_ALLOC, SHF_EXECINSTR).
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section flags (sh_flags field)
 */
uint64_t shtable_entry_get_flags(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_flags);
	case BITS_64:
		return (entry->data.s64.sh_flags);
	default:
		return (0);
	}
}

/**
 * @brief Get the section file offset from a section header entry.
 *
 * Returns the byte offset from the beginning of the file to the first
 * byte in the section.
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section file offset (sh_offset field)
 */
uint64_t shtable_entry_get_offset(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_offset);
	case BITS_64:
		return (entry->data.s64.sh_offset);
	default:
		return (0);
	}
}

/**
 * @brief Get the section size from a section header entry.
 *
 * Returns the section's size in bytes. Unless the section type is
 * SHT_NOBITS, the section occupies sh_size bytes in the file.
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section size in bytes (sh_size field)
 */
uint64_t shtable_entry_get_size(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_size);
	case BITS_64:
		return (entry->data.s64.sh_size);
	default:
		return (0);
	}
}

/**
 * @brief Get the section link from a section header entry.
 *
 * This member holds a section header table index link, whose interpretation
 * depends on the section type.
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section link (sh_link field)
 */
uint32_t shtable_entry_get_link(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_link);
	case BITS_64:
		return (entry->data.s64.sh_link);
	default:
		return (0);
	}
}

/**
 * @brief Get the section entry size from a section header entry.
 *
 * Some sections hold a table of fixed-size entries, such as a symbol table.
 * For such a section, this member gives the size in bytes of each entry.
 * The member contains 0 if the section does not hold a table of fixed-size
 * entries.
 *
 * @param entry Pointer to the SectionHeaderTableEntry
 * @return The section entry size in bytes (sh_entsize field)
 */
uint64_t shtable_entry_get_entsize(SectionHeaderTableEntry *entry) {
	switch (entry->type) {
	case BITS_32:
		return (entry->data.s32.sh_entsize);
	case BITS_64:
		return (entry->data.s64.sh_entsize);
	default:
		return (0);
	}
}
