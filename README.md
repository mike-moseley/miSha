# c-template

A minimal C90 project template with CMake, clangd support, and Unity unit testing.

## Setup

After cloning, configure the project name and binary name by replacing `c-project` and `app` in `CMakeLists.txt`.

Configure the build system (run this after adding/removing source or header files, or after editing `CMakeLists.txt`):

```bash
./clangd_cmake.sh
```

This generates `cmake/compile_commands.json` used by clangd for IDE integration.

## Build & Run

```bash
cmake --build cmake
./bin/app
```

Or use the convenience script:

```bash
./build_run.sh
```

## Debugging

Build with debug info and launch gdb:

```bash
./gdb_build_run.sh
```

## Testing

Tests live in `test/`. Each test file requires a corresponding block in `CMakeLists.txt` — see the existing `test_example` block as a guide. Run all tests:

```bash
./tests_build_run.sh
```

The script exits with a non-zero status if any test fails.

## Project Structure

```
.
├── src/            # Source files (.c) — all are compiled automatically
├── include/        # Header files (.h)
├── test/
│   ├── unity/      # Unity test framework
│   └── test_*.c    # Test files
├── bin/            # Compiled binaries (git-ignored)
│   └── test/       # Compiled test binaries
└── cmake/          # CMake build directory (git-ignored)
```

## Adding a New Test

1. Create `test/test_foo.c` using `test_example.c` as a starting point.
2. Copy the test block in `CMakeLists.txt`, replacing `test_example` with `test_foo`.
3. Add `test_foo` to the `add_custom_target(tests DEPENDS ...)` line.
4. Re-run `./clangd_cmake.sh` to reconfigure.
