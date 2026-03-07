#include "my_nm.h"

static int  elf_hdr_validate_magic(char *data);
static void elf_hdr_debug_print_32(ElfFile *elf);
static void elf_hdr_debug_print_64(ElfFile *elf);

int elf_hdr_parse(ElfFile *elf) {
	unsigned char *elf_hdr_start;
	size_t         header_size;

	if (elf->file_data.size < EI_NIDENT) {
		return (-1);
	}

	elf_hdr_start = (unsigned char *) elf->file_data.data;

	if (elf_hdr_validate_magic((char *) elf_hdr_start) != 0) {
		return (-1);
	}

	switch (elf_hdr_start[EI_CLASS]) {
	case ELFCLASS32:
		header_size = sizeof(Elf32_Ehdr);
		break;
	case ELFCLASS64:
		header_size = sizeof(Elf64_Ehdr);
		break;
	default:
		return (-1);
	}

	if (elf->file_data.size < header_size) {
		return (-1);
	}

	memcpy(elf->hdr.data, elf->file_data.data, header_size);
	return (0);
}

static int elf_hdr_validate_magic(char *data) {
	return (memcmp(data, ELFMAG, SELFMAG) == 0 ? 0 : -1);
}

/**
 * @brief Print debug information about the ELF header.
 *
 * @param elf Pointer to ElfFile
 */
void elf_hdr_debug_print(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		elf_hdr_debug_print_32(elf);
		return;
	case BITS_64:
		elf_hdr_debug_print_64(elf);
		return;
	}
}

static void elf_hdr_debug_print_32(ElfFile *elf) {
	Elf32_Ehdr *hdr = (Elf32_Ehdr *) elf->hdr.data;

	printf("ELF Header (32-bit):\n");
	printf("  Type: %u\n", hdr->e_type);
	printf("  Machine: %u\n", hdr->e_machine);
	printf("  Version: %u\n", hdr->e_version);
	printf("  Entry point: 0x%x\n", hdr->e_entry);
	printf("  Program header offset: %u\n", hdr->e_phoff);
	printf("  Section header offset: %u\n", hdr->e_shoff);
	printf("  Flags: 0x%x\n", hdr->e_flags);
	printf("  ELF header size: %u\n", hdr->e_ehsize);
	printf("  Program header entry size: %u\n", hdr->e_phentsize);
	printf("  Program header count: %u\n", hdr->e_phnum);
	printf("  Section header entry size: %u\n", hdr->e_shentsize);
	printf("  Section header count: %u\n", hdr->e_shnum);
	printf("  Section header string table index: %u\n", hdr->e_shstrndx);
}

static void elf_hdr_debug_print_64(ElfFile *elf) {
	Elf64_Ehdr *hdr = (Elf64_Ehdr *) elf->hdr.data;

	printf("ELF Header (64-bit):\n");
	printf("  Type: %u\n", hdr->e_type);
	printf("  Machine: %u\n", hdr->e_machine);
	printf("  Version: %u\n", hdr->e_version);
	printf("  Entry point: 0x%lx\n", hdr->e_entry);
	printf("  Program header offset: %lu\n", hdr->e_phoff);
	printf("  Section header offset: %lu\n", hdr->e_shoff);
	printf("  Flags: 0x%x\n", hdr->e_flags);
	printf("  ELF header size: %u\n", hdr->e_ehsize);
	printf("  Program header entry size: %u\n", hdr->e_phentsize);
	printf("  Program header count: %u\n", hdr->e_phnum);
	printf("  Section header entry size: %u\n", hdr->e_shentsize);
	printf("  Section header count: %u\n", hdr->e_shnum);
	printf("  Section header string table index: %u\n", hdr->e_shstrndx);
}

/* getters */
uint16_t elf_hdr_get_type(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_type;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_type;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_machine(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_machine;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_machine;
	default:
		return (0);
	}
}

uint32_t elf_hdr_version(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_version;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_version;
	default:
		return (0);
	}
}

uint64_t elf_hdr_get_entry(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_entry;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_entry;
	default:
		return (0);
	}
}

uint64_t elf_hdr_get_phoff(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_phoff;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_phoff;
	default:
		return (0);
	}
}

uint64_t elf_hdr_get_shoff(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_shoff;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_shoff;
	default:
		return (0);
	}
}

uint32_t elf_hdr_get_flags(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_flags;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_flags;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_ehsize(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_ehsize;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_ehsize;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_phentsize(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_phentsize;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_phentsize;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_phnum(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_phnum;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_phnum;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_shentsize(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_shentsize;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_shentsize;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_shnum(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_shnum;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_shnum;
	default:
		return (0);
	}
}

uint16_t elf_hdr_get_shstrndx(ElfFile *elf) {
	switch (elf_get_arch_type(elf)) {
	case BITS_32:
		return ((Elf32_Ehdr *) elf->hdr.data)->e_shstrndx;
	case BITS_64:
		return ((Elf64_Ehdr *) elf->hdr.data)->e_shstrndx;
	default:
		return (0);
	}
}
