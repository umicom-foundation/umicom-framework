/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/assembly/src/main.c
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
/* Umicom assembly interoperability template | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
/* Add add only after its inputs and available capacity have been checked. */
extern int umicom_add(int left,int right);
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { (void)printf("2 + 3 = %d\n",umicom_add(2,3)); return 0; }
