/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_environment_model.h
 *
 * PURPOSE:
 *   Model test environment model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_ENVIRONMENT_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_ENVIRONMENT_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestEnvironmentModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestEnvironmentModel;
UmiStatus umi_test_environment_model_init(UmiTestEnvironmentModel *model,const char *id,const char *label);
UmiStatus umi_test_environment_model_set_active(UmiTestEnvironmentModel *model,bool active);
UmiStatus umi_test_environment_model_set_count(UmiTestEnvironmentModel *model,uint32_t item_count);
UmiStatus umi_test_environment_model_set_state(UmiTestEnvironmentModel *model,UmiTestWorkbenchState state);
int umi_test_environment_model_valid(const UmiTestEnvironmentModel *model);
#ifdef __cplusplus
}
#endif
#endif
