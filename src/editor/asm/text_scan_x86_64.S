/*-----------------------------------------------------------------------------
 * Umicom Framework x86-64 scalar byte scanner.
 *
 * System V / MinGW64 ABI:
 *   rdi/rcx = bytes, rsi/rdx = count, rdx/r8b = value depending on platform.
 *
 * The MinGW64 build defines _WIN32 and uses the Microsoft x64 argument
 * registers. The loop is intentionally scalar and auditable; later SIMD
 * implementations can replace it behind the identical C boundary.
 *
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/

    .text
    .globl umi_editor_text_scan_count_byte_x86_64
#ifdef _WIN32
umi_editor_text_scan_count_byte_x86_64:
    movq %rcx, %r9
    movq %rdx, %r10
    movb %r8b, %r11b
#else
    .type umi_editor_text_scan_count_byte_x86_64, @function
umi_editor_text_scan_count_byte_x86_64:
    movq %rdi, %r9
    movq %rsi, %r10
    movb %dl, %r11b
#endif
    xorl %eax, %eax
    xorl %ecx, %ecx
    testq %r10, %r10
    je .Lumi_scan_x86_done
.Lumi_scan_x86_loop:
    cmpb %r11b, (%r9,%rcx,1)
    jne .Lumi_scan_x86_next
    incq %rax
.Lumi_scan_x86_next:
    incq %rcx
    cmpq %r10, %rcx
    jb .Lumi_scan_x86_loop
.Lumi_scan_x86_done:
    ret
#ifndef _WIN32
    .size umi_editor_text_scan_count_byte_x86_64, .-umi_editor_text_scan_count_byte_x86_64
#endif

#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack,"",@progbits
#endif
