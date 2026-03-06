#include "my_nm.h"
#include <elf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void ehdr_debug_print_32(ElfHeader *header);
static void ehdr_debug_print_64(ElfHeader *header);
static int  ehdr_validate_header_start(char *elf_hdr_start);

/**
 * @brief Parse an ELF header from a memory-mapped file.
 *
 * Reads the ELF header from the memory-mapped file data
 * and stores it in the provided header buffer, accounting for 32/64 bit difference.
 *
 * @param obj_file Pointer to ObjectFile containing mmap'd data
 * @param header Pointer to buffer where the ELF header will be stored
 *
 * @return 0 on success, -1 on failure
 *
 * @see ElfHeader
 * @see ObjectFile
 */
int ehdr_parse_from_obj(ObjectFile *obj_file, ElfHeader *header) {
	unsigned char *elf_hdr_start;
	size_t         header_size;

	if (obj_file == NULL || obj_file->data == NULL || obj_file->size < EI_NIDENT) {
		return (-1);
	}

	elf_hdr_start = (unsigned char *) obj_file->data;

	if (ehdr_validate_header_start((char *) elf_hdr_start) != 0) {
		return (-1);
	}

	switch (elf_hdr_start[EI_CLASS]) {
	case (ELFDATANONE):
		return (-1);
	case (ELFCLASS32):
		header_size = sizeof(Elf32_Ehdr);
		break;
	case (ELFCLASS64):
		header_size = sizeof(Elf64_Ehdr);
		break;
	default:
		return (-1);
	}

	if (obj_file->size < header_size) {
		return (-1);
	}

	memcpy(header->data, obj_file->data, header_size);
	return (0);
}

static int ehdr_validate_header_start(char *elf_hdr_start) {
	return (strcmp(elf_hdr_start, ELFMAG) == 0);
}

/**
 * @brief Get the architecture type (bitness) from an ELF header.
 *
 * Determines whether the ELF file is 32-bit or 64-bit.
 *
 * @param header ElfHeader
 *
 * @return The architecture type:
 *         - BITS_32 if the ELF file is 32-bit
 *         - BITS_64 if the ELF file is 64-bit
 *         - -1 if the class field is invalid or unrecognized
 *
 * @see ElfHeader
 * @see ELFCLASS32
 * @see ELFCLASS64
 */
ArchType ehdr_get_arch_type(ElfHeader *header) {
	switch (header->data[EI_CLASS]) {
	case (ELFCLASS32):
		return (BITS_32);
	case (ELFCLASS64):
		return (BITS_64);
	default:
		return (-1);
	}
}

/**
 * @brief Print debug information about an ELF header.
 *
 * Displays detailed information about the ELF header to stdout, including
 * type, machine architecture, version, entry point, offsets, and various
 * header counts. The output format is automatically adjusted based on
 * whether the header is 32-bit or 64-bit.
 *
 * @param header Pointer to the ELF header to print
 *
 * @note This function does nothing if the header has an invalid architecture type
 * @see hdr_get_arch_type
 * @see ElfHeader
 */
void ehdr_debug_print(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		ehdr_debug_print_32(header);
		return;
	case (BITS_64):
		ehdr_debug_print_64(header);
		return;
	}
}

static void ehdr_debug_print_32(ElfHeader *header) {
	Elf32_Ehdr typed_header;

	memcpy(&typed_header, header->data, sizeof(Elf32_Ehdr));
	printf("ELF Header (32-bit):\n");
	printf("  Type: %u\n", typed_header.e_type);
	printf("  Machine: %u\n", typed_header.e_machine);
	printf("  Version: %u\n", typed_header.e_version);
	printf("  Entry point: 0x%x\n", typed_header.e_entry);
	printf("  Program header offset: %u\n", typed_header.e_phoff);
	printf("  Section header offset: %u\n", typed_header.e_shoff);
	printf("  Flags: 0x%x\n", typed_header.e_flags);
	printf("  ELF header size: %u\n", typed_header.e_ehsize);
	printf("  Program header entry size: %u\n", typed_header.e_phentsize);
	printf("  Program header count: %u\n", typed_header.e_phnum);
	printf("  Section header entry size: %u\n", typed_header.e_shentsize);
	printf("  Section header count: %u\n", typed_header.e_shnum);
	printf("  Section header string table index: %u\n", typed_header.e_shstrndx);
}

