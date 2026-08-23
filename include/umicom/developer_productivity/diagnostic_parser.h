/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parser.h
 *
 * PURPOSE:
 *   Define pluggable normalized diagnostic parsers for compiler/build/test tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PARSER_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PARSER_H

#include "umicom/developer_productivity/problem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiDeveloperDiagnosticParseFunction)(
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched);

typedef struct UmiDeveloperDiagnosticParser {
    uint32_t structure_size;
    uint32_t api_version;
    const char *parser_id;
    const char *title;
    const char *tool_id;
    int priority;
    UmiDeveloperDiagnosticParseFunction parse;
} UmiDeveloperDiagnosticParser;

UmiStatus umi_developer_diagnostic_parser_validate(
    const UmiDeveloperDiagnosticParser *parser);

#ifdef __cplusplus
}
#endif

#endif
