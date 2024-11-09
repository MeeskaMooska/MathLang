section .data
    hello db 'Hello, World!', 0  ; Null-terminated string

section .text
    global main
    extern ExitProcess
    extern WriteConsoleA
    extern GetStdHandle

main:
    ; Get the handle to stdout
    mov rcx, -11                   ; STD_OUTPUT_HANDLE
    call GetStdHandle

    ; Write the string to stdout
    mov rcx, rax                   ; stdout handle
    mov rdx, hello                 ; Address of the string
    mov r8, 13                     ; Length of the string (excluding null terminator)
    mov r9, 0                      ; No specific bytes written tracking
    call WriteConsoleA

    ; Exit the program
    xor rcx, rcx                   ; Exit code 0
    call ExitProcess
