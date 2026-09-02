/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/problem_report.h
 *
 * PURPOSE:
 *   Publish the public problem report contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_PROBLEM_REPORT_H
#define UMICOM_LANGUAGE_PROBLEM_REPORT_H
#include "umicom/language/diagnostic.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the language problem report data shared with callers of this public contract.
 */
typedef struct UmiLanguageProblemReport {size_t total;size_t errors;size_t warnings;size_t information;size_t hints;uint64_t revision;} UmiLanguageProblemReport;
/**
 * Provide the language problem report build operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_problem_report_build(const UmiLanguageDiagnosticRegistry *registry,UmiLanguageProblemReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
