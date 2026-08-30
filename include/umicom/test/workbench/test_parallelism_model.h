/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_parallelism_model.h
 *
 * PURPOSE:
 *   Model test parallelism model state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_TEST_PARALLELISM_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_PARALLELISM_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestParallelismModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestParallelismModel;
UmiStatus umi_test_parallelism_model_init(UmiTestParallelismModel *model,const char *id,const char *label);
UmiStatus umi_test_parallelism_model_set_active(UmiTestParallelismModel *model,bool active);
UmiStatus umi_test_parallelism_model_set_count(UmiTestParallelismModel *model,uint32_t item_count);
UmiStatus umi_test_parallelism_model_set_state(UmiTestParallelismModel *model,UmiTestWorkbenchState state);
int umi_test_parallelism_model_valid(const UmiTestParallelismModel *model);
#ifdef __cplusplus
}
#endif
#endif
