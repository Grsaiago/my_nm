#include "my_nm.h"
#include <elf.h>

inline SymbolTableIterator get_symbol_table_it(ElfFile			 *elf,
											   SymbolTableHeader *header) {
	switch (elf->type) {
	case (BITS_32):
		return (SymbolTableIterator){
			.type = BITS_32,
			.idx = -1,
			.size = symtab_header_get_size(header) /
					symtab_header_get_entsize(header),
			.start = &elf->data[symtab_header_get_offset(header)]};
	case (BITS_64):
		return (SymbolTableIterator){
			.type = BITS_64,
			.idx = -1,
			.size = symtab_header_get_size(header) /
					symtab_header_get_entsize(header),
			.start = &elf->data[symtab_header_get_offset(header)]};
	};
}

inline int symbol_table_it_has_next(SymbolTableIterator *it) {
	return (it->idx + 1 < (int)it->size);
}

int symbol_table_it_next(SymbolTableIterator *it, SymbolTableEntry *value) {
	if (it->idx >= (int)it->size) {
		return (-1);
	}

	it->idx++;
	switch (it->type) {
	case (BITS_32):
		*value = (SymbolTableEntry){
			.type = it->type,
			.data.s32 = ((Elf32_Sym *)it->start)[it->idx],
		};
		break;
	case (BITS_64):
		*value = (SymbolTableEntry){
			.type = it->type,
			.data.s64 = ((Elf64_Sym *)it->start)[it->idx],
		};
		break;
	}
	return (0);
}
