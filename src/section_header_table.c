#include "my_nm.h"

int sh_table_parse(ElfFile *elf) {
	uint64_t shoff;
	size_t   table_size;

	/* check if section table exists */
	shoff = elf_hdr_get_shoff(elf);
	if (shoff == 0) {
		elf->sh_table.table = NULL;
		return (0);
	}

	elf->sh_table.entry_size = elf_hdr_get_shentsize(elf);
	elf->sh_table.entry_count = elf_hdr_get_shnum(elf);
	elf->sh_table.current_index = 0;

	table_size = elf->sh_table.entry_size * elf->sh_table.entry_count;

	if (shoff + table_size > elf->file_data.size) {
		return (-1);
	}

	elf->sh_table.table = (char *) malloc(table_size);
	if (elf->sh_table.table == NULL) {
		return (-1);
	}

	memcpy(elf->sh_table.table, elf->file_data.data + shoff, table_size);
	return (0);
}

/**
 * @brief Free memory allocated for a section header table.
 *
 * Releases the dynamically allocated memory used to store the section
 * header table and sets the table pointer to NULL.
 *
 * @param tbl Pointer to the SectionHeaderTable structure to free
 */
void sh_table_free(SectionHeaderTable *tbl) {
	if (tbl->table != NULL) {
		free(tbl->table);
	}
	memset(tbl, 0, sizeof(SectionHeaderTable));
}

/* Table Iteration */

/**
 * @brief Get the current section header.
 *
 * @param elf Pointer to ElfFile
 * @return Pointer to current section header (Elf32_Shdr* or Elf64_Shdr*), or NULL
 */
void *sh_table_get_current(ElfFile *elf) {
	if (elf->sh_table.table == NULL) {
		return (NULL);
	}
	if (elf->sh_table.current_index >= elf->sh_table.entry_count) {
		return (NULL);
	}
	return (elf->sh_table.table + (elf->sh_table.current_index * elf->sh_table.entry_size));
}

/**
 * @brief Get a section header at a specific index.
 *
 * @param elf Pointer to ElfFile
 * @param index Index of the section header
 * @return Pointer to section header (Elf32_Shdr* or Elf64_Shdr*), or NULL
 */
void *sh_table_get_at(ElfFile *elf, size_t index) {
	if (elf->sh_table.table == NULL) {
		return (NULL);
	}
	if (index >= elf->sh_table.entry_count) {
		return (NULL);
	}
	return (elf->sh_table.table + (index * elf->sh_table.entry_size));
}

/**
 * @brief Move to the next section header.
 *
 * @param elf Pointer to ElfFile
 * @return 0 if moved to next, -1 if already at end
 */
int sh_table_next(ElfFile *elf) {
	if (elf->sh_table.current_index >= elf->sh_table.entry_count) {
		return (-1);
	}
	elf->sh_table.current_index++;
	return (0);
}

/**
 * @brief Reset the section header iterator to the beginning.
 *
 * @param elf Pointer to ElfFile
 */
void sh_table_reset(ElfFile *elf) {
	elf->sh_table.current_index = 0;
}

/**
 * @brief Check if there are more section headers to iterate over.
 *
 * @param elf Pointer to ElfFile
 * @return 1 if there are more headers, 0 otherwise
 */
int sh_table_has_more(ElfFile *elf) {
	if (elf->sh_table.table == NULL) {
		return (0);
	}
	return (elf->sh_table.current_index < elf->sh_table.entry_count);
}
