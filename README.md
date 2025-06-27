# Hydrogen Compiler (C++)

Built a custom compiler from scratch for my own programming language, using assembly as intermediate code.

---

## 🔧 How to Build and Run

```bash
# Step 1: Create a separate build folder
mkdir build

# Step 2: Move into the build directory
cd build

# Step 3: Generate Makefiles using CMake
cmake ..

# Step 4: Compile the source code
make

# Step 5: Move the compiled binary (hydro) to the root project directory
#         so it can access input files like test1.hy located outside /build
mv hydro ..

# Step 6: Go back to root directory and run the compiler
cd ..
./hydro test1.hy

```
---
## Grammar

$$
\begin{align}
    [\text{Prog}] &\to [\text{Stmt}]^* \\
    [\text{Stmt}] &\to
    \begin{cases}
        \text{exit}([\text{Expr}]); \\
        \text{let}\space\text{ident} = [\text{Expr}]; \\
        \text{ident} = \text{[Expr]}; \\
        \text{if} ([\text{Expr}])[\text{Scope}]\text{[IfPred]}\\
        [\text{Scope}]
    \end{cases} \\
    \text{[Scope]} &\to \{[\text{Stmt}]^*\} \\
    \text{[IfPred]} &\to 
    \begin{cases}
        \text{elif}(\text{[Expr]})\text{[Scope]}\text{[IfPred]} \\
        \text{else}\text{[Scope]} \\
        \epsilon
    \end{cases} \\
    [\text{Expr}] &\to
    \begin{cases}
        [\text{Term}] \\
        [\text{BinExpr}]
    \end{cases} \\
    [\text{BinExpr}] &\to
    \begin{cases}
        [\text{Expr}] * [\text{Expr}] & \text{prec} = 1 \\
        [\text{Expr}] / [\text{Expr}] & \text{prec} = 1 \\
        [\text{Expr}] + [\text{Expr}] & \text{prec} = 0 \\
        [\text{Expr}] - [\text{Expr}] & \text{prec} = 0 \\
    \end{cases} \\ 
    [\text{Term}] &\to
    \begin{cases}
        \text{integer} \\
        \text{ident} \\
        ([\text{Expr}])
    \end{cases}
\end{align}
$$

---
## Example (test1.hy)

```
# Input
let x=12;
exit(x);
```
```
# (Output)
Token List (10 tokens):
0. [Line: 1, Type: `let`]
1. [Line: 1, Type: identifier, Value: "x"]
2. [Line: 1, Type: `=`]
3. [Line: 1, Type: int literal, Value: "12"]
4. [Line: 1, Type: `;`]
5. [Line: 2, Type: `exit`]
6. [Line: 2, Type: `(`]
7. [Line: 2, Type: identifier, Value: "x"]
8. [Line: 2, Type: `)`]
9. [Line: 2, Type: `;`]

Generated AST:
Program
├── Let(x)
│   └── IntLit(12)
└── Exit
   └── Ident(x)

Generated Assembly Code:
section .data
div_zero_msg db "Division by zero error", 10

section .text
global _start
_start:
    ;; let
    mov rax, 12
    push rax
    ;; /let

    ;; exit
    push QWORD [rsp + 0]
    mov rax, 60
    pop rdi
    syscall
    ;; /exit

    mov rax, 60
    mov rdi, 0
    syscall

Output is : 12
```
---
