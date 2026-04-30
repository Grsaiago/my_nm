#include "my_nm.h"

SymbolList *symblst_new(void *addr, char *name, bool is_heap_allocated,
						char digit) {
	SymbolList *new;

	new = (SymbolList *)malloc(sizeof(SymbolList));
	if (!new)
		return (NULL);
	*new = (SymbolList){
		.digit = digit,
		.addr = addr,
		.name = name,
		.heap_allocated = is_heap_allocated,
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
		if (node->heap_allocated) {
			free(node->name);
		}
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

void symblst_add_front(SymbolList **lst, SymbolList *new) {
	if (!lst || !new)
		return;
	new->next = *lst;
	*lst = new;
}

SymbolList *symblst_last(SymbolList *lst) {
	SymbolList *node;

	if (!lst)
		return (NULL);
	node = lst;
	while (node->next)
		node = node->next;
	return (node);
}

int symblst_size(SymbolList *lst) {
	SymbolList *node;
	int			i;

	if (!lst)
		return (0);
	i = 1;
	node = lst;
	while (node->next) {
		i++;
		node = node->next;
	}
	return (i);
}

void symblst_foreach(SymbolList *lst, void (*f)(SymbolList *)) {
	SymbolList *node;

	node = lst;
	if (!lst || !f)
		return;
	while (node) {
		f(node);
		node = node->next;
	}
	return;
}
