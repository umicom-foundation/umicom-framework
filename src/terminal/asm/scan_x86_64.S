/* Audited x86-64 scalar byte counter for terminal transcript scans. */
    .text
    .globl umi_terminal_scan_count_byte_x86_64
#ifdef _WIN32
umi_terminal_scan_count_byte_x86_64:
    movq %rcx, %r9
    movq %rdx, %r10
    movb %r8b, %r11b
#else
    .type umi_terminal_scan_count_byte_x86_64, @function
umi_terminal_scan_count_byte_x86_64:
    movq %rdi, %r9
    movq %rsi, %r10
    movb %dl, %r11b
#endif
    xorl %eax, %eax
    xorl %ecx, %ecx
    testq %r10, %r10
    je .Lumi_terminal_scan_x86_done
.Lumi_terminal_scan_x86_loop:
    cmpb %r11b, (%r9,%rcx,1)
    jne .Lumi_terminal_scan_x86_next
    incq %rax
.Lumi_terminal_scan_x86_next:
    incq %rcx
    cmpq %r10, %rcx
    jb .Lumi_terminal_scan_x86_loop
.Lumi_terminal_scan_x86_done:
    ret
#ifndef _WIN32
    .size umi_terminal_scan_count_byte_x86_64, .-umi_terminal_scan_count_byte_x86_64
#endif
#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",@progbits
#endif
