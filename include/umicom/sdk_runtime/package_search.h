/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_search.h
 *
 * PURPOSE:
 *   Search ordered SDK/package roots and retain selection evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_SEARCH
#define UMICOM_SDK_RUNTIME_PACKAGE_SEARCH
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime package search data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimePackageSearch
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t matched_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimePackageSearch;
/**
 * Initialise sdk runtime package search from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_search_init(UmiSdkRuntimePackageSearch *value,const char *id);
/**
 * Check that sdk runtime package search satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_search_validate(const UmiSdkRuntimePackageSearch *value);
/**
 * Provide the sdk runtime package search set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_search_set_path(UmiSdkRuntimePackageSearch *value,const char *path);
/**
 * Provide the sdk runtime package search set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_search_set_detail(UmiSdkRuntimePackageSearch *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime package search set candidate
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_search_set_candidate_count(UmiSdkRuntimePackageSearch *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime package search set matched
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_search_set_matched_count(UmiSdkRuntimePackageSearch *value,uint64_t number);
/**
 * Provide the sdk runtime package search set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_search_set_state(UmiSdkRuntimePackageSearch *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime package search same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_package_search_same_identity(const UmiSdkRuntimePackageSearch *left,const UmiSdkRuntimePackageSearch *right);
#ifdef __cplusplus
}
#endif
#endif
