/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_resolver.h
 *
 * PURPOSE:
 *   Resolve installed Framework components from explicit package roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_RESOLVER
#define UMICOM_SDK_RUNTIME_RUNTIME_RESOLVER
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime runtime resolver data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeRuntimeResolver {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t resolved_count;
    uint64_t missing_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeRuntimeResolver;
/**
 * Initialise sdk runtime runtime resolver from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_runtime_resolver_init(UmiSdkRuntimeRuntimeResolver *value, const char *id);
/**
 * Check that sdk runtime runtime resolver satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_runtime_resolver_validate(const UmiSdkRuntimeRuntimeResolver *value);
/**
 * Provide the sdk runtime runtime resolver set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_resolver_set_path(UmiSdkRuntimeRuntimeResolver *value, const char *path);
/**
 * Provide the sdk runtime runtime resolver set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_resolver_set_detail(UmiSdkRuntimeRuntimeResolver *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime runtime resolver set resolved
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_resolver_set_resolved_count(UmiSdkRuntimeRuntimeResolver *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime runtime resolver set missing
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_resolver_set_missing_count(UmiSdkRuntimeRuntimeResolver *value, uint64_t number);
/**
 * Provide the sdk runtime runtime resolver set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_resolver_set_state(UmiSdkRuntimeRuntimeResolver *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime runtime resolver same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_runtime_resolver_same_identity(const UmiSdkRuntimeRuntimeResolver *left, const UmiSdkRuntimeRuntimeResolver *right);
#ifdef __cplusplus
}
#endif
#endif
