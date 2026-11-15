# easy-db

`easy-db` is a small database project written in C++17. It is inspired by
[cstack/db_tutorial](https://github.com/cstack/db_tutorial), which builds a
small SQLite-like database to demonstrate how database components fit
together. This project is intended for learning and experimentation; it is
not a production database.

## Features

- Fixed-size rows with an unsigned 32-bit integer primary key
- A disk-backed B-tree with leaf and internal nodes
- A command-line REPL
- `INSERT` and full-table `SELECT` commands
- Persistence across process restarts
- Unit tests for the storage, database, command, and REPL layers

## Architecture

The implementation is divided into layers:

```text
main
  └── Repl
        └── Command
              ├── CommandParser
              ├── CommandStatement
              └── CommandExecutor
                    └── Table
                          └── Btree
                                ├── InternalNode / LeafNode
                                └── PageManager
                                      └── Page
```

`Row` is the fixed-size record representation used by `Table`. `Database`
provides an API for opening and initializing a `Table`. `Status` and
`Result<T>` carry errors and return values across layers.

- **REPL and command layer** — Reads commands, parses them into statements,
  executes them, and formats the output.
- **Database layer** — `Row` validates and serializes records. `Table` stores
  rows using their IDs as B-tree keys. `Database::open()` is a convenience API
  for opening and initializing a table.
- **B-tree layer** — Manages key/value insertion, lookup, ordered scans, and
  node splits. Leaf nodes store serialized row values; internal nodes route
  searches.
- **Page layer** — `Page` represents a 4096-byte page. `PageManager` loads,
  caches, allocates, and flushes pages to the database file.
- **Error handling** — Operations return `Status` or `Result<T>` instead of
  using exceptions for normal error reporting.

## Build

Requirements:

- CMake 3.16 or later
- A C++17 compiler
- CppUTest (required to configure the project and build its test target)

Configure and build from the repository root:

```sh
cmake -S . -B build
cmake --build build
```

The CLI executable is created at `build/easy_db`.

## Run

Pass the database file path as the only argument:

```sh
./build/easy_db my_database.db
```

The REPL displays a `>` prompt. Example session:

```text
> INSERT 1 alice alice@example.com
Inserted row 1
> INSERT 2 bob bob@example.com
Inserted row 2
> SELECT
1, alice, alice@example.com
2, bob, bob@example.com
> quit
```

`exit` and `quit` both leave the REPL. Inserted rows are flushed to the
database file, so they can be read in a later invocation:

```sh
./build/easy_db my_database.db
```

## Supported commands

| Command | Description |
|---|---|
| `INSERT <id> <username> <email>` | Insert a row, or replace the row with the same ID. |
| `SELECT` | Display all rows ordered by ID. |
| `SELECT *` | Same as `SELECT`. |
| `exit` / `quit` | Exit the REPL. |

Command names are case-insensitive. Values are whitespace-delimited; usernames
and email addresses cannot contain spaces. IDs are unsigned 32-bit integers.
Usernames are limited to 32 bytes and email addresses to 255 bytes. Rows have a
fixed serialized size.

## Run tests

```sh
ctest --test-dir build --output-on-failure
```

## Current scope and limitations

- The SQL-like command syntax is intentionally small; `CREATE`, `UPDATE`,
  `DELETE`, filters, and multi-row statements are not implemented.
- The database API currently exposes one table per database file.
- `SELECT` materializes all rows in memory; there is no incremental cursor API.
- Transactions, rollback, concurrent access, and crash recovery are not
  implemented.
- The on-disk format is a project-specific binary format and should be treated
  as experimental. Compatibility between future versions is not guaranteed.

## Suggested next steps

Potential areas to extend the project:

1. Improve REPL shutdown and error reporting, including clean handling of
   end-of-file and stream failures.
2. Add lookup by primary key at the `Table` and command layers.
3. Add a cursor or iterator for scanning large tables without loading every
   row into memory.
4. Add schema and catalog metadata to support multiple tables and column
   definitions.
5. Add `UPDATE`, `DELETE`, and basic `WHERE` expressions.
6. Define and document an explicit on-disk format/versioning policy, then add
   integrity checks and recovery behavior.
7. Add transaction support and define the durability guarantees of commits.

## Project layout

```text
src/
  btree/      B-tree and node implementations
  command/    Command parsing, statements, and execution
  common/     Status and Result types
  database/   Row, Table, and Database APIs
  page/       Page storage and file-backed page manager
  repl/       Interactive command loop
  main.cc     CLI entry point
tests/        Unit tests organized by component
```
