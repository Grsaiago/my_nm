#include "my_nm.h"
#include <criterion/internal/test.h>
#include <fcntl.h>

const char *filepath_32_bits = "./tests/test_artifacts/32_bit_executable";

Test(header_parsing_test, parse_32_header) {
	int        result;
	ElfHeader  header;
	ObjectFile obj_file;

	result = load_object_file((char *) filepath_32_bits, &obj_file);
	cr_assert(result == 0, "failed to load object file");

	result = ehdr_parse_from_obj(&obj_file, &header);
	cr_assert(result == 0, "failed to parse the elf "
	                       "header from the 32bit test "
	                       "file");
	result = ehdr_get_arch_type(&header);
	cr_assert(result == BITS_32,
	          "parsed header should be "
	          "32 bits, but it's %d",
	          result);

	free_object_file(&obj_file);
}
