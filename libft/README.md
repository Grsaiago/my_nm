<div align="center">
  <img src="https://user-images.githubusercontent.com/98427284/175352149-d1146a34-d163-4d0e-806f-1c63c04d3663.png" height="150" width="150"/>
  
</div>

<h1 align ="center">
  42 Cursus' libft
  
</h1>
<p align ="center">
  This repo is my first project as a cadet <a href="https://42.rio/">@42rio.</a><br>It acts as both a study project on replicating libc functions, as well as a developer toolbelt to be used throughout the cursus.

</p>

## Install
```sh
$> git clone https://github.com/Grsaiago/libft <path where to put the lib> && cd <path to lib>;
$> make
```

**Don't forget to link the lib with the -lft -L$(path_to_lib) on your project's makefile**

`make` supports the following flags. (the flags already compile with the bonus part)
- `all` or simply `make` -> Compiles everything and gets the lib ready to go!
- `make clean` -> Removes the object files.
- `make fclean` -> Removes the object files, as well as the .a file
- `make re` -> Removes the object and .a files and recompiles everything.


## Functions List
### Functions from `<stdio.h>`
* [`ft_printf`](ft_printf.c) - my implementation of the printf function.

### Functions from `<ctype.h>`
* [`ft_isascii`](ft_isascii.c) - checks if it's an ASCII char.
* [`ft_isalpha`](ft_isalpha.c) - checks if it's an alphabetic char.
* [`ft_isalnum`](ft_isalnum.c) - checks if it'a an alphanumeric char.
* [`ft_isdigit`](ft_isdigit.c) - checks if it's a digit.
* [`ft_isprint`](ft_isprint.c) - checks if it's a printable character.
* [`ft_toupper`](ft_toupper.c) - turns the char into uppercase.
* [`ft_tolower`](ft_tolower.c) - turns the char into lowercase.
* [`ft_isspace`](ft_isspace.c) - checks if it's a whitespace.

### Functions from `<stdlib.h>`
* [`ft_atoi`](ft_atoi.c) - converts ASCII to integer.
* [`ft_calloc`](ft_calloc.c) - allocate and initialize to 0 a block of memory.

### Functions from `<string.h>` && `<strings.h>`
* [`ft_strlen`](ft_strlen.c) - gets the lenght of a null terminated string.
* [`ft_strdup`](ft_strdup.c) - duplicates a string (with malloc).
* [`ft_strnstr`](ft_strnstr.c) - searches for the occurence of a byte stream inside another byte stream.
* [`ft_strchr`](ft_strchr.c) - searches for the first occurence of a byte in a byte stream.
* [`ft_strrchr`](ft_strrchr.c) - searches for the last occurence of a byte in a byte stream.
* [`ft_strlcpy`](ft_strlcpy.c) - copies the string src to dst.
* [`ft_strlcat`](ft_strlcat.c) - concatenates two strings.
* [`ft_strncmp`](ft_strncmp.c) - compares the first n bytes of two byte streams.
* [`ft_memset`](ft_memset.c) - fill a byte stream with a value.
* [`ft_bzero`](ft_bzero.c) - fill a byte stream with zero.
* [`ft_memcpy`](ft_memcpy.c) - copy a byte stream (doesn't account for overlapping memory blocks).
* [`ft_memmove`](ft_memmove.c) - copy a byte stream (accounts for overlapping memory blocks).
* [`ft_memchr`](ft_memchr.c) - search for a byte in the first n bytes of a byte stream.
* [`ft_memcmp`](ft_memcp.c) - compare two byte streams.

### Functions from `<math.h>`
* [`ft_pow`](ft_pow.c) - power function.

### Nonstandard functions
* [`ft_itoa`](ft_itoa.c) - returns a malloc'd string from an int. 
* [`ft_strndup`](ft_strndup.c) - returns a malloc'd copy of the first n bytes of a string.
* [`ft_substr`](ft_substr.c) - creates a new string from a portion of a given string.
* [`ft_strjoin`](ft_strjoin.c) - joins two strings (with malloc).
* [`ft_strtrim`](ft_strtrim.c) - trailes off the matching char from the beginning and end of a string.
* [`ft_split`](ft_split.c) - splits a string into malloc'd matrix.
* [`ft_free_mat`](ft_free_mat.c) - frees a malloc'd matrix.
* [`ft_strmapi`](ft_strmapi.c) - returns malloc'd string that is the result of successive iterations of a function over a given string.
* [`ft_striteri`](ft_striteri.c) - iterates a given function over each position of a given string.
* [`ft_putchar`](ft_putchar.c) - prints a char to stdout.
* [`ft_putchar_fd`](ft_putchar_fd.c) - writes a char to the given file descriptor.
* [`ft_putstr`](ft_putstr.c) - writes a string to stdout.
* [`ft_putstr_fd`](ft_putstr_fd.c) - writes a string to the given file descriptor.
* [`ft_putendl`](ft_putendl.c) - writes a string followed by a \n to stdout.
* [`ft_putendl_fd`](ft_putendl_fd.c) - writes a string followed by a \n to the given file descriptor.
* [`ft_putnbr_fd`](ft_putnbr.c) - writes a number to stdout.
* [`ft_putnbr_fd`](ft_putnbr_fd.c) - writes a number into the given file descriptor.
* [`ft_printhex`](ft_printhex.c) - prints to stdout a number as it's hexadecimal format.
* [`ft_printptr`](ft_printptr.c) - prints to stdout an address in 0x0hex format.
* [`get_next_line`](get_next_line.c) - my implementation of the GNU get_next_line.
* [`samrtpointer`](smartptr.c) - my implementation of Snaipe's smart pointers. (check his original work [here](https://github.com/Snaipe/libcsptr))

### Linked list functions
* [`ft_lstnew`](ft_lstnew.c) - creates a new node.
* [`ft_lstadd_front`](ft_lstadd_front.c) - adds a new node to the front of the list.
* [`ft_lstadd_back`](ft_lstadd_back.c) - adds a new node to the back of the list.
* [`ft_lstsize`](ft_lstsize.c) - gets the size of the list.
* [`ft_lstlast`](ft_lstlast.c) - gets the last node of the list.
* [`ft_lstdelone`](ft_lstdelone.c) - deletes a node from the list.
* [`ft_lstclear`](ft_lstclear.c) - deletes the whole list.
* [`ft_lstiter`](ft_lstiter.c) - iterates a function over the content of each node.
* [`ft_lstmap`](ft_lstmap.c) - returns a new list that is the result iterations over every node of the list.
* [`ft_lst_remove_if`](ft_lst_remove_if.c) - removes a node if the compare function returns true over a sample data and the contents of said node.
