/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test/workbench/quality_gate_result.h
 *
 * PURPOSE:
 *   Model quality gate result state for the Framework-owned production Test/Quality workbench.
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
#ifndef UMICOM_TEST_WORKBENCH_QUALITY_GATE_RESULT_H
#define UMICOM_TEST_WORKBENCH_QUALITY_GATE_RESULT_H
#include "umicom/test/workbench/workbench_types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiQualityGateResult {
    UmiTestWorkbenchEntry value;
    uint64_t generation;
    uint32_t item_count;
    bool active;
} UmiQualityGateResult;
UmiStatus umi_quality_gate_result_init(UmiQualityGateResult *model,const char *id,const char *label);
UmiStatus umi_quality_gate_result_set_active(UmiQualityGateResult *model,bool active);
UmiStatus umi_quality_gate_result_set_count(UmiQualityGateResult *model,uint32_t item_count);
UmiStatus umi_quality_gate_result_set_state(UmiQualityGateResult *model,UmiTestWorkbenchState state);
int umi_quality_gate_result_valid(const UmiQualityGateResult *model);
#ifdef __cplusplus
}
#endif
#endif
