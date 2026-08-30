/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_duration_model.h
 *
 * PURPOSE:
 *   Model test duration model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_DURATION_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_DURATION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestDurationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestDurationModel;
UmiStatus umi_test_duration_model_init(UmiTestDurationModel *model,const char *id,const char *label);
UmiStatus umi_test_duration_model_set_active(UmiTestDurationModel *model,bool active);
UmiStatus umi_test_duration_model_set_count(UmiTestDurationModel *model,uint32_t item_count);
UmiStatus umi_test_duration_model_set_state(UmiTestDurationModel *model,UmiTestWorkbenchState state);
int umi_test_duration_model_valid(const UmiTestDurationModel *model);
#ifdef __cplusplus
}
#endif
#endif
