#include "my_nm.h"
#include <sys/mman.h>

int load_object_file(char *name, ObjectFile *obj) {
	void       *obj_data;
	struct stat file_metadata;
	int         fd;

	fd = open(name, O_RDONLY);
	if (fd < 0) {
		return (-1);
	}
	if (fstat(fd, &file_metadata) != 0) {
		return (-1);
	}

	obj_data = mmap(NULL, file_metadata.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (obj_data == MAP_FAILED) {
		return (-1);
	}

	*obj = (ObjectFile) {
	    .data = obj_data,
	    .size = file_metadata.st_size,
	};
	return (0);
}

void free_object_file(ObjectFile *obj) {
	if (obj->data != NULL) {
		munmap(obj->data, obj->size);
	}
	return;
}
