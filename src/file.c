#include "my_nm.h"
#include <sys/mman.h>
#include <sys/stat.h>

int map_file(const char *filename, MappedFile *file) {
	int			fd;
	char	   *elf_data;
	struct stat file_info;

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		ft_printf("error on opening file: %s", strerror(errno));
		return (-1);
	}

	if (fstat(fd, &file_info) != 0) {
		ft_printf("error on fstat: %s", strerror(errno));
		return (-1);
	}

	elf_data = mmap(NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (elf_data == NULL) {
		ft_printf("error on mmap: %s", strerror(errno));
		return (-1);
	}

	*file = (MappedFile){.data = elf_data, .size = file_info.st_size};
	close(fd);
	return (0);
}

void unmap_file(MappedFile *file) {
	if (file->data != NULL) {
		munmap(file->data, file->size);
	}
	return;
}
