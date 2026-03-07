#include "my_nm.h"

int main(int argc, char **argv) {
	ElfFile elf;
	int     result;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
		return (1);
	}

	/* Load the ELF file - this opens, mmaps, and parses everything */
	result = elf_file_load(argv[1], &elf);
	if (result != 0) {
		fprintf(stderr, "Error: Failed to load ELF file '%s'\n", argv[1]);
		return (1);
	}

	/* Print ELF header information */
	elf_hdr_debug_print(&elf);
	printf("\n");

	/* Example: Access specific header fields */
	printf("Additional info:\n");
	printf("  Architecture: %s\n",
	       elf_get_arch_type(&elf) == BITS_32 ? "32-bit" : "64-bit");
	printf("  Entry point: 0x%lx\n", elf_hdr_get_entry(&elf));
	printf("  Number of sections: %u\n", elf_hdr_get_shnum(&elf));
	printf("\n");

	/* Example: Iterate through section headers */
	printf("Section Headers:\n");
	sh_table_reset(&elf);

	size_t index = 0;
	while (sh_table_has_more(&elf)) {
		void *shdr = sh_table_get_current(&elf);

		if (elf_get_arch_type(&elf) == BITS_32) {
			Elf32_Shdr *sh = (Elf32_Shdr *) shdr;
			printf("  [%2zu] type=%-8u size=%-8u offset=%-8u\n", index, sh->sh_type, sh->sh_size, sh->sh_offset);
		} else {
			Elf64_Shdr *sh = (Elf64_Shdr *) shdr;
			printf("  [%2zu] type=%-8u size=%-8lu offset=%-8lu\n", index, sh->sh_type, sh->sh_size, sh->sh_offset);
		}

		sh_table_next(&elf);
		index++;
	}
	printf("\n");

	/* Example: Access a specific section by index */
	void *first_section = sh_table_get_at(&elf, 0);
	if (first_section != NULL) {
		printf("First section (index 0) accessed directly\n");
	}

	/* Clean up - unmaps memory and frees allocated resources */
	elf_file_free(&elf);

	return (0);
}
