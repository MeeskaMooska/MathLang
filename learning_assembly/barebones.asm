section .text
    global main
    extern ExitProcess

main:
    xor rcx, rcx            ; Set exit code to 0
    call ExitProcess        ; Call Windows API to exit
