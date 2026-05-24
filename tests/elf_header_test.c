#include "my_nm.h"
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>

const char *filepath_32_bits = "./tests/test_artifacts/32_bit_executable";
const char *filepath_64_bits = "./tests/test_artifacts/64_shared_libft.so";

Test(header_parsing_test, parse_32_header) {
	ElfFile	   elf;
	MappedFile file;
	int		   result;

	result = map_file(filepath_32_bits, &file);
	cr_assert(result == 0, "return from map_file should be 0");
	result = new_elf_file(&file, &elf);
	cr_assert(result == 0, "failed to load ELF file");
	cr_assert(elf.type == BITS_32,
			  "parsed header should be "
			  "32 bits, but it's %d",
			  elf.type);

	elf_file_free(&elf);
}

Test(header_parsing_test, parse_64_header) {
	int		   result;
	ElfFile	   elf;
	MappedFile file;

	result = map_file(filepath_64_bits, &file);
	cr_assert(result == 0, "return from map_file should be 0");
	result = new_elf_file(&file, &elf);
	cr_assert(result == 0, "failed to load ELF file");
	cr_assert(elf.type == BITS_64,
			  "parsed header should be "
			  "64 bits, but it's %d",
			  elf.type);

	elf_file_free(&elf);
}
