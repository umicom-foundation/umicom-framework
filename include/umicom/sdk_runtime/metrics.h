/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/metrics.h
 *
 * PURPOSE:
 *   Aggregate package discovery, compatibility and load metrics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_METRICS
#define UMICOM_SDK_RUNTIME_METRICS
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeMetrics {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t probe_count;
    uint64_t failure_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeMetrics;
void umi_sdk_runtime_metrics_init(UmiSdkRuntimeMetrics *value, const char *id);
UmiStatus umi_sdk_runtime_metrics_validate(const UmiSdkRuntimeMetrics *value);
UmiStatus umi_sdk_runtime_metrics_set_path(UmiSdkRuntimeMetrics *value, const char *path);
UmiStatus umi_sdk_runtime_metrics_set_detail(UmiSdkRuntimeMetrics *value, const char *detail);
UmiStatus umi_sdk_runtime_metrics_set_probe_count(UmiSdkRuntimeMetrics *value, uint64_t number);
UmiStatus umi_sdk_runtime_metrics_set_failure_count(UmiSdkRuntimeMetrics *value, uint64_t number);
UmiStatus umi_sdk_runtime_metrics_set_state(UmiSdkRuntimeMetrics *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_metrics_same_identity(const UmiSdkRuntimeMetrics *left, const UmiSdkRuntimeMetrics *right);
#ifdef __cplusplus
}
#endif
#endif
