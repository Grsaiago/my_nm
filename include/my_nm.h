#ifndef MY_NM
#define MY_NM

#include <elf.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef enum arch_type {
	BITS_32,
	BITS_64,
} ArchType;

typedef struct object_file {
	char  *data;
	size_t size;
} ObjectFile;

typedef struct section_header_table {
	size_t entry_size;
	size_t entry_count;
	size_t table_start_offset;
	size_t current_index;
} SectionHeaderTable;

typedef struct section_header_table_entry {
	ArchType type;
	union {
		Elf32_Shdr s32;
		Elf64_Shdr s64;
	} data;
} SectionHeaderTableEntry;

typedef struct symbol_table {
	size_t					entry_count;
	size_t					entry_size;
	size_t					table_start_offset;
	size_t					associated_strtab_offset;
	size_t					current_index;
	SectionHeaderTableEntry associated_strtab_section_header;
} SymbolTable;

typedef struct symbol_table_entry {
	ArchType type;
	union {
		Elf32_Sym s32;
		Elf64_Sym s64;
	} data;
} SymbolTableEntry;

typedef struct elf_header {
	char data[sizeof(Elf64_Ehdr)];
} ElfHeader;

typedef struct elf_file {
	ObjectFile		   file_data;
	ElfHeader		   hdr;
	SectionHeaderTable sh_table;
} ElfFile;

typedef struct symbol_list {
	void			   *addr;
	char			   *name;
	bool				heap_allocated;
	char				digit;
	struct symbol_list *next;
} SymbolList;

/* ElfFile */
int		 elf_file_load(char *path, ElfFile *elf);
void	 elf_file_free(ElfFile *elf);
ArchType elf_get_arch_type(ElfFile *elf);

/* ELF header */
int	 elf_hdr_parse(ElfFile *elf);
void elf_hdr_debug_print(ElfFile *elf);

/* ELF header getters */
uint16_t elf_hdr_get_type(ElfFile *elf);
uint16_t elf_hdr_get_machine(ElfFile *elf);
uint32_t elf_hdr_version(ElfFile *elf);
uint64_t elf_hdr_get_entry(ElfFile *elf);
uint64_t elf_hdr_get_phoff(ElfFile *elf);
uint64_t elf_hdr_get_shoff(ElfFile *elf);
uint32_t elf_hdr_get_flags(ElfFile *elf);
uint16_t elf_hdr_get_ehsize(ElfFile *elf);
uint16_t elf_hdr_get_phentsize(ElfFile *elf);
uint16_t elf_hdr_get_phnum(ElfFile *elf);
uint16_t elf_hdr_get_shentsize(ElfFile *elf);
uint16_t elf_hdr_get_shnum(ElfFile *elf);
uint16_t elf_hdr_get_shstrndx(ElfFile *elf);

/* Section header iteration */
int	 sh_table_parse(ElfFile *elf);
void sh_table_free(SectionHeaderTable *tbl);
void sh_table_get_current(ElfFile *elf, SectionHeaderTableEntry *entry);
void sh_table_get_at(ElfFile *elf, size_t index,
					 SectionHeaderTableEntry *entry);
int	 sh_table_next(ElfFile *elf);
void sh_table_reset(ElfFile *elf);
int	 sh_table_has_more(ElfFile *elf);

/* Section header getters */
int sh_table_get_entry_size(ElfFile *elf);

/* Section header entry getters */
uint32_t shtable_entry_get_name(SectionHeaderTableEntry *entry);
uint32_t shtable_entry_get_type(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_flags(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_addr(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_offset(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_size(SectionHeaderTableEntry *entry);
uint32_t shtable_entry_get_link(SectionHeaderTableEntry *entry);
uint32_t shtable_entry_get_info(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_addralign(SectionHeaderTableEntry *entry);
uint64_t shtable_entry_get_entsize(SectionHeaderTableEntry *entry);

/* Symbol table iteration */
int	 symtab_parse(ElfFile *elf, SectionHeaderTableEntry *symtab_section,
				  SymbolTable *table);
void symtab_get_current(ElfFile *elf, SymbolTable *table,
						SymbolTableEntry *entry);
void symtab_get_at(ElfFile *elf, SymbolTable *table, size_t index,
				   SymbolTableEntry *entry);
int	 symtab_next(SymbolTable *table);
void symtab_reset(SymbolTable *table);
int	 symtab_has_more(SymbolTable *table);

/* Symbol table entry getters */
uint32_t symtab_entry_get_name_index(SymbolTableEntry *entry);
uint64_t symtab_entry_get_value(SymbolTableEntry *entry);
uint64_t symtab_entry_get_size(SymbolTableEntry *entry);
uint8_t	 symtab_entry_get_info(SymbolTableEntry *entry);
uint8_t	 symtab_entry_get_other(SymbolTableEntry *entry);
uint16_t symtab_entry_get_shndx(SymbolTableEntry *entry);

/* Symbol table entry helper functions */
uint8_t		symtab_entry_get_bind(SymbolTableEntry *entry);
uint8_t		symtab_entry_get_type(SymbolTableEntry *entry);
const char *symtab_entry_get_name_string(ElfFile *elf, SymbolTable *table,
										 SymbolTableEntry *entry);
char		symtab_entry_get_nm_type(ElfFile *elf, SymbolTableEntry *entry);

/* Symbol Linked List */
SymbolList *symblst_new(void *addr, char *name, bool is_heap_allocated,
						char digit);
void		symblst_clear(SymbolList **lst, void (*del)(SymbolList *));
void		symblst_add_back(SymbolList **lst, SymbolList *new);
void		symblst_add_front(SymbolList **lst, SymbolList *new);
SymbolList *symblst_last(SymbolList *lst);
int			symblst_size(SymbolList *lst);
void		symblst_foreach(SymbolList *lst, void (*f)(SymbolList *));

#endif // MY_NM
