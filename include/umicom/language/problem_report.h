/* Umicom Framework language problem summaries. Sammy Hegab, Umicom Foundation, MIT. */
#ifndef UMICOM_LANGUAGE_PROBLEM_REPORT_H
#define UMICOM_LANGUAGE_PROBLEM_REPORT_H
#include "umicom/language/diagnostic.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLanguageProblemReport {size_t total;size_t errors;size_t warnings;size_t information;size_t hints;uint64_t revision;} UmiLanguageProblemReport;
UmiStatus umi_language_problem_report_build(const UmiLanguageDiagnosticRegistry *registry,UmiLanguageProblemReport *out_report);
#ifdef __cplusplus
}
#endif
#endif
