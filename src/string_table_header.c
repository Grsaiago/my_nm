#include "my_nm.h"

uint64_t strtab_header_get_data_offset(StringTableHeader *header) {
	return (shtable_entry_get_offset(&header->val));
}

const char *strtab_header_get_string_at(ElfFile *elf, StringTableHeader *header,
										uint32_t name_index) {
	return (elf->file_data.data + strtab_header_get_data_offset(header) +
			name_index);
}
