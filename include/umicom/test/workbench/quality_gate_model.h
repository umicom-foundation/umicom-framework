/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_gate_model.h
 *
 * PURPOSE:
 *   Model quality gate model state for the Framework-owned production Test/Quality workbench.
 *
 * ARCHITECTURE:
 *   Toolkit-neutral Test Explorer, diagnostics, coverage and quality state is
 *   owned by Framework; Studio and other applications remain thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_GATE_MODEL_H
#define UMICOM_TEST_WORKBENCH_QUALITY_GATE_MODEL_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiQualityGateModel {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityGateModel;
UmiStatus umi_quality_gate_model_init(UmiQualityGateModel *model,const char *id,const char *label);
UmiStatus umi_quality_gate_model_set_active(UmiQualityGateModel *model,bool active);
UmiStatus umi_quality_gate_model_set_count(UmiQualityGateModel *model,uint32_t item_count);
UmiStatus umi_quality_gate_model_set_state(UmiQualityGateModel *model,UmiTestWorkbenchState state);
int umi_quality_gate_model_valid(const UmiQualityGateModel *model);
#ifdef __cplusplus
}
#endif
#endif
