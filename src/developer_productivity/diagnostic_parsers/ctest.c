/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/ctest.c
 *
 * PURPOSE:
 *   Normalize CTest output into the shared Framework Problems model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/ctest.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"

static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_PREFIX,
        "developer.diagnostic.ctest",
        "Failed",
        line,
        out_problem,
        out_matched);
}

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_ctest(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.ctest",
        .title = "CTest",
        .tool_id = "ctest",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
