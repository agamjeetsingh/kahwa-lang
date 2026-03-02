# Kahwa (v1)

**A Java-like compiled language — implemented in C++20.**

> This is the original C++ implementation of Kahwa. The project has since migrated to
> Scala 3: see [kahwa-lang-v2](https://github.com/agamjeetsingh/kahwa-lang-v2).

---

## What is Kahwa?

Kahwa is a compiled, statically-typed language with Java-like syntax. The tokeniser and
parser are complete. Semantic analysis is multi-phase and partially implemented: name
declaration, typedef replacement, and type resolution are done; expression type checking
is a work in progress. The project also includes a working Language Server Protocol
implementation providing live diagnostics and syntax highlighting.

---

## Language at a Glance

```java
public class Container<out T> {
    private T item;

    public T getItem() {
        return this.item;
    }
}

typedef List<String> StringList;
```

Key features:

- **Classes** with modifiers (`public`, `private`, `abstract`, `open`, `final`),
  inheritance, and nested classes
- **Generics** with variance annotations (`out` covariant, `in` contravariant, invariant)
- **Type aliases** (`typedef`)
- Methods, fields, and standard control flow (`if`, `while`, `for`, `return`,
  `break`, `continue`)
- Static type system with subtype checking and generic substitution

---

## Implementation Highlights

### Hand-Written Recursive Descent + Pratt Parser

Declarations and statements use recursive
descent parsing and expressions use a Pratt parser with explicit binding powers across 13 precedence
levels, covering left/right associativity, prefix, postfix, infix, member access, calls,
and indexing.

One non-trivial detail: closing nested generic arguments like `Container<Box<T>>` produces
a `>>` token, which is lexically identical to the right-shift operator, a classic problem
in C++-style generics (the angle bracket problem). Rather than complicating the lexer or requiring spaces, the parser
recognises and resolves the ambiguity by splitting `>>` into two `>` tokens in place when in a type context.

The parser is fast: a 10,000-line file tokenises and parses in ~7 ms on average (100 runs,
file I/O and semantic analysis excluded). Throughput scales linearly: 100K lines in
~71 ms, 1M lines in ~735 ms.

### Working LSP Implementation

A Language Server Protocol server implemented from scratch in C++ over stdio using
nlohmann/json:

- **Semantic token-based syntax highlighting** — class and method declarations, type
  references, type parameters, modifiers, and literals
- **Live diagnostics** — tokeniser and parser errors pushed to the editor on every file
  change
- File lifecycle management (`didOpen`, `didChange`, `didClose`) and full protocol
  lifecycle (`initialize`, `shutdown`, `exit`)

### Comprehensive Parser Test Suite

GTest-based tests with a `ParserTestBase` helper and builder-pattern fixtures for readable,
composable test construction. Expression tests are exhaustive:

- Precedence correctness across all 13 levels — tested as a cross-product of all operator
  pairs
- Left- and right-associativity for every operator at the same precedence level
- Prefix/postfix binding power relative to infix
- Literals, calls, indexing, member access, ternary, and unary expressions

---

## Architecture

```
Source Code
    ↓
Tokeniser  →  Tokens
    ↓
Parser  →  AST
    ↓
Semantic Analyser
   Phase 1:    Symbol Declaration       (done)
   Phase 1.5:  Typedef Replacement      (done)
   Phase 2:    Type Resolution          (done)
   Phase 3:    Expression Type Checking (partial)
   Phase 4:    Other Semantic Analysis  (partial)
    ↓
Language Server  →  Editor Integration
```

---

## Current Progress

| Component                               | Status  |
|-----------------------------------------|---------|
| Tokenisation                            | Done    |
| Parsing (recursive descent + Pratt)     | Done    |
| Symbol Declaration                      | Done    |
| Typedef Replacement                     | Done    |
| Type Resolution                         | Done    |
| Expression Type Checking                | Partial |
| Other Semantic Analysis                 | Planned |
| LSP (diagnostics + syntax highlighting) | Done    |
| Code Generation                         | Planned |

---

## About the Migration

After the type system grew complex enough, C++ became significant friction: no pattern
matching, no algebraic data types, and the lack of a type system that could mirror the
compiler's own abstractions made each new feature harder to add. The project migrated to
Scala 3 as a deliberate engineering choice. Scala's case classes, exhaustive pattern
matching, and expressive type system map naturally to compiler internals. The language
design and semantics carry forward in
[kahwa-lang-v2](https://github.com/agamjeetsingh/kahwa-lang-v2).

---

## Building & Running

**Prerequisites**: CMake 3.31+, a C++20 compiler. Dependencies (Google Test,
nlohmann/json, magic_enum) are fetched automatically via CMake FetchContent.

```bash
mkdir build && cd build
cmake ..
make
```

**Run the LSP server:**

```bash
./kahwa_lang
```

The server communicates over stdio and integrates with any LSP-compatible editor.

**Run tests:**

```bash
cd build
make tests
./tests
```