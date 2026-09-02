/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/loader_search.h
 *
 * PURPOSE:
 *   Search runtime dependencies using explicit platform rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LOADER_SEARCH
#define UMICOM_SDK_RUNTIME_LOADER_SEARCH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime loader search data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeLoaderSearch
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t resolved_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeLoaderSearch;
/**
 * Initialise sdk runtime loader search from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_loader_search_init(UmiSdkRuntimeLoaderSearch *value,const char *id);
/**
 * Check that sdk runtime loader search satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_loader_search_validate(const UmiSdkRuntimeLoaderSearch *value);
/**
 * Provide the sdk runtime loader search set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_search_set_path(UmiSdkRuntimeLoaderSearch *value,const char *path);
/**
 * Provide the sdk runtime loader search set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_search_set_detail(UmiSdkRuntimeLoaderSearch *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime loader search set candidate
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_loader_search_set_candidate_count(UmiSdkRuntimeLoaderSearch *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime loader search set resolved
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_loader_search_set_resolved_count(UmiSdkRuntimeLoaderSearch *value,uint64_t number);
/**
 * Provide the sdk runtime loader search set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_search_set_state(UmiSdkRuntimeLoaderSearch *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime loader search same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_loader_search_same_identity(const UmiSdkRuntimeLoaderSearch *left,const UmiSdkRuntimeLoaderSearch *right);
#ifdef __cplusplus
}
#endif
#endif
