/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/documentation_coverage.h
 *
 * PURPOSE:
 *   Measure whether C source and header files explain their public functions,
 *   important decisions and standard Umicom ownership information.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_DOCUMENTATION_COVERAGE_H
#define UMICOM_CODEGUARD_DOCUMENTATION_COVERAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Reports own a bounded path so they never retain a caller's temporary pointer. */
#define UMI_CODEGUARD_DOCUMENTATION_PATH_CAPACITY 512U

/* The policy separates public-function guidance from decision guidance so a
 * team can improve old code progressively without hiding either measurement. */
typedef struct UmiCodeGuardDocumentationPolicy {
    uint32_t structure_size;              /* Size protects compatible ABI growth. */
    unsigned minimum_function_percent;    /* Required explained functions, 0–100. */
    unsigned minimum_decision_percent;    /* Required explained decisions, 0–100. */
    bool require_standard_file_header;    /* Enforce purpose, author and licence. */
} UmiCodeGuardDocumentationPolicy;

/* A report contains counts rather than pointers, so it can be logged, stored
 * as build evidence or displayed safely by Studio after the scan has ended. */
typedef struct UmiCodeGuardDocumentationReport {
    uint32_t structure_size;  /* Size protects compatible ABI growth. */
    char path[UMI_CODEGUARD_DOCUMENTATION_PATH_CAPACITY]; /* Owned source path. */
    size_t line_count;                    /* Physical lines read successfully. */
    size_t function_count;                /* Function declaration/definition hints. */
    size_t documented_function_count;     /* Function hints with nearby guidance. */
    size_t decision_count;                /* Conditions, loops and switches found. */
    size_t explained_decision_count;      /* Decisions with nearby explanations. */
    unsigned function_percent;            /* Bounded function coverage, 0–100. */
    unsigned decision_percent;            /* Bounded decision coverage, 0–100. */
    bool has_standard_file_header;         /* Purpose, author and licence found. */
    bool passes;                           /* Cached result for the scan policy. */
} UmiCodeGuardDocumentationReport;

/* Fill a beginner-friendly default policy. New projects may use stricter
 * percentages, while legacy modules can raise the threshold in reviewed steps. */
void umi_codeguard_documentation_policy_default(
    UmiCodeGuardDocumentationPolicy *out_policy);

/* Scan one C or header file without changing it. The scanner deliberately uses
 * conservative hints; a result guides human review and is not a C parser. */
UmiStatus umi_codeguard_documentation_scan_file(
    const char *path,
    const UmiCodeGuardDocumentationPolicy *policy,
    UmiCodeGuardDocumentationReport *out_report);

/* Re-evaluate a stored report against another policy, allowing release profiles
 * to apply stronger requirements without reading the source file again. */
bool umi_codeguard_documentation_report_passes(
    const UmiCodeGuardDocumentationReport *report,
    const UmiCodeGuardDocumentationPolicy *policy);

#ifdef __cplusplus
}
#endif

#endif
