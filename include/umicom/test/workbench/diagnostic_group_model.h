/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_group_model.h
 *
 * PURPOSE:
 *   Model diagnostic group model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_GROUP_MODEL_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_GROUP_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the diagnostic group model data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticGroupModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticGroupModel;
/**
 * Initialise diagnostic group model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_group_model_init(UmiDiagnosticGroupModel *model,const char *id,const char *label);
/**
 * Exercise diagnostic group model set active and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_diagnostic_group_model_set_active(UmiDiagnosticGroupModel *model,bool active);
/**
 * Return the number of records represented by diagnostic group model set without changing
 * their state.
 */
UmiStatus umi_diagnostic_group_model_set_count(UmiDiagnosticGroupModel *model,uint32_t item_count);
/**
 * Exercise diagnostic group model set state and return a clear result when the behaviour
 * no longer matches its contract.
 */
UmiStatus umi_diagnostic_group_model_set_state(UmiDiagnosticGroupModel *model,UmiTestWorkbenchState state);
/**
 * Check that diagnostic group model satisfies its contract before another service relies
 * on it.
 */
int umi_diagnostic_group_model_valid(const UmiDiagnosticGroupModel *model);
#ifdef __cplusplus
}
#endif
#endif
