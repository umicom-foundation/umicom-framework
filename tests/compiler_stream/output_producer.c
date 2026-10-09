/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/output_producer.c
 * PURPOSE: Produce a long real child-process transcript for diagnostic retention coverage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    /* This is an isolated test producer, not a compiler replacement in Studio.
     * Its nonzero exit must remain a failed operation despite parsed messages. */
    if (argc != 2 || strcmp(argv[1], "emit") != 0)
        return 2;
    fputs("first.c:2:3: error: first diagnostic\n", stdout);
    for (unsigned i = 0U; i < 5000U; ++i)
        fputs("ordinary build progress without diagnostics\n", stdout);
    fputs("CMake Error at CMakeLists.txt:7 (message):\n  final configure explanation\n", stdout);
    fputs("last.c:9: warning: final diagnostic", stdout);
    return 7;
}
