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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SEVERITY_SUMMARY_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_SEVERITY_SUMMARY_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDiagnosticSeveritySummary {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticSeveritySummary;
UmiStatus umi_diagnostic_severity_summary_init(UmiDiagnosticSeveritySummary *model,const char *id,const char *label);
UmiStatus umi_diagnostic_severity_summary_set_active(UmiDiagnosticSeveritySummary *model,bool active);
UmiStatus umi_diagnostic_severity_summary_set_count(UmiDiagnosticSeveritySummary *model,uint32_t item_count);
UmiStatus umi_diagnostic_severity_summary_set_state(UmiDiagnosticSeveritySummary *model,UmiTestWorkbenchState state);
int umi_diagnostic_severity_summary_valid(const UmiDiagnosticSeveritySummary *model);
#ifdef __cplusplus
}
#endif
#endif
