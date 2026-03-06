#include "my_nm.h"

int shdrt_create_from_obj(ObjectFile *obj_file, ElfHeader *elf_hdr, SectionHeaderTable *table) {
	uint64_t shoff;
	size_t   section_header_table_size;

	if (obj_file == NULL || obj_file->data == NULL) {
		return (-1);
	}

	shoff = ehdr_get_shoff(elf_hdr);
	if (shoff == 0) {
		return (-1);
	}

	table->arch = ehdr_get_arch_type(elf_hdr);
	table->header_cursor = 0;

	section_header_table_size = ehdr_get_shentsize(elf_hdr) * ehdr_get_shnum(elf_hdr);

	if (shoff + section_header_table_size > obj_file->size) {
		return (-1);
	}

	table->table = (char *) malloc(section_header_table_size);
	if (table->table == NULL) {
		return (-1);
	}

	memcpy(table->table, obj_file->data + shoff, section_header_table_size);
	return (0);
}

void shdrt_free(SectionHeaderTable *table) {
	free(table->table);
	table->table = NULL;
}

// void shdrt_next_section_header(SHT_SYMTAB |) SHT_SYMTAB
