/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parsers/java.c
 *
 * PURPOSE:
 *   Normalize Java/javac output into the shared Framework Problems model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parsers/java.h"

#include "umicom/developer_productivity/diagnostic_parser_support.h"

/* Provide the parse line operation used by this module and its client applications. */
static UmiStatus parse_line(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched)
{
    return umi_developer_diagnostic_parse_pattern(
        UMI_DEVELOPER_DIAGNOSTIC_COLON_LOCATION,
        "developer.diagnostic.java",
        NULL,
        line,
        out_problem,
        out_matched);
}

/*
 * Provide the developer diagnostic parser java operation used by this module and its
 * client applications.
 */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_java(void)
{
    static const UmiDeveloperDiagnosticParser parser = {
        .structure_size = (uint32_t)sizeof(UmiDeveloperDiagnosticParser),
        .api_version = UMI_DEVELOPER_PRODUCTIVITY_API_VERSION,
        .parser_id = "developer.parser.java",
        .title = "Java/javac",
        .tool_id = "java",
        .priority = 100,
        .parse = parse_line
    };
    return &parser;
}
