#include "my_nm.h"
#include <criterion/internal/test.h>

const char *filepath_32_bits = "./tests/test_artifacts/32_bit_executable";

Test(header_parsing_test, parse_32_header) {
	int     result;
	ElfFile elf;

	result = elf_file_load((char *) filepath_32_bits, &elf);
	cr_assert(result == 0, "failed to load ELF file");

	result = elf_get_arch_type(&elf);
	cr_assert(result == BITS_32,
	          "parsed header should be "
	          "32 bits, but it's %d",
	          result);

	elf_file_free(&elf);
}
