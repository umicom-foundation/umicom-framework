/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/python.c
 *
 * PURPOSE:
 *   Normalize Python output into the shared Framework Problems model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/python.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"

static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_PYTHON_TRACE,
        "developer.diagnostic.python",
        NULL,
        line,
        out_problem,
        out_matched);
}

const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_python(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.python",
        .title = "Python",
        .tool_id = "python",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
