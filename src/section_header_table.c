#include "my_nm.h"

int sh_table_parse(ElfFile *elf) {
	uint64_t shoff;

	/* check if section table exists */
	shoff = elf_hdr_get_shoff(elf);
	if (shoff == 0) {
		return (-1);
	}

	elf->sh_table = (SectionHeaderTable){
		.entry_size = elf_hdr_get_shentsize(elf),
		.entry_count = elf_hdr_get_shnum(elf),
		.current_index = 0,
		.table_start_offset = shoff,
	};
	return (0);
}

/* Table Iteration */

/**
 * @brief Get the current section header.
 *
 * @param elf Pointer to ElfFile
 * @return Pointer to current section header (Elf32_Shdr* or Elf64_Shdr*), or
 * NULL
 */
void sh_table_get_current(ElfFile *elf, SectionHeaderTableEntry *entry) {
	sh_table_get_at(elf, elf->sh_table.current_index, entry);
	return;
}

/**
 * @brief Get a section header at a specific index.
 *
 * @param elf Pointer to ElfFile
 * @param index Index of the section header
 * @return Pointer to section header (Elf32_Shdr* or Elf64_Shdr*), or NULL
 */
void sh_table_get_at(ElfFile *elf, size_t index,
					 SectionHeaderTableEntry *entry) {
	switch (elf_get_arch_type(elf)) {
	case (BITS_32):
		entry->type = BITS_32;
		memcpy(&entry->data.s32,
			   (elf->file_data.data + elf_hdr_get_shoff(elf)) +
				   (index * elf->sh_table.entry_size),
			   sizeof(Elf32_Shdr));
		break;
	case (BITS_64):
		entry->type = BITS_64;
		memcpy(&entry->data.s64,
			   (elf->file_data.data + elf_hdr_get_shoff(elf)) +
				   (index * elf->sh_table.entry_size),
			   sizeof(Elf64_Shdr));
		break;
	}
	return;
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
void sh_table_reset(ElfFile *elf) { elf->sh_table.current_index = 0; }

/**
 * @brief Check if there are more section headers to iterate over.
 *
 * @param elf Pointer to ElfFile
 * @return 1 if there are more headers, 0 otherwise
 */
int sh_table_has_more(ElfFile *elf) {
	return (elf->sh_table.current_index < elf->sh_table.entry_count);
}

int sh_table_get_symtab_header(ElfFile *elf, SymbolTableHeader *symtab_header) {
	int						section_header_index;
	SectionHeaderTableEntry symtab_associated_strtab;
	SectionHeaderTableEntry curr_header;

	section_header_index = 0;
	for (; sh_table_has_more(elf); sh_table_next(elf), section_header_index++) {
		sh_table_get_current(elf, &curr_header);
		if (shtable_entry_get_type(&curr_header) != SHT_SYMTAB) {
			continue;
		}
		// get the strtab section header associated with that symtab via the
		// sh_link property as per docs and do a sanity check on it's type.
		sh_table_get_at(elf, shtable_entry_get_link(&curr_header),
						&symtab_associated_strtab);
		if (shtable_entry_get_type(&symtab_associated_strtab) != SHT_STRTAB) {
			printf("O header na posição %d (sh_link da symtab), não é uma "
				   "SHT_STRTAB",
				   shtable_entry_get_link(&curr_header));
			return (-1);
		}
		break;
	}
	symtab_header->val = curr_header;
	return (0);
}
