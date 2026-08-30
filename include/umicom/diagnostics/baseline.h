/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/baseline.h
 *
 * PURPOSE:
 *   Retain a diagnostic baseline and classify later findings as new,
 *   unchanged, updated or absent. This supports regression-focused Problems
 *   views, quality gates and SARIF baselineState without hiding old findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_BASELINE_H
#define UMICOM_DIAGNOSTICS_BASELINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/diagnostic_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_BASELINE_API_VERSION 1U

typedef enum UmiDiagnosticBaselineState {
    UMI_DIAGNOSTIC_BASELINE_NONE = 0,
    UMI_DIAGNOSTIC_BASELINE_NEW = 1,
    UMI_DIAGNOSTIC_BASELINE_UNCHANGED = 2,
    UMI_DIAGNOSTIC_BASELINE_UPDATED = 3,
    UMI_DIAGNOSTIC_BASELINE_ABSENT = 4
} UmiDiagnosticBaselineState;

typedef struct UmiDiagnosticBaselineEntry {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t fingerprint;
    uint64_t content_fingerprint;
    char provider_id[UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY];
    char diagnostic_id[UMI_DIAGNOSTIC_ID_CAPACITY];
    char source[UMI_DIAGNOSTIC_SOURCE_CAPACITY];
    char code[UMI_DIAGNOSTIC_CODE_CAPACITY];
    char uri[UMI_DIAGNOSTIC_URI_CAPACITY];
    uint32_t line;
    uint32_t column;
    UmiDiagnosticSeverity severity;
} UmiDiagnosticBaselineEntry;

typedef struct UmiDiagnosticBaselineDecision {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDiagnosticBaselineState state;
    uint64_t fingerprint;
    uint64_t baseline_content_fingerprint;
    uint64_t current_content_fingerprint;
    UmiDiagnosticSeverity baseline_severity;
    UmiDiagnosticSeverity current_severity;
    int severity_changed;
    int content_changed;
} UmiDiagnosticBaselineDecision;

typedef struct UmiDiagnosticBaselineSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t entry_count;
    size_t seen_count;
    size_t absent_count;
    uint64_t generation;
    uint64_t revision;
} UmiDiagnosticBaselineSnapshot;

typedef struct UmiDiagnosticBaselineDiff {
    uint32_t struct_size;
    uint32_t api_version;
    size_t new_count;
    size_t unchanged_count;
    size_t updated_count;
    size_t absent_count;
    size_t severity_changed_count;
    uint64_t baseline_generation;
    uint64_t revision;
} UmiDiagnosticBaselineDiff;

typedef struct UmiDiagnosticBaseline UmiDiagnosticBaseline;

UmiStatus umi_diagnostic_baseline_create(UmiDiagnosticBaseline **out_baseline);
void umi_diagnostic_baseline_destroy(UmiDiagnosticBaseline *baseline);
UmiStatus umi_diagnostic_baseline_clear(UmiDiagnosticBaseline *baseline);
UmiStatus umi_diagnostic_baseline_capture_finding(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderFinding *finding);
UmiStatus umi_diagnostic_baseline_capture_batch(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderBatch *batch);
UmiStatus umi_diagnostic_baseline_begin_comparison(
    UmiDiagnosticBaseline *baseline);
UmiStatus umi_diagnostic_baseline_compare(
    UmiDiagnosticBaseline *baseline,
    const UmiDiagnosticProviderFinding *finding,
    UmiDiagnosticBaselineDecision *out_decision);
UmiStatus umi_diagnostic_baseline_finish_comparison(
    UmiDiagnosticBaseline *baseline,
    UmiDiagnosticBaselineDiff *out_diff);
UmiStatus umi_diagnostic_baseline_at(
    const UmiDiagnosticBaseline *baseline,
    size_t position,
    UmiDiagnosticBaselineEntry *out_entry);
UmiStatus umi_diagnostic_baseline_absent_at(
    const UmiDiagnosticBaseline *baseline,
    size_t position,
    UmiDiagnosticBaselineEntry *out_entry);
UmiStatus umi_diagnostic_baseline_snapshot(
    const UmiDiagnosticBaseline *baseline,
    UmiDiagnosticBaselineSnapshot *out_snapshot);
size_t umi_diagnostic_baseline_count(const UmiDiagnosticBaseline *baseline);
uint64_t umi_diagnostic_baseline_revision(const UmiDiagnosticBaseline *baseline);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_BASELINE_H */
