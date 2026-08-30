/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_filter_model.h
 *
 * PURPOSE:
 *   Model diagnostic filter model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_FILTER_MODEL_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_FILTER_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDiagnosticFilterModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticFilterModel;
UmiStatus umi_diagnostic_filter_model_init(UmiDiagnosticFilterModel *model,const char *id,const char *label);
UmiStatus umi_diagnostic_filter_model_set_active(UmiDiagnosticFilterModel *model,bool active);
UmiStatus umi_diagnostic_filter_model_set_count(UmiDiagnosticFilterModel *model,uint32_t item_count);
UmiStatus umi_diagnostic_filter_model_set_state(UmiDiagnosticFilterModel *model,UmiTestWorkbenchState state);
int umi_diagnostic_filter_model_valid(const UmiDiagnosticFilterModel *model);
#ifdef __cplusplus
}
#endif
#endif
