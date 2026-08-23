/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parser_support.h
 *
 * PURPOSE:
 *   Share bounded parsing helpers across compiler/build/test diagnostic parsers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PARSER_SUPPORT_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_DIAGNOSTIC_PARSER_SUPPORT_H

#include "umicom/developer_productivity/problem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDeveloperDiagnosticPattern {
    UMI_DEVELOPER_DIAGNOSTIC_COLON_LOCATION = 1,
    UMI_DEVELOPER_DIAGNOSTIC_PAREN_LOCATION = 2,
    UMI_DEVELOPER_DIAGNOSTIC_CMAKE = 3,
    UMI_DEVELOPER_DIAGNOSTIC_PREFIX = 4,
    UMI_DEVELOPER_DIAGNOSTIC_PYTHON_TRACE = 5,
    UMI_DEVELOPER_DIAGNOSTIC_TYPESCRIPT = 6
} UmiDeveloperDiagnosticPattern;

UmiStatus umi_developer_diagnostic_parse_pattern(
    UmiDeveloperDiagnosticPattern pattern,
    const char *source_id,
    const char *required_token,
    const char *line,
    UmiDeveloperProblem *out_problem,
    int *out_matched);

UmiDeveloperProductivitySeverity
umi_developer_diagnostic_severity_from_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif
