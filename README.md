# Generic Catalog System

A configurable C++ catalog application that reads a format definition from a file and stores records with mixed field types (string, integer, double, bool) in either single or multi-value form. Supports search and sort operations through a simple command interface.

## Features

- **Format-driven**: The first line of the data file defines field names, types, and whether each field is single- or multi-valued. The catalog adapts at runtime — no recompilation needed for different schemas.
- **Polymorphic storage with templates**: An abstract `FieldBase` class together with templated `FieldSingle<T>` and `FieldMulti<T>` derived classes (with template specializations for `bool`) allows different value types to live in the same container.
- **Custom exception hierarchy**: `MissingFieldException`, `DuplicateEntryException`, and `WrongCommandException` report parsing and command errors, all logged to an output file.
- **Deep-copy semantics**: `Entry` implements the rule of three (destructor, copy constructor, assignment operator) so entries can be safely copied without aliasing the underlying field pointers.
- **Search and sort**: Case-insensitive substring search across any field, and bubble-sort by any field.

## Build & Run

```bash
make
./program
```

The program reads `data.txt` (catalog data) and `commands.txt` (search/sort commands) from the current directory, and writes results to `output.txt`.

## File Format

**Format line** (first line of `data.txt`):
```
name:string:single|tags:string:multi|age:integer:single|active:bool:single
```

**Entry lines** (one per record):
```
Alice|developer:reader|25|true
Bob|writer|30|false
```

**Commands** (`commands.txt`):
```
search "Alice" in "name"
sort "age"
```

## Project Structure

```
.
├── main.cpp        # Entry point
├── catalog.h/.cpp  # Catalog & Entry classes, file parsing, commands
├── field.h         # FieldBase, FieldSingle<T>, FieldMulti<T> templates
├── Makefile        # Build script
└── README.md
```

## Concepts Demonstrated

- Object-oriented design with abstract base classes and pure virtual functions
- Template programming and template specialization
- Polymorphic storage with dynamic dispatch
- Custom exception hierarchies derived from `std::runtime_error`
- The rule of three for safe deep-copy of objects with raw owned pointers
- File I/O and string parsing with `stringstream`

## Notes

This project was developed as part of an undergraduate course in Object-Oriented Programming at Gebze Technical University.
