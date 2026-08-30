/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/cmake.c
 *
 * PURPOSE:
 *   Normalize CMake output into the shared Framework Problems model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/cmake.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"

static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_CMAKE,
        "developer.diagnostic.cmake",
        NULL,
        line,
        out_problem,
        out_matched);
}

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_cmake(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.cmake",
        .title = "CMake",
        .tool_id = "cmake",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
