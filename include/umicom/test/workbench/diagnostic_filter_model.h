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
/**
 * Represent the diagnostic filter model data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticFilterModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticFilterModel;
/**
 * Initialise diagnostic filter model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_filter_model_init(UmiDiagnosticFilterModel *model,const char *id,const char *label);
/**
 * Exercise diagnostic filter model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_diagnostic_filter_model_set_active(UmiDiagnosticFilterModel *model,bool active);
/**
 * Return the number of records represented by diagnostic filter model set without changing
 * their state.
 */
UmiStatus umi_diagnostic_filter_model_set_count(UmiDiagnosticFilterModel *model,uint32_t item_count);
/**
 * Exercise diagnostic filter model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_diagnostic_filter_model_set_state(UmiDiagnosticFilterModel *model,UmiTestWorkbenchState state);
/**
 * Check that diagnostic filter model satisfies its contract before another service relies
 * on it.
 */
int umi_diagnostic_filter_model_valid(const UmiDiagnosticFilterModel *model);
#ifdef __cplusplus
}
#endif
#endif
