section .data
div_zero_msg db "Division by zero error", 10
section .text
global _start
_start:
    ;; let
    mov rax, 2
    push rax
    mov rax, 3
    push rax
    mov rax, 2
    push rax
    pop rax
    pop rbx
    mul rbx
    push rax
    mov rax, 10
    push rax
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rbx
    pop rax
    cmp rax, 0
    je label1
    xor rdx, rdx
    div rbx
    push rax
    jmp label0
label1:
    mov rax, 1
    mov rdi, 1
    mov rsi, div_zero_msg
    mov rdx, 23
    syscall
    mov rax, 60
    mov rdi, 1
    syscall
label0:
    ;; /let
    ;; let
    mov rax, 0
    push rax
    ;; /let
    ;; if
    push QWORD [rsp + 0]
    pop rax
    test rax, rax
    jz label2
    mov rax, 33
    push rax
    pop rax
    mov [rsp + 0], rax
    jmp label3
label2:
    ;; elif
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label4
    mov rax, 2
    push rax
    pop rax
    mov [rsp + 0], rax
    jmp label3
label4:
    ;; elif
    mov rax, 1
    push rax
    push QWORD [rsp + 8]
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    test rax, rax
    jz label5
    mov rax, 6
    push rax
    pop rax
    mov [rsp + 0], rax
    jmp label3
label5:
    ;; else
    mov rax, 5
    push rax
    pop rax
    mov [rsp + 0], rax
label3:
    ;; /if
    ;; exit
    push QWORD [rsp + 0]
    mov rax, 60
    pop rdi
    syscall
    ;; /exit
    mov rax, 60
    mov rdi, 0
    syscall
