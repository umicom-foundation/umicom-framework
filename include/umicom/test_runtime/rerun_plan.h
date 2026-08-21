/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/rerun_plan.h
 *
 * PURPOSE:
 *   Plan explicit reruns for failed tests without masking original evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RERUN_PLAN
#define UMICOM_TEST_RUNTIME_RERUN_PLAN
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeRerunPlan
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t test_count;
    uint64_t attempt;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeRerunPlan;
void umi_test_runtime_rerun_plan_init(UmiTestRuntimeRerunPlan *value,const char *id);
UmiStatus umi_test_runtime_rerun_plan_validate(const UmiTestRuntimeRerunPlan *value);
UmiStatus umi_test_runtime_rerun_plan_set_detail(UmiTestRuntimeRerunPlan *value,const char *detail);
UmiStatus umi_test_runtime_rerun_plan_set_test_count(UmiTestRuntimeRerunPlan *value,uint64_t number);
UmiStatus umi_test_runtime_rerun_plan_set_attempt(UmiTestRuntimeRerunPlan *value,uint64_t number);
bool umi_test_runtime_rerun_plan_same_identity(const UmiTestRuntimeRerunPlan *left,const UmiTestRuntimeRerunPlan *right);
#ifdef __cplusplus
}
#endif
#endif
