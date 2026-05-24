#include "my_nm.h"

StringTable get_string_table(ElfFile *elf, StringTableHeader *header) {
	switch (header->val.type) {
	case (BITS_32):
		return (StringTable){
			.data = &elf->data[header->val.data.s32.sh_offset],
			.size = header->val.data.s32.sh_size,
		};
	case (BITS_64):
		return (StringTable){
			.data = &elf->data[header->val.data.s64.sh_offset],
			.size = header->val.data.s64.sh_size,
		};
	default:
		return ((StringTable){0});
	}
}
