# Kahwa Lang

A Java-like programming language with support for classes, generics, and modern type system features.

> **Note:** This project is currently in active development. The compiler frontend (tokeniser, parser, and semantic analysis) is functional but incomplete, and backend code generation and runtime are not yet implemented.

## Features

- **Object-oriented design** - Classes with inheritance, fields, and methods
- **Generic types** - Type parameters with variance support (covariant, contravariant, invariant)
- **Modern syntax** - Clean, expressive Java-like syntax
- **Strong type system** - Static type checking with subtype relationships
- **Language Server Protocol** - Editor integration with syntax highlighting (work in progress)

## Quick Start

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Requirements

- CMake 3.31+
- C++20 compatible compiler
- Dependencies (fetched automatically):
  - Google Test (for testing)
  - nlohmann/json (for LSP)
  - magic_enum (for enum reflection)

### Running the LSP Server

```bash
./kahwa_lang
```

The language server can be integrated with editors supporting LSP for syntax highlighting and other features.

## Language Overview

Kahwa supports classes, methods, fields, generics, and standard control flow constructs.

### Example: Generic Box Class

```java
public class Box<T> {
    private T value;

    public void set(T newValue) {
        this.value = newValue;
    }

    public T get() {
        return this.value;
    }

    public boolean isEmpty() {
        return this.value == null;
    }
}
```

### Supported Language Features

**Declarations:**
- Classes with modifiers (`public`, `private`, `abstract`, `open`, `final`)
- Generic type parameters with variance annotations
- Methods and fields (static and instance)
- Type aliases (typedefs)
- Nested classes

**Type System:**
- Generic types with type parameters
- Type variance (covariance, contravariance, invariance)
- Subtype checking and inheritance
- Type aliases

> **Note:** Primitive types like `String`, `Boolean`, `Integer`, and `Double` are referenced in the syntax but not yet fully implemented in the runtime.

## Architecture

The compiler follows a multi-stage pipeline:

```
Source Code
    ↓
Tokeniser → Tokens
    ↓
Parser → Abstract Syntax Tree (AST)
    ↓
Semantic Analyser (Phase 1) → Symbol Table
    ↓
Semantic Analyser (Phase 2) → Type Resolution
    ↓
Semantic Analyser (Phase 3) → Expression and Statement Type Checking
    ↓
Language Server → Editor Integration
```

### Semantic Analysis Phases

The semantic analyser uses a **4-phase approach**:

1. **Phase 1: Symbol Declaration** - Builds the symbol table with proper scoping for all classes, methods, fields, and variables
2. **Phase 1.5: Typedef Replacement** - Replaces type alias references with actual types
3. **Phase 2: Type Resolution** - Resolves all type references to concrete type symbols, handles generic type arguments
4. **Phase 3: Expression Type Checking** - Type-checks expressions and statements, produces bound (type-checked) AST nodes *(in progress)*
5. **Phase 4: Semantic Validation** - Validates inheritance rules, access modifiers, abstract method implementations etc. *(not yet started)*

### Design Patterns

- **Visitor Pattern** - For traversing both AST and semantic trees
- **Builder Pattern** - Fluent APIs for constructing AST nodes and symbols
- **Multi-Phase Resolution** - Enables forward references and handles circular dependencies

## Project Structure

```
kahwa-lang/
├── include/
│   ├── tokeniser/        # Lexical analysis
│   ├── parser/           # Syntax analysis and AST
│   ├── symbols/          # Semantic analysis and symbol table
│   ├── types/            # Type system
│   ├── lsp/              # Language Server Protocol
│   ├── diagnostics/      # Error and warning reporting
│   └── source/           # Source file management
├── src/                  # Implementation files
├── tests/                # Test suite
└── main.cpp              # Entry point
```

## Current Status

### Complete
- Tokeniser (lexical analysis)
- Parser (AST generation)
- Symbol table construction
- Type resolution
- Diagnostic engine
- Source location tracking
- Generic type system with variance

### In Progress
- Expression type checking (Phase 3)
- Language Server Protocol implementation
  - Syntax highlighting (work in progress)
  - Other LSP features planned

### Planned
- Semantic validation (Phase 4)
- Lowering and Backend code generation
- Runtime and execution
- Standard library

## LSP Integration

Kahwa includes a Language Server Protocol implementation for editor integration.

**Current LSP Features:**
- Syntax highlighting (work in progress)
- Diagnostics (work in progress)
- File management and change tracking

**Planned Features:**
- Go to definition
- Find references
- Hover information
- Auto-completion

## Development

### Running Tests

```bash
cd build
make tests
./tests
```

The test suite includes:
- Tokeniser tests
- Parser tests (expressions, statements, declarations)
- Semantic analysis tests
- Type system tests
- LSP tests

### Testing Infrastructure

Tests use Google Test with custom utilities:
- `ParserTestBase` - Helper functions for parsing and AST comparison
- Builder pattern test fixtures - Readable test construction
- Diagnostic testing utilities - Error validation

## Examples

### Class Declaration

```java
public class Person {
    private String name;
    private int age;

    public String getName() {
        return this.name;
    }

    public boolean isAdult() {
        return this.age >= 18;
    }
}
```

### Generic Type with Variance

```java
public class Container<out T> {
    private T item;

    public T getItem() {
        return this.item;
    }
}
```

### Type Aliases

```java
typedef List<String> StringList;
typedef Map<Integer, String> IntToString;
```

## Technical Details

### Parser Implementation

The parser uses:
- **Pratt parsing** (operator precedence) for expressions
- **Recursive descent** for declarations and statements
- Integrated error recovery with diagnostic reporting

### Type System

The type system supports:
- Generic type parameters with bounds
- Variance annotations (`out` for covariance, `in` for contravariance)
- Subtype checking with generic substitution
- Type inference for method calls