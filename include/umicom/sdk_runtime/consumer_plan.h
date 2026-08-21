/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/consumer_plan.h
 *
 * PURPOSE:
 *   Plan headers, libraries, runtime files and resources for one application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CONSUMER_PLAN
#define UMICOM_SDK_RUNTIME_CONSUMER_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeConsumerPlan {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t compile_count;
    uint64_t runtime_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeConsumerPlan;
void umi_sdk_runtime_consumer_plan_init(UmiSdkRuntimeConsumerPlan *value, const char *id);
UmiStatus umi_sdk_runtime_consumer_plan_validate(const UmiSdkRuntimeConsumerPlan *value);
UmiStatus umi_sdk_runtime_consumer_plan_set_path(UmiSdkRuntimeConsumerPlan *value, const char *path);
UmiStatus umi_sdk_runtime_consumer_plan_set_detail(UmiSdkRuntimeConsumerPlan *value, const char *detail);
UmiStatus umi_sdk_runtime_consumer_plan_set_compile_count(UmiSdkRuntimeConsumerPlan *value, uint64_t number);
UmiStatus umi_sdk_runtime_consumer_plan_set_runtime_count(UmiSdkRuntimeConsumerPlan *value, uint64_t number);
UmiStatus umi_sdk_runtime_consumer_plan_set_state(UmiSdkRuntimeConsumerPlan *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_consumer_plan_same_identity(const UmiSdkRuntimeConsumerPlan *left, const UmiSdkRuntimeConsumerPlan *right);
#ifdef __cplusplus
}
#endif
#endif
