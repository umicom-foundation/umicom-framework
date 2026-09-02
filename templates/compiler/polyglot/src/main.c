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
/* Provide the multiply operation used by this module and its client applications. */
extern int umicom_multiply(int,int);
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { (void)printf("polyglot result: %d\n",umicom_multiply(6,7)); return 0; }
