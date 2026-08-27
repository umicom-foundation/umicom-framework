/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/test_impact_model.h
 *
 * PURPOSE:
 *   Model test impact model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_TEST_IMPACT_MODEL_H
#define UMICOM_TEST_WORKBENCH_TEST_IMPACT_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestImpactModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiTestImpactModel;
UmiStatus umi_test_impact_model_init(UmiTestImpactModel *model,const char *id,const char *label);
UmiStatus umi_test_impact_model_set_active(UmiTestImpactModel *model,bool active);
UmiStatus umi_test_impact_model_set_count(UmiTestImpactModel *model,uint32_t item_count);
UmiStatus umi_test_impact_model_set_state(UmiTestImpactModel *model,UmiTestWorkbenchState state);
int umi_test_impact_model_valid(const UmiTestImpactModel *model);
#ifdef __cplusplus
}
#endif
#endif
