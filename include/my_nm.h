#ifndef MY_NM
# define MY_NM

# include <elf.h>
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <fcntl.h>
# include <stdlib.h>
# include <sys/mman.h>
# include <sys/stat.h>


typedef enum arch_type {
    BITS_32,
    BITS_64,
}   ArchType;


typedef struct object_file {
    char *data;
    size_t size;
}   ObjectFile;

typedef struct elf_header {
    char data[sizeof(Elf64_Ehdr)];
}   ElfHeader;

typedef struct section_header_table {
    ArchType    arch;
    char        *table;
    char        *header_cursor;
}   SectionHeaderTable;

/* object file manipulation */
int load_object_file(char *name, ObjectFile *obj);
void free_object_file(ObjectFile *obj);

/* elf header manipulation */
int         ehdr_parse_from_obj(ObjectFile *obj_file, ElfHeader *header);
ArchType    ehdr_get_arch_type(ElfHeader *header);
void        ehdr_debug_print(ElfHeader *header);

/* elf header getters */
uint16_t    ehdr_get_type(ElfHeader *header);
uint16_t    ehdr_get_machine(ElfHeader *header);
uint32_t    ehdr_get_version(ElfHeader *header);
uint64_t    ehdr_get_entry(ElfHeader *header);
uint64_t    ehdr_get_phoff(ElfHeader *header);
uint64_t    ehdr_get_shoff(ElfHeader *header);
uint32_t    ehdr_get_flags(ElfHeader *header);
uint16_t    ehdr_get_ehsize(ElfHeader *header);
uint16_t    ehdr_get_phentsize(ElfHeader *header);
uint16_t    ehdr_get_phnum(ElfHeader *header);
uint16_t    ehdr_get_shentsize(ElfHeader *header);
uint16_t    ehdr_get_shnum(ElfHeader *header);
uint16_t    ehdr_get_shstrndx(ElfHeader *header);

/* section headers */
int shdrt_create_from_obj(ObjectFile *obj_file, ElfHeader *elf_hdr, SectionHeaderTable *table);
void shdrt_free(SectionHeaderTable *table);

#endif // MY_NM
