/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_configuration_model.h
 *
 * PURPOSE:
 *   Model test configuration model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_CONFIGURATION_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_CONFIGURATION_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestConfigurationModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestConfigurationModel;
UmiStatus umi_test_configuration_model_init(UmiTestConfigurationModel *model,const char *id,const char *label);
UmiStatus umi_test_configuration_model_set_active(UmiTestConfigurationModel *model,bool active);
UmiStatus umi_test_configuration_model_set_count(UmiTestConfigurationModel *model,uint32_t item_count);
UmiStatus umi_test_configuration_model_set_state(UmiTestConfigurationModel *model,UmiTestWorkbenchState state);
int umi_test_configuration_model_valid(const UmiTestConfigurationModel *model);
#ifdef __cplusplus
}
#endif
#endif
