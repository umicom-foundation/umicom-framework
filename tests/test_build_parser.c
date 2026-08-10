/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_parser.c
 *
 * PURPOSE:
 *   Verify Clang/GCC and MSVC diagnostic parsing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/build/parser.h"

int main(void)
{
    UmiBuildDiagnostic diagnostic;
    UmiBuildDiagnosticList list;
    assert(umi_build_parse_diagnostic_line("src/main.c:12:7: error: invalid token", &diagnostic) == UMI_STATUS_OK);
    assert(diagnostic.line == 12U);
    assert(diagnostic.column == 7U);
    assert(diagnostic.severity == UMI_BUILD_DIAGNOSTIC_ERROR);
    assert(strcmp(diagnostic.file, "src/main.c") == 0);
    assert(umi_build_parse_output("a.c:1:2: warning: first\n"
                                  "b.c:3:4: error: second\n", &list) == UMI_STATUS_OK);
    assert(list.count == 2U);
    return 0;
}
