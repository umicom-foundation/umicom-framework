/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/query.h
 *
 * PURPOSE:
 *   Describe read-only SDK/runtime queries and paging.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_QUERY
#define UMICOM_SDK_RUNTIME_QUERY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime query data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeQuery {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t offset;
    uint64_t limit;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeQuery;
/**
 * Initialise sdk runtime query from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_query_init(UmiSdkRuntimeQuery *value, const char *id);
/**
 * Check that sdk runtime query satisfies its contract before another service relies on it.
 */
UmiStatus umi_sdk_runtime_query_validate(const UmiSdkRuntimeQuery *value);
/**
 * Provide the sdk runtime query set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_query_set_path(UmiSdkRuntimeQuery *value, const char *path);
/**
 * Provide the sdk runtime query set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_query_set_detail(UmiSdkRuntimeQuery *value, const char *detail);
/**
 * Provide the sdk runtime query set offset operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_query_set_offset(UmiSdkRuntimeQuery *value, uint64_t number);
/**
 * Provide the sdk runtime query set limit operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_query_set_limit(UmiSdkRuntimeQuery *value, uint64_t number);
/**
 * Provide the sdk runtime query set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_query_set_state(UmiSdkRuntimeQuery *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime query same identity operation used by this module and its client
 * applications.
 */
bool umi_sdk_runtime_query_same_identity(const UmiSdkRuntimeQuery *left, const UmiSdkRuntimeQuery *right);
#ifdef __cplusplus
}
#endif
#endif
