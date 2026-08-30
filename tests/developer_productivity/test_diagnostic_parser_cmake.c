/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diagnostic_parser_cmake.c
 *
 * PURPOSE:
 *   Verify the built-in CMake diagnostic parser recognizes representative
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

#include "umicom/developer_productivity/diagnostic_parsers/cmake.h"

int main(void)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    const UmiDeveloperDiagnosticParser *parser =
        umi_developer_diagnostic_parser_cmake();

    assert(umi_developer_diagnostic_parser_validate(parser) == UMI_STATUS_OK);
    assert(parser->parse(
        "CMake Error at CMakeLists.txt:24 (add_executable):",
        &problem,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(umi_developer_problem_validate(&problem) == UMI_STATUS_OK);
    return 0;
}
