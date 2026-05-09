#include "my_nm.h"

uint32_t symtab_header_get_strtab_index(SymbolTableHeader *header) {
	return (shtable_entry_get_link(&header->val));
}

uint64_t symtab_header_get_offset(SymbolTableHeader *header) {
	return (shtable_entry_get_offset(&header->val));
}

uint64_t symtab_header_get_size(SymbolTableHeader *header) {
	return (shtable_entry_get_size(&header->val));
}

uint64_t symtab_header_get_entsize(SymbolTableHeader *header) {
	return (shtable_entry_get_entsize(&header->val));
}
