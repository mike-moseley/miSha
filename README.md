# miSHa

A basic Unix shell written in C90, built with CMake.
Uses custom data structures and allocators from sibling projects:
[c-data-structures](https://github.com/mike-moseley/c-data-structures) | [my-alloc](https://github.com/mike-moseley/my-alloc)

*Outline and roadmap written by Claude (claude.ai/code).*

---

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
  - [ ] `unset`
- [x] `$VAR` expansion in the lexer
- [ ] Inherit and manage the environment table (hash map)

### Milestone 3 — Pipelines
- [ ] Parser: recognise `|` and build a pipeline structure
- [ ] Executor: chain commands with `pipe(2)`, dup stdin/stdout

### Milestone 4 — Redirects
- [ ] Parser: recognise `>`, `>>`, `<`
- [ ] Executor: open files and `dup2` onto stdin/stdout/stderr

### Milestone 5 — Quality of life
- [ ] Background jobs with `&`
- [ ] Command history (ring buffer)
- [ ] `Ctrl-C` handling (`SIGINT`)