static void ehdr_debug_print_64(ElfHeader *header) {
	Elf64_Ehdr typed_header;

	memcpy(&typed_header, header->data, sizeof(Elf64_Ehdr));
	printf("ELF Header (64-bit):\n");
	printf("  Type: %u\n", typed_header.e_type);
	printf("  Machine: %u\n", typed_header.e_machine);
	printf("  Version: %u\n", typed_header.e_version);
	printf("  Entry point: 0x%lx\n", typed_header.e_entry);
	printf("  Program header offset: %lu\n", typed_header.e_phoff);
	printf("  Section header offset: %lu\n", typed_header.e_shoff);
	printf("  Flags: 0x%x\n", typed_header.e_flags);
	printf("  ELF header size: %u\n", typed_header.e_ehsize);
	printf("  Program header entry size: %u\n", typed_header.e_phentsize);
	printf("  Program header count: %u\n", typed_header.e_phnum);
	printf("  Section header entry size: %u\n", typed_header.e_shentsize);
	printf("  Section header count: %u\n", typed_header.e_shnum);
	printf("  Section header string table index: %u\n", typed_header.e_shstrndx);
}

/**
 * @brief Get the object file type from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The object file type (e_type field)
 */
uint16_t ehdr_get_type(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_type;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_type;
	default:
		return (0);
	}
}

/**
 * @brief Get the machine architecture from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The machine architecture (e_machine field)
 */
uint16_t ehdr_get_machine(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_machine;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_machine;
	default:
		return (0);
	}
}

/**
 * @brief Get the object file version from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The object file version (e_version field)
 */
uint32_t ehdr_get_version(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_version;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_version;
	default:
		return (0);
	}
}

/**
 * @brief Get the entry point address from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The entry point virtual address (e_entry field)
 */
uint64_t ehdr_get_entry(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_entry;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_entry;
	default:
		return (0);
	}
}

/**
 * @brief Get the program header table offset from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The program header table file offset (e_phoff field)
 */
uint64_t ehdr_get_phoff(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_phoff;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_phoff;
	default:
		return (0);
	}
}

/**
 * @brief Get the section header table offset from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The section header table file offset (e_shoff field)
 */
uint64_t ehdr_get_shoff(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_shoff;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_shoff;
	default:
		return (0);
	}
}

/**
 * @brief Get the processor-specific flags from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The processor-specific flags (e_flags field)
 */
uint32_t ehdr_get_flags(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_flags;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_flags;
	default:
		return (0);
	}
}

/**
 * @brief Get the ELF header size from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The ELF header size in bytes (e_ehsize field)
 */
uint16_t ehdr_get_ehsize(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_ehsize;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_ehsize;
	default:
		return (0);
	}
}

/**
 * @brief Get the program header entry size from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The size of one program header entry (e_phentsize field)
 */
uint16_t ehdr_get_phentsize(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_phentsize;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_phentsize;
	default:
		return (0);
	}
}

/**
 * @brief Get the program header count from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The number of program header entries (e_phnum field)
 */
uint16_t ehdr_get_phnum(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_phnum;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_phnum;
	default:
		return (0);
	}
}

/**
 * @brief Get the section header entry size from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The size of one section header entry (e_shentsize field)
 */
uint16_t ehdr_get_shentsize(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_shentsize;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_shentsize;
	default:
		return (0);
	}
}

/**
 * @brief Get the section header count from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The number of section header entries (e_shnum field)
 */
uint16_t ehdr_get_shnum(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_shnum;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_shnum;
	default:
		return (0);
	}
}

/**
 * @brief Get the section header string table index from an ELF header.
 *
 * @param header Pointer to the ELF header
 * @return The section header table index of the string table (e_shstrndx field)
 */
uint16_t ehdr_get_shstrndx(ElfHeader *header) {
	switch (ehdr_get_arch_type(header)) {
	case (BITS_32):
		return ((Elf32_Ehdr *) header->data)->e_shstrndx;
	case (BITS_64):
		return ((Elf64_Ehdr *) header->data)->e_shstrndx;
	default:
		return (0);
	}
}
