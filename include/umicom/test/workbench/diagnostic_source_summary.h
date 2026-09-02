/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_source_summary.h
 *
 * PURPOSE:
 *   Model diagnostic source summary state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SOURCE_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SOURCE_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the diagnostic source summary data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticSourceSummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticSourceSummary;
/**
 * Initialise diagnostic source summary from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_source_summary_init(UmiDiagnosticSourceSummary *model,const char *id,const char *label);
/**
 * Exercise diagnostic source summary set active and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_diagnostic_source_summary_set_active(UmiDiagnosticSourceSummary *model,bool active);
/**
 * Return the number of records represented by diagnostic source summary set without
 * changing their state.
 */
UmiStatus umi_diagnostic_source_summary_set_count(UmiDiagnosticSourceSummary *model,uint32_t item_count);
/**
 * Exercise diagnostic source summary set state and return a clear result when the
 * behaviour no longer matches its contract.
 */
UmiStatus umi_diagnostic_source_summary_set_state(UmiDiagnosticSourceSummary *model,UmiTestWorkbenchState state);
/**
 * Check that diagnostic source summary satisfies its contract before another service
 * relies on it.
 */
int umi_diagnostic_source_summary_valid(const UmiDiagnosticSourceSummary *model);
#ifdef __cplusplus
}
#endif
#endif
