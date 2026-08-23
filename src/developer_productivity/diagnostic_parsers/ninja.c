/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/ninja.c
 *
 * PURPOSE:
 *   Normalize Ninja output into the shared Framework Problems model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/ninja.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"

static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_PREFIX,
        "developer.diagnostic.ninja",
        "ninja:",
        line,
        out_problem,
        out_matched);
}

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_ninja(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.ninja",
        .title = "Ninja",
        .tool_id = "ninja",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
