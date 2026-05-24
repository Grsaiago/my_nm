#ifndef MY_NM
#define MY_NM

#include "libft.h"

#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef enum arch_type {
	BITS_32,
	BITS_64,
} ArchType;

/* File */
typedef struct {
	char		 *data;
	unsigned long size;
} MappedFile;

typedef struct {
	ArchType type;
	union {
		Elf32_Ehdr t32;
		Elf64_Ehdr t64;
	} header;
	char		 *data;
	unsigned long size;
} ElfFile;

/* Section Header */
typedef struct {
	ArchType type;
	char	*start;
	int32_t	 idx;
	uint32_t size;
} SectionHeaderTableIterator;

typedef struct {
	ArchType type;
	union {
		Elf32_Shdr s32;
		Elf64_Shdr s64;
	} data;
} SectionHeaderTableEntry;

typedef struct {
	SectionHeaderTableEntry val;
} SymbolTableHeader;

typedef struct {
	SectionHeaderTableEntry val;
} StringTableHeader;

/* Symbol Table */
typedef struct {
	ArchType type;
	char	*start;
	int32_t	 idx;
	uint32_t size;
} SymbolTableIterator;

typedef struct {
	ArchType type;
	union {
		Elf32_Sym s32;
		Elf64_Sym s64;
	} data;
} SymbolTableEntry;

/* String Table */
typedef struct {
	char	*data;
	uint32_t size;
} StringTable;

/* Symbol List */
typedef struct symbol_list {
	unsigned int		value;
	char			   *name;
	char				digit;
	struct symbol_list *next;
} SymbolList;

/* File load */
int	 map_file(const char *filename, MappedFile *file);
void unmap_file(MappedFile *file);

/* ElfFile */
int		 new_elf_file(MappedFile *file, ElfFile *elf);
void	 elf_file_free(ElfFile *elf);
uint64_t elf_get_shoff(ElfFile *elf);
int		 elf_get_shnum(ElfFile *elf);

/* SectionHeaderTableIterator */
SectionHeaderTableIterator get_section_header_table_it(ElfFile *elf);
int32_t					   sh_table_it_reset(SectionHeaderTableIterator *it);
int						   sh_table_it_has_next(SectionHeaderTableIterator *it);
int						   sh_table_it_next(SectionHeaderTableIterator *it,
											SectionHeaderTableEntry	   *value);
int sh_table_it_get_at(SectionHeaderTableIterator *it, uint32_t index,
					   SectionHeaderTableEntry *entry);
int sh_table_it_get_symbol_and_associated_string_table(
	SectionHeaderTableIterator *it, SymbolTableHeader *symtab,
	StringTableHeader *strtab);

/* Section header entry getters */
uint32_t shtable_entry_get_name(SectionHeaderTableEntry *entry);
uint32_t shtable_entry_get_type(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_flags(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_offset(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_size(SectionHeaderTableEntry *entry);
uint32_t shtable_entry_get_link(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_entsize(SectionHeaderTableEntry *entry);

/* SymbolTableHeader specialized functions */
uint32_t symtab_header_get_strtab_index(SymbolTableHeader *header);
uint64_t symtab_header_get_offset(SymbolTableHeader *header);
uint64_t symtab_header_get_size(SymbolTableHeader *header);
uint64_t symtab_header_get_entsize(SymbolTableHeader *header);

/* SymbolTableIterator */
SymbolTableIterator get_symbol_table_it(ElfFile			  *elf,
										SymbolTableHeader *symtab_header);
int symbol_table_it_next(SymbolTableIterator *it, SymbolTableEntry *value);
int symbol_table_it_has_next(SymbolTableIterator *it);

/* Symbol table entry getters */
uint32_t	  symtab_entry_get_name_index(SymbolTableEntry *entry);
uint64_t	  symtab_entry_get_value(SymbolTableEntry *entry);
uint8_t		  symtab_entry_get_info(SymbolTableEntry *entry);
uint16_t	  symtab_entry_get_shndx(SymbolTableEntry *entry);
unsigned char symtab_entry_extract_st_bind(SymbolTableEntry *entry);
unsigned char symtab_entry_extract_st_type(SymbolTableEntry *entry);
uint8_t		  symtab_entry_get_type(SymbolTableEntry *entry);
const char	 *symtab_entry_get_name_string(SymbolTableEntry *entry,
										   StringTable		*strtab);

/* StringTable */
StringTable get_string_table(ElfFile *elf, StringTableHeader *header);

/* SymbolList */
SymbolList *symblst_parse(SectionHeaderTableIterator *shdr_it,
						  SymbolTableIterator *symtab_it, StringTable *strtab);
SymbolList *symblst_new(unsigned int value, char *name, char digit);
void		symblst_clear(SymbolList **lst);
void		symblst_add_back(SymbolList **lst, SymbolList *new);
void symblst_sort(SymbolList **lst, int (*cmp)(SymbolList *, SymbolList *));
int	 symblst_cmp_lexicographic(SymbolList *a, SymbolList *b);

#endif // MY_NM
