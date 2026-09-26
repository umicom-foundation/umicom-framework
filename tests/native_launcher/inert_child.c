/* Umicom Framework tests | Sammy Hegab, Umicom Foundation | MIT
 * ELF bytes for staging tests, not an application substitute. A native stage
 * must only read this file; it must never execute this entry point. */
#include <stdio.h>
int main(void){fputs("ERROR: a staging test executed its input\n",stderr);return 91;}
