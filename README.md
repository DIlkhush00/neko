# Neko

Neko is an educational end-to-end compiler that translates a custom high-level language into native **x86_64 Linux Assembly**. It is designed to demonstrate the core principles of compiler construction in a clear, modular, and modern C++ way.

## Pipeline

The compiler follows the classic 5-phase architecture:

> [!NOTE]
> This pipeline doesn't include optimisation phase.

1.  **Lexical Analysis (Lexer)**: Converts raw source code into a stream of tokens.
2.  **Syntax Analysis (Parser)**: Transforms tokens into an Abstract Syntax Tree (AST) using recursive descent.
3.  **Semantic Analysis (Sema)**: Validates the AST for scope rules, variable declarations, and basic type consistency.
4.  **Intermediate Representation (IR)**: Flattens the AST into **Three-Address Code (TAC)**, handling control flow and temporaries.
5.  **Code Generation (CodeGen)**: Translates TAC into **x86_64 NASM Assembly** following the System V AMD64 ABI.

## Language Features

- **Variables**: Declaration and assignment (`var x = 10;`).
- **Functions**: First-class function definitions with parameters and return values.
- **Control Flow**: `if-else` statements and `while` loops.
- **Expressions**: Arithmetic operations, comparisons, and logical negation.
- **Built-ins**: Native `print` statement for integers and strings.

---

## Getting Started (Docker)

The easiest way to build and run Neko is using the provided Docker environment, which comes pre-configured with all necessary tools (`cmake`, `nasm`, `gcc`, etc.).

### 1. Spin up the Container

From the project root, build and run the Docker container:

```bash
# Build the image
docker build -t neko-compiler .

# Run the container in the background
docker run -d --name neko-dev neko-compiler

# Enter the container
docker exec -it neko-dev bash
```

### 2. Build the Compiler

Inside the container, use the provided build script:

```bash
# Build the C++ compiler binary
./build.sh
```
This creates the `neko` executable inside the `build/` directory.

---

## Running the Compiler

To compile a source file into assembly:

```bash
./build/neko ../tests/functions.ne
```

This will:
1. Print the AST.
2. Print the generated Three-Address Code.
3. Print the final x86_64 Assembly.
4. Save the assembly to `output.asm`.

---

## Assembling and Linking

Once you have `output.asm`, you can turn it into a native Linux executable:

```bash
# switch to the build directory
cd build

# assemble into an object file
nasm -felf64 output.asm

# link
gcc -no-pie output.o -o output

# and finally run the native binary
./output
```

---

## Code Formatting

To keep the codebase clean, I use `clang-format`. You can run it via the build script or CMake:

```bash
./build.sh --format

# or just directly run the script
./format.sh
```

## Contributing

All type of contributions are welcome!

## License

[MIT](./LICENSE)