#include "my_nm.h"
#include <elf.h>
#include <sys/mman.h>

int new_elf_file(MappedFile *file, ElfFile *elf) {
	if (memcmp(file->data, ELFMAG, SELFMAG) != 0) {
		return (-1);
	}
	switch (file->data[EI_CLASS]) {
	case ELFCLASS32:
		elf->type = BITS_32;
		memcpy(&elf->header, file->data, sizeof(Elf32_Ehdr));
		break;
	case ELFCLASS64:
		elf->type = BITS_64;
		memcpy(&elf->header, file->data, sizeof(Elf64_Ehdr));
		break;
	case ELFCLASSNONE:
	default:
		printf("Failed to identify the architecture (32 or 64)\n");
		return (-1);
	}
	elf->data = file->data;
	elf->size = file->size;
	return (0);
}

void elf_file_free(ElfFile *elf) {
	if (elf->data != NULL) {
		munmap(elf->data, elf->size);
		elf->data = NULL;
	}
}

int elf_get_shnum(ElfFile *elf) {
	switch (elf->type) {
	case (BITS_32):
		return elf->header.t32.e_shnum;
	case (BITS_64):
		return elf->header.t64.e_shnum;
		break;
	}
}

uint64_t elf_get_shoff(ElfFile *elf) {
	switch (elf->type) {
	case BITS_32:
		return (elf->header.t32.e_shoff);
	case BITS_64:
		return (elf->header.t64.e_shoff);
	default:
		return (0);
	}
}
