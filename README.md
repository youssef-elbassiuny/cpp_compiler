# C++ Compiler

A small compiler written in C++ that parses a custom programming language and generates x86-64 assembly code.

## Requirements

Before using the compiler, make sure you have the following installed:

- C++
- CMake
- Make
- NASM
- Linux

## Building the Compiler

First, clone the repository:

    git clone https://github.com/youssef-elbassiuny/comp.git
    cd comp

## Complete Workflow

The complete workflow from the `build` directory is:

    cmake -G "Unix Makefiles" ..
    make
    ./main ../test/test1.txt
    nasm -felf64 aut.asm
    ld -o aut aut.o
    ./aut
    echo $?

## Example Program

Example source code:

    let x = 10;
    let y = 20;

    let z = x + y * 2;

    while (x)
    {
        x = x - 1;
    }

    if (z)
    {
        let result = z / 2;
        return result;
    }
    else
    {
        return 0;
    }

Save the program as a `.txt` file and pass it to the compiler:

    ./main ../test/test1.txt

Then assemble and link the generated assembly:

    nasm -felf64 aut.asm
    ld -o aut aut.o

Finally, run it:

    ./aut

And check the return value:

    echo $?

## Supported Features

The compiler currently supports:

- Variables
- Variable assignment
- Arithmetic expressions
- Addition (`+`)
- Subtraction (`-`)
- Multiplication (`*`)
- Division (`/`)
- Parenthesized expressions
- `if`
- `elif`
- `else`
- `while`
- Nested scopes
- `return`
- x86-64 assembly generation

## Compiler Pipeline

The compiler works through several stages:

    Source Code
         |
         v
    Tokenizer
         |
         v
    Tokens
         |
         v
    Parser
         |
         v
    AST (Abstract Syntax Tree)
         |
         v
    Code Generator
         |
         v
    x86-64 Assembly
         |
         v
    NASM
         |
         v
    Object File
         |
         v
    Executable

## Project Structure

    comp/
    ├── include/
    ├── scr/
    ├── test/
    ├── CMakeLists.txt
    └── README.md

## Technologies

- C++
- CMake
- NASM
- x86-64 Assembly
- Linux
- Git

## Status

This project is currently under development.