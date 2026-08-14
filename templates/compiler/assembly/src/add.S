/* Umicom x86-64 assembly template | Sammy Hegab | Umicom Foundation | MIT */
.text
.globl umicom_add
umicom_add:
    movl %edi, %eax
    addl %esi, %eax
    ret
