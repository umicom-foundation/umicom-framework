/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/hello/main.c
 *
 * PURPOSE:
 *   Print one message and return a status to the operating system.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/* main is where this console program starts. */
int main(void)
{
    /* puts writes the text and a newline. EOF means the write failed. */
    if (puts("Hello from Umicom.") == EOF) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
