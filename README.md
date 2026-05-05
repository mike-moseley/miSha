# miSHa

A basic Unix shell written in C90, built with CMake.

Uses custom data structures and allocators from sibling projects:
[c-data-structures](https://github.com/mike-moseley/c-data-structures) | [my-alloc](https://github.com/mike-moseley/my-alloc)

https://github.com/user-attachments/assets/9f5788aa-75ab-4e3c-9497-5565f08e55dc

*Outline and roadmap written, and general guidance provided by Claude (claude.ai/code).*

---

## Motivation

Deepen understanding of C, shells, and linux system calls.

---

## Requirements

CMake and a C compiler, I used gcc.

---

## Quick Start and Usage

Run
```bash
cmake -S . -G "Unix Makefiles" -B cmake
cmake --build cmake
./bin/miSHa
```

---

## Contributing

### Clone the repo

```bash
git clone https://github.com/mike-moseley/miSHa@latest
cd miSHa
```

### Build and run

```bash
cmake -S . -G "Unix Makefiles" -B cmake
cmake --build cmake
./bin/miSHa
```

## Architecture

```
main.c        REPL loop — prompt, read, dispatch
lexer.c       tokenize raw input into a NULL-terminated argv array
parser.c      token stream → command struct (pipelines, redirects, sequences)
executor.c    fork/exec for external commands; pipe and redirect setup
builtins.c    built-in handlers: cd, exit, export, unset
env.c         environment variable table and $VAR expansion
```

---

## Roadmap

### Milestone 1 — Simple commands
- [x] REPL loop: prompt, `fgets`, strip newline, EOF handling
- [x] Lexer: split input on whitespace into `char *argv[]`
- [x] Executor: `fork` + `execvp` + `waitpid`
- [x] Built-in: `exit`

### Milestone 2 — Environment
- Built-ins:
  - [x] `cd`
  - [x] `pwd`
  - [x] `export`
  - [x] `unset`
- [x] `$VAR` expansion in the lexer
- [x] Inherit and manage the environment table (hash map)

### Milestone 3 — Pipelines
- [x] Parser: recognise `|` and build a pipeline structure
- [x] Executor: chain commands with `pipe(2)`, dup stdin/stdout

### Milestone 4 — Redirects
- [x] Parser: recognise `>`, `>>`, `<`
- [ ] Executor: open files and `dup2` onto stdin/stdout/stderr

### Milestone 5 — Quality of life
- [ ] Background jobs with `&`
- [x] Command history (ring buffer)
- [x] Navigating input
- [x] `Ctrl-C` handling (`SIGINT`)
