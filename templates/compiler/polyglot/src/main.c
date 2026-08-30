/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/polyglot/src/main.c
 *
 * PURPOSE:
 *   Implement the main behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom polyglot host | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
extern int umicom_multiply(int,int);
int main(void) { (void)printf("polyglot result: %d\n",umicom_multiply(6,7)); return 0; }
