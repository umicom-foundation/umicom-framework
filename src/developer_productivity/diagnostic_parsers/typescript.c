/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/typescript.c
 *
 * PURPOSE:
 *   Normalize TypeScript output into the shared Framework Problems model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/typescript.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"

/* Provide the parse line operation used by this module and its client applications. */
static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_TYPESCRIPT,
        "developer.diagnostic.typescript",
        NULL,
        line,
        out_problem,
        out_matched);
}

/*
 * Provide the developer diagnostic parser typescript operation used by this module and its
 * client applications.
 */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_typescript(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.typescript",
        .title = "TypeScript",
        .tool_id = "typescript",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
