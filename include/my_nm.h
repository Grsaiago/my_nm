#ifndef MY_NM
# define MY_NM

# include <elf.h>
#include <stddef.h>
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

typedef struct symbol_header_table_iterator {
    char *table;
    size_t  entry_size;
    size_t  entry_count;
    size_t  current_index;
}   SectionHeaderTable;

typedef struct elf_header {
    char data[sizeof(Elf64_Ehdr)];
}   ElfHeader;

typedef struct elf_file {
    ObjectFile  file_data;
    ElfHeader   hdr;
    SectionHeaderTable sh_table;
}   ElfFile;

/* ElfFile */
int         elf_file_load(char *path, ElfFile *elf);
void        elf_file_free(ElfFile *elf);
ArchType    elf_get_arch_type(ElfFile *elf);

/* ELF header */
int         elf_hdr_parse(ElfFile *elf);
void        elf_hdr_debug_print(ElfFile *elf);

/* ELF header getters */
uint16_t    elf_hdr_get_type(ElfFile *elf);
uint16_t    elf_hdr_get_machine(ElfFile *elf);
uint32_t    elf_hdr_version(ElfFile *elf);
uint64_t    elf_hdr_get_entry(ElfFile *elf);
uint64_t    elf_hdr_get_phoff(ElfFile *elf);
uint64_t    elf_hdr_get_shoff(ElfFile *elf);
uint32_t    elf_hdr_get_flags(ElfFile *elf);
uint16_t    elf_hdr_get_ehsize(ElfFile *elf);
uint16_t    elf_hdr_get_phentsize(ElfFile *elf);
uint16_t    elf_hdr_get_phnum(ElfFile *elf);
uint16_t    elf_hdr_get_shentsize(ElfFile *elf);
uint16_t    elf_hdr_get_shnum(ElfFile *elf);
uint16_t    elf_hdr_get_shstrndx(ElfFile *elf);

/* Section header iteration */
int         sh_table_parse(ElfFile *elf);
void        sh_table_free(SectionHeaderTable *tbl);
void*       sh_table_get_current(ElfFile *elf);
void*       sh_table_get_at(ElfFile *elf, size_t index);
int         sh_table_next(ElfFile *elf);
void        sh_table_reset(ElfFile *elf);
int         sh_table_has_more(ElfFile *elf);

#endif // MY_NM
