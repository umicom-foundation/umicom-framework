/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/diagnostic_item_model.h
 *
 * PURPOSE:
 *   Model diagnostic item model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_DIAGNOSTIC_ITEM_MODEL_H
#define UMICOM_TEST_WORKBENCH_DIAGNOSTIC_ITEM_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDiagnosticItemModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiDiagnosticItemModel;
UmiStatus umi_diagnostic_item_model_init(UmiDiagnosticItemModel *model,const char *id,const char *label);
UmiStatus umi_diagnostic_item_model_set_active(UmiDiagnosticItemModel *model,bool active);
UmiStatus umi_diagnostic_item_model_set_count(UmiDiagnosticItemModel *model,uint32_t item_count);
UmiStatus umi_diagnostic_item_model_set_state(UmiDiagnosticItemModel *model,UmiTestWorkbenchState state);
int umi_diagnostic_item_model_valid(const UmiDiagnosticItemModel *model);
#ifdef __cplusplus
}
#endif
#endif
