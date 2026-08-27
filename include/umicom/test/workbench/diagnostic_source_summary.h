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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SOURCE_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SOURCE_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDiagnosticSourceSummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticSourceSummary;
UmiStatus umi_diagnostic_source_summary_init(UmiDiagnosticSourceSummary *model,const char *id,const char *label);
UmiStatus umi_diagnostic_source_summary_set_active(UmiDiagnosticSourceSummary *model,bool active);
UmiStatus umi_diagnostic_source_summary_set_count(UmiDiagnosticSourceSummary *model,uint32_t item_count);
UmiStatus umi_diagnostic_source_summary_set_state(UmiDiagnosticSourceSummary *model,UmiTestWorkbenchState state);
int umi_diagnostic_source_summary_valid(const UmiDiagnosticSourceSummary *model);
#ifdef __cplusplus
}
#endif
#endif
