/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/health.h
 *
 * PURPOSE:
 *   Summarise installed SDK/runtime readiness for applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_HEALTH
#define UMICOM_SDK_RUNTIME_HEALTH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeHealth {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t ready_components;
    uint64_t blocked_components;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeHealth;
void umi_sdk_runtime_health_init(UmiSdkRuntimeHealth *value, const char *id);
UmiStatus umi_sdk_runtime_health_validate(const UmiSdkRuntimeHealth *value);
UmiStatus umi_sdk_runtime_health_set_path(UmiSdkRuntimeHealth *value, const char *path);
UmiStatus umi_sdk_runtime_health_set_detail(UmiSdkRuntimeHealth *value, const char *detail);
UmiStatus umi_sdk_runtime_health_set_ready_components(UmiSdkRuntimeHealth *value, uint64_t number);
UmiStatus umi_sdk_runtime_health_set_blocked_components(UmiSdkRuntimeHealth *value, uint64_t number);
UmiStatus umi_sdk_runtime_health_set_state(UmiSdkRuntimeHealth *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_health_same_identity(const UmiSdkRuntimeHealth *left, const UmiSdkRuntimeHealth *right);
#ifdef __cplusplus
}
#endif
#endif
