# my_nm

A minimal reimplementation of GNU's `nm`.  
List symbol names, values, and types from elf files, be it a 32 or a 64 bit!


## Run Locally

Clone the project

```bash
git clone https://github.com/Grsaiago/my_nm.git
```

Go to the project directory

```bash
cd my-nm
```

Build it

```bash
make build
```

Run it

```bash
./my_nm <elf_file> [<elf_file> ...]
```


## Running Tests

To run the tests, you must have [libcriterion](https://github.com/snaipe/criterion) installed in your machine
```bash
make test
```


## Project structure

| File | Purpose |
|---|---|
| `src/file.c` | Memory-mapped file loading |
| `src/elf.c` | ELF header parsing |
| `src/section_header_table_it.c` | Section header table iterator |
| `src/section_header_entry.c` | Section header field accessors |
| `src/symbol_table_it.c` | Symbol table iterator |
| `src/symbol_table_entry.c` | Symbol table entry field accessors |
| `src/symbol_table_header.c` | Symbol table header helpers (strtab index lookup) |
| `src/string_table.c` | String table construction |
| `src/symbol_list.c` | Symbol linked list — parsing, sorting, printing |
| `src/main.c` | Entry point |
