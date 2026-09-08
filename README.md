# C++ Compiler

This is a small compiler I'm building using C++.

The idea is to make my own simple programming language and compile it
to x86-64 assembly.




## What it can do right now

- Tokenizer
- Parser
- AST
- Variables
- Variable assignment
- Arithmetic expressions
- `if / elif / else`
- Scopes
- x86-64 assembly generation
- Simple arena allocator

## Example

```text
let x = 10;
let y = 20;

let z = x + y * 2;

if (z)
{
    return z;
}
else
{
    return 0;
}
