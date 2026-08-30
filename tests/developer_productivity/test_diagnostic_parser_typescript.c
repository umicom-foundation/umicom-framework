/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diagnostic_parser_typescript.c
 *
 * PURPOSE:
 *   Verify the built-in TypeScript diagnostic parser recognizes representative
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

#include "umicom/developer_productivity/diagnostic_parsers/typescript.h"

int main(void)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    const UmiDeveloperDiagnosticParser *parser =
        umi_developer_diagnostic_parser_typescript();

    assert(umi_developer_diagnostic_parser_validate(parser) == UMI_STATUS_OK);
    assert(parser->parse(
        "src/app.ts(10,5): error TS2304: Cannot find name 'x'.",
        &problem,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(umi_developer_problem_validate(&problem) == UMI_STATUS_OK);
    return 0;
}
