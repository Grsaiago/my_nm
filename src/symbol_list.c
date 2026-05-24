#include "my_nm.h"

static char resolve_nm_char(SectionHeaderTableIterator *shdr_it,
							SymbolTableEntry		   *entry);

SymbolList *symblst_parse(SectionHeaderTableIterator *shdr_it,
						  SymbolTableIterator *symtab_it, StringTable *strtab) {
	SymbolTableEntry symtab_entry;
	unsigned char	 nm_char;
	SymbolList		*new_entry;
	SymbolList		*lst;

	new_entry = NULL;
	lst = NULL;

	while (symbol_table_it_has_next(symtab_it)) {
		symbol_table_it_next(symtab_it, &symtab_entry);

		if (symtab_entry_get_name_index(&symtab_entry) == 0 ||
			symtab_entry_get_type(&symtab_entry) == STT_FILE) {
			continue;
		}

		nm_char = resolve_nm_char(shdr_it, &symtab_entry);
		new_entry = symblst_new(
			symtab_entry_get_value(&symtab_entry),
			(char *)symtab_entry_get_name_string(&symtab_entry, strtab),
			nm_char);
		if (new_entry == NULL) {
			symblst_clear(&lst);
			return (NULL);
		}
		symblst_add_back(&lst, new_entry);
	}
	symblst_sort(&lst, symblst_cmp_lexicographic);
	return (lst);
}

SymbolList *symblst_new(unsigned int value, char *name, char digit) {
	SymbolList *new;

	new = (SymbolList *)malloc(sizeof(SymbolList));
	if (!new)
		return (NULL);
	*new = (SymbolList){
		.digit = digit,
		.value = value,
		.name = name,
		.next = NULL,
	};
	return (new);
}

void symblst_clear(SymbolList **lst) {
	void	   *aux;
	SymbolList *node;

	if (!lst)
		return;
	node = *lst;
	while (node) {
		aux = node->next;
		free(node);
		node = aux;
	}
	*lst = NULL;
	return;
}

void symblst_add_back(SymbolList **lst, SymbolList *new) {
	SymbolList *last_node;

	if (!new)
		return;
	if (!*lst)
		*lst = new;
	else {
		last_node = *lst;
		while (last_node->next)
			last_node = last_node->next;
		last_node->next = new;
	}
	return;
}

void symblst_sort(SymbolList **lst, int (*cmp)(SymbolList *, SymbolList *)) {
	int			swapped;
	SymbolList *a;
	SymbolList *b;
	SymbolList *prev;

	if (!lst || !*lst || !cmp)
		return;
	swapped = 1;
	while (swapped) {
		swapped = 0;
		prev = NULL;
		a = *lst;
		while (a->next) {
			b = a->next;
			if (cmp(a, b) > 0) {
				a->next = b->next;
				b->next = a;
				if (prev)
					prev->next = b;
				else
					*lst = b;
				prev = b;
				swapped = 1;
			} else {
				prev = a;
				a = a->next;
			}
		}
	}
}

int symblst_cmp_lexicographic(SymbolList *a, SymbolList *b) {
	return (ft_strncmp(a->name, b->name, SIZE_MAX));
}

static char resolve_nm_char(SectionHeaderTableIterator *shdr_it,
							SymbolTableEntry		   *entry) {
	SectionHeaderTableEntry associated_section;
	unsigned char			symbol_attribute_bind;
	unsigned char			symbol_attribute_type;

	sh_table_it_get_at(shdr_it, symtab_entry_get_shndx(entry),
					   &associated_section);
	symbol_attribute_bind = symtab_entry_extract_st_bind(entry);
	symbol_attribute_type = symtab_entry_extract_st_type(entry);

	// Let's rule out the GNU extensions
	if (symbol_attribute_bind == STB_GNU_UNIQUE) {
		return 'u';
	} else if (symbol_attribute_type == STT_GNU_IFUNC) {
		return 'i';
	}
	// absolute symbols
	else if (symtab_entry_get_shndx(entry) == SHN_ABS) {
		return 'A';
	}
	// weak symbols are globals that provide a "default value"
	// think of a library that has a default implementation of some func
	// that the user can overwrite.
	//
	// We'll rule out all the STB_WEAK + SHN_UNDEF
	else if (symbol_attribute_bind == STB_WEAK) {
		// the weak symbols are either defined or not
		if (symbol_attribute_type == STT_OBJECT) {
			return symtab_entry_get_shndx(entry) != SHN_UNDEF ? 'V' : 'v';
		}
		return symtab_entry_get_shndx(entry) != SHN_UNDEF ? 'W' : 'w';
	}
	// After rulling out the STB_WEAK
	// any other SHN_UNDEF is just an undefined symbol
	else if (symtab_entry_get_shndx(entry) == SHN_UNDEF) {
		return 'U';
	} // the symbol is in the .bss section
	else if (shtable_entry_get_type(&associated_section) == SHT_NOBITS &&
			 (shtable_entry_get_flags(&associated_section) &
			  (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE)) {
		return symbol_attribute_bind == STB_LOCAL ? 'b' : 'B';
	} // the symbol is a common symbol
	else if (symtab_entry_get_shndx(entry) == SHN_COMMON) {
		// check it it's not a 'common small'
		return symtab_entry_get_shndx(entry) != SHN_MIPS_SCOMMON ? 'C' : 'c';
	} // if the symbol is on the .text
	else if (shtable_entry_get_type(&associated_section) == SHT_PROGBITS &&
			 (shtable_entry_get_flags(&associated_section) &
			  (SHF_ALLOC | SHF_EXECINSTR)) == (SHF_ALLOC | SHF_EXECINSTR)) {
		return symbol_attribute_bind == STB_LOCAL ? 't' : 'T';
	}
	// initialized data section (.ctors, .data, .data1, .dtors).
	// I don't check the SHT_PROGBITS because on
	// trial and error testing that's what worked
	else if ((shtable_entry_get_flags(&associated_section) &
			  (SHF_ALLOC | SHF_WRITE)) == (SHF_ALLOC | SHF_WRITE)) {
		return symbol_attribute_bind == STB_LOCAL ? 'd' : 'D';
	}
	// if it exists on the program data
	// and we don't have permission to write (read only)
	else if ((shtable_entry_get_flags(&associated_section) & SHF_ALLOC) ==
				 SHF_ALLOC &&
			 (shtable_entry_get_flags(&associated_section) & SHF_WRITE) !=
				 SHF_WRITE) {
		return symbol_attribute_bind == STB_LOCAL ? 'r' : 'R';
	} // if it occupies space, but it's not used for anything in runtime, then
	  // it's debugging
	else if (shtable_entry_get_type(&associated_section) == SHT_PROGBITS &&
			 (shtable_entry_get_flags(&associated_section) &
			  (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR | SHF_MASKPROC)) == 0) {
		return 'N';
	}
	return '?';
}
