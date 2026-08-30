/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diagnostic_parser_gcc.c
 *
 * PURPOSE:
 *   Verify the built-in GCC diagnostic parser recognizes representative
 *   tool output and emits a normalized problem.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_productivity/diagnostic_parsers/gcc.h"

int main(void)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    const UmiDeveloperDiagnosticParser *parser =
        umi_developer_diagnostic_parser_gcc();

    assert(umi_developer_diagnostic_parser_validate(parser) == UMI_STATUS_OK);
    assert(parser->parse(
        "src/main.c:10:5: error: unknown symbol",
        &problem,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(umi_developer_problem_validate(&problem) == UMI_STATUS_OK);
    return 0;
}
