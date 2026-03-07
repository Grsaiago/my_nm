#include "my_nm.h"

/**
 * @brief Load an ELF file from disk and parse its structure.
 *
 * This function opens the file, maps it into memory using mmap,
 * parses the ELF header, and initializes the section header table.
 *
 * @param path Path to the ELF file
 * @param elf Pointer to ElfFile structure to initialize
 *
 * @return 0 on success, -1 on failure
 *
 * @see elf_file_free
 */
int elf_file_load(char *path, ElfFile *elf) {
	void       *file_data;
	struct stat file_metadata;
	int         fd;

	memset(elf, 0, sizeof(ElfFile));

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		return (-1);
	}

	if (fstat(fd, &file_metadata) != 0) {
		close(fd);
		return (-1);
	}

	file_data = mmap(NULL, file_metadata.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);

	if (file_data == MAP_FAILED) {
		return (-1);
	}

	elf->file_data.data = file_data;
	elf->file_data.size = file_metadata.st_size;

	if (elf_hdr_parse(elf) != 0) {
		elf_file_free(elf);
		return (-1);
	}
	if (sh_table_parse(elf) != 0) {
		elf_file_free(elf);
		return (-1);
	}

	return (0);
}

/**
 * @brief Free resources associated with an ELF file.
 *
 * @param elf Pointer to ElfFile structure to free
 */
void elf_file_free(ElfFile *elf) {
	if (elf->file_data.data != NULL) {
		munmap(elf->file_data.data, elf->file_data.size);
		elf->file_data.data = NULL;
	}
	if (elf->sh_table.table != NULL) {
		sh_table_free(&elf->sh_table);
	}
}

/**
 * @brief Get the architecture type (bitness) from an ELF file.
 *
 * @param elf Pointer to ElfFile
 * @return BITS_32 for 32-bit, BITS_64 for 64-bit, -1 on error
 */
ArchType elf_get_arch_type(ElfFile *elf) {
	switch (elf->hdr.data[EI_CLASS]) {
	case ELFCLASS32:
		return (BITS_32);
	case ELFCLASS64:
		return (BITS_64);
	default:
		return (-1);
	}
}
