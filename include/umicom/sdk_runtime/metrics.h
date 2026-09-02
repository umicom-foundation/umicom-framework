/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/metrics.h
 *
 * PURPOSE:
 *   Aggregate package discovery, compatibility and load metrics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_METRICS
#define UMICOM_SDK_RUNTIME_METRICS
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime metrics data shared with callers of this public contract.
 */
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
/**
 * Initialise sdk runtime metrics from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_metrics_init(UmiSdkRuntimeMetrics *value, const char *id);
/**
 * Check that sdk runtime metrics satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_metrics_validate(const UmiSdkRuntimeMetrics *value);
/**
 * Provide the sdk runtime metrics set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_metrics_set_path(UmiSdkRuntimeMetrics *value, const char *path);
/**
 * Provide the sdk runtime metrics set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_metrics_set_detail(UmiSdkRuntimeMetrics *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime metrics set probe without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_metrics_set_probe_count(UmiSdkRuntimeMetrics *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime metrics set failure without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_metrics_set_failure_count(UmiSdkRuntimeMetrics *value, uint64_t number);
/**
 * Provide the sdk runtime metrics set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_metrics_set_state(UmiSdkRuntimeMetrics *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime metrics same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_metrics_same_identity(const UmiSdkRuntimeMetrics *left, const UmiSdkRuntimeMetrics *right);
#ifdef __cplusplus
}
#endif
#endif
