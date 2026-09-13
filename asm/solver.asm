section .data
  num1 dd 0x000039B5      ; first operand
  num2 dd 0x000062B3      ; second operand

section .text
  global asm_multiply

; unsigned long long asm_multiply(void);
; Returns num1 * num2 as a 64-bit unsigned value in RAX (SysV x86-64 ABI)
asm_multiply:
    mov     eax, [rel num1] ; EAX = num1 (RIP-relative: needed for PIE executables)
    mov     ecx, [rel num2] ; ECX = num2
    mul     ecx             ; unsigned EAX * ECX -> EDX:EAX (64-bit result)
    shl     rdx, 32         ; move high 32 bits up
    or      rax, rdx        ; combine into full 64-bit RAX
    ret

section .note.GNU-stack noalloc noexec nowrite  ; marks stack non-executable, silences a linker warning
