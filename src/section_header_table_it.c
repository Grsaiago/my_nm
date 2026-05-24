#include "my_nm.h"

static int sh_table_it_get_symtab_header(SectionHeaderTableIterator *it,
										 SymbolTableHeader			*symtab);

inline SectionHeaderTableIterator get_section_header_table_it(ElfFile *elf) {
	switch (elf->type) {
	case (BITS_32):
		return (SectionHeaderTableIterator){.type = BITS_32,
											.idx = -1,
											.size = elf_get_shnum(elf),
											.start =
												&elf->data[elf_get_shoff(elf)]};
	case (BITS_64):
		return (SectionHeaderTableIterator){
			.type = BITS_64,
			.idx = -1,
			.size = elf_get_shnum(elf),
			.start = &elf->data[elf_get_shoff(elf)],
		};
	}
}

inline int32_t sh_table_it_reset(SectionHeaderTableIterator *it) {
	uint32_t prev_idx;

	prev_idx = it->idx;
	it->idx = -1;
	return (prev_idx);
}

inline int sh_table_it_has_next(SectionHeaderTableIterator *it) {
	return (it->idx + 1 < (int)it->size);
}

int sh_table_it_next(SectionHeaderTableIterator *it,
					 SectionHeaderTableEntry	*value) {
	if (it->idx >= (int)it->size) {
		return (-1);
	}

	it->idx++;
	switch (it->type) {
	case (BITS_32):
		*value = (SectionHeaderTableEntry){
			.type = it->type,
			.data.s32 = ((Elf32_Shdr *)it->start)[it->idx],
		};
		break;
	case (BITS_64):
		*value = (SectionHeaderTableEntry){
			.type = it->type,
			.data.s64 = ((Elf64_Shdr *)it->start)[it->idx],
		};
		break;
	}
	return (0);
}

int sh_table_it_get_at(SectionHeaderTableIterator *it, uint32_t index,
					   SectionHeaderTableEntry *entry) {
	if (it->size <= index) {
		return (-1);
	}

	switch (it->type) {
	case (BITS_32):
		*entry = (SectionHeaderTableEntry){
			.type = BITS_32,
			.data.s32 = ((Elf32_Shdr *)it->start)[index],
		};
		break;
	case (BITS_64):
		*entry = (SectionHeaderTableEntry){
			.type = BITS_64,
			.data.s64 = ((Elf64_Shdr *)it->start)[index],
		};
		break;
	}
	return (0);
}

// return values:
//     0: success
//     1: symbol table header not found
//     2: string table header associated with symbol header table not found
int sh_table_it_get_symbol_and_associated_string_table(
	SectionHeaderTableIterator *it, SymbolTableHeader *symtab,
	StringTableHeader *strtab) {

	uint32_t string_table_index;

	if (sh_table_it_get_symtab_header(it, symtab) != 0) {
		return (1);
	}

	string_table_index = symtab_header_get_strtab_index(symtab);
	if (string_table_index == 0) {
		return (2);
	}
	if (sh_table_it_get_at(it, string_table_index, &strtab->val) != 0) {
		return (2);
	}
	return (0);
}

static int sh_table_it_get_symtab_header(SectionHeaderTableIterator *it,
										 SymbolTableHeader			*symtab) {
	SectionHeaderTableEntry curr_header;
	uint32_t				save_idx;

	save_idx = sh_table_it_reset(it);
	while (sh_table_it_has_next(it)) {
		sh_table_it_next(it, &curr_header);

		if (shtable_entry_get_type(&curr_header) != SHT_SYMTAB) {
			continue;
		}
		symtab->val = curr_header;
		it->idx = save_idx;
		return (0);
	}
	return (-1);
}
