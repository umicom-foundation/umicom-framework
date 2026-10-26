/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/diagnostic_parsers/ctest.h
 *
 * PURPOSE:
 *   Publish the normalized CTest diagnostic parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_CTEST_H
#define UMICOM_DEVELOPER_DIAGNOSTIC_PARSER_CTEST_H

#include "umicom/developer_productivity/diagnostic_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer diagnostic parser ctest operation used by this module and its
 * client applications.
 */
/* Recognises individual "Test #N: name ...outcome" lines, with optional
 * completed/total counters and ANSI SGR colour. Failure, exception, timeout
 * and not-run rows are errors; skipped and disabled rows are coverage warnings.
 * Passed rows, progress messages and summaries do not create Problems entries.
 * No source location is inferred from a CTest test number.
 * The visible input must fit UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY including
 * its terminator. Malformed escapes return PARSE_ERROR; overlong input returns
 * CAPACITY_EXCEEDED. Unmatched/refused input preserves out_problem and clears
 * out_matched, except invalid arguments which leave both outputs untouched. */
const UmiDeveloperDiagnosticParser *umi_developer_diagnostic_parser_ctest(void);

#ifdef __cplusplus
}
#endif

#endif
