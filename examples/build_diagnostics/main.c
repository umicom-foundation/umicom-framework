/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/build_diagnostics/main.c
 * PURPOSE: Read a compiler message for an Umicom Notes source file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/parser.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *line = argc > 1 ? argv[1] :
        "C:/Umicom Notes/src/main.c:8:5: error: expected ';' before return";
    UmiBuildDiagnostic problem;
    UmiStatus status = umi_build_parse_diagnostic_line(line, &problem);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "No complete compiler diagnostic: %s\n", umi_status_text(status));
        return 1;
    }
    printf("Source: %s\nLine: %zu\nColumn: %zu\nMessage: %s\n",
        problem.file, problem.line, problem.column, problem.message);
    /* The default invocation is also a useful check for CTest. */
    if (argc == 1 && (strcmp(problem.file, "C:/Umicom Notes/src/main.c") != 0 ||
        problem.line != 8U || problem.column != 5U)) return 1;
    return 0;
}
