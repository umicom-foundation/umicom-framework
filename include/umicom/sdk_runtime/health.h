/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/health.h
 *
 * PURPOSE:
 *   Summarise installed SDK/runtime readiness for applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_HEALTH
#define UMICOM_SDK_RUNTIME_HEALTH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime health data shared with callers of this public contract.
 */
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
/**
 * Initialise sdk runtime health from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_health_init(UmiSdkRuntimeHealth *value, const char *id);
/**
 * Check that sdk runtime health satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_health_validate(const UmiSdkRuntimeHealth *value);
/**
 * Provide the sdk runtime health set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_path(UmiSdkRuntimeHealth *value, const char *path);
/**
 * Provide the sdk runtime health set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_detail(UmiSdkRuntimeHealth *value, const char *detail);
/**
 * Provide the sdk runtime health set ready components operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_health_set_ready_components(UmiSdkRuntimeHealth *value, uint64_t number);
/**
 * Provide the sdk runtime health set blocked components operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_health_set_blocked_components(UmiSdkRuntimeHealth *value, uint64_t number);
/**
 * Provide the sdk runtime health set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_state(UmiSdkRuntimeHealth *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime health same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_health_same_identity(const UmiSdkRuntimeHealth *left, const UmiSdkRuntimeHealth *right);
#ifdef __cplusplus
}
#endif
#endif
