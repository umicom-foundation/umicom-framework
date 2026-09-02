/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/service.h
 *
 * PURPOSE:
 *   Own installed package catalogues, resolver state and evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SERVICE
#define UMICOM_SDK_RUNTIME_SERVICE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime service data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeService {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t lifecycle_state_code;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeService;
/**
 * Initialise sdk runtime service from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_service_init(UmiSdkRuntimeService *value, const char *id);
/**
 * Check that sdk runtime service satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_service_validate(const UmiSdkRuntimeService *value);
/**
 * Provide the sdk runtime service set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_service_set_path(UmiSdkRuntimeService *value, const char *path);
/**
 * Provide the sdk runtime service set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_service_set_detail(UmiSdkRuntimeService *value, const char *detail);
/**
 * Provide the sdk runtime service set lifecycle state code operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_service_set_lifecycle_state_code(UmiSdkRuntimeService *value, uint64_t number);
/**
 * Provide the sdk runtime service set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_service_set_generation(UmiSdkRuntimeService *value, uint64_t number);
/**
 * Provide the sdk runtime service set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_service_set_state(UmiSdkRuntimeService *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime service same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_service_same_identity(const UmiSdkRuntimeService *left, const UmiSdkRuntimeService *right);
#ifdef __cplusplus
}
#endif
#endif
