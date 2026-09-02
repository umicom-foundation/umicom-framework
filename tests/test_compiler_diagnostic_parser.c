/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_diagnostic_parser.c
 *
 * PURPOSE:
 *   Implement the test compiler diagnostic parser behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/compiler_parser.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiOutputRecord output;
    UmiDiagnosticSnapshot item;
    int matched = 0;
    assert(umi_output_record_init(&output, "build", "Build", "gcc",
        UMI_OUTPUT_STREAM_ERROR, "C:/src/main.c:17:9: error: bad token") == UMI_STATUS_OK);
    assert(umi_compiler_diagnostic_parse(&output, &item, &matched, NULL) == UMI_STATUS_OK);
    assert(matched && item.line == 17U && item.column == 9U);
    assert(strcmp(item.uri, "C:/src/main.c") == 0);
    assert(strcmp(item.message, "bad token") == 0);
    return 0;
}
