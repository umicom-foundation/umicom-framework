/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_severity_summary.h
 *
 * PURPOSE:
 *   Model diagnostic severity summary state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SEVERITY_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SEVERITY_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the diagnostic severity summary data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticSeveritySummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticSeveritySummary;
/**
 * Initialise diagnostic severity summary from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_severity_summary_init(UmiDiagnosticSeveritySummary *model,const char *id,const char *label);
/**
 * Exercise diagnostic severity summary set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_diagnostic_severity_summary_set_active(UmiDiagnosticSeveritySummary *model,bool active);
/**
 * Return the number of records represented by diagnostic severity summary set without
 * changing their state.
 */
UmiStatus umi_diagnostic_severity_summary_set_count(UmiDiagnosticSeveritySummary *model,uint32_t item_count);
/**
 * Exercise diagnostic severity summary set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_diagnostic_severity_summary_set_state(UmiDiagnosticSeveritySummary *model,UmiTestWorkbenchState state);
/**
 * Check that diagnostic severity summary satisfies its contract before another service
 * relies on it.
 */
int umi_diagnostic_severity_summary_valid(const UmiDiagnosticSeveritySummary *model);
#ifdef __cplusplus
}
#endif
#endif
