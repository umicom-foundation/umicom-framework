/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/library_root.h
 *
 * PURPOSE:
 *   Describe static/shared library roots and target mapping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LIBRARY_ROOT
#define UMICOM_SDK_RUNTIME_LIBRARY_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime library root data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeLibraryRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t library_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeLibraryRoot;
/**
 * Initialise sdk runtime library root from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_library_root_init(UmiSdkRuntimeLibraryRoot *value,const char *id);
/**
 * Check that sdk runtime library root satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_library_root_validate(const UmiSdkRuntimeLibraryRoot *value);
/**
 * Provide the sdk runtime library root set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_library_root_set_path(UmiSdkRuntimeLibraryRoot *value,const char *path);
/**
 * Provide the sdk runtime library root set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_library_root_set_detail(UmiSdkRuntimeLibraryRoot *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime library root set library without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_library_root_set_library_count(UmiSdkRuntimeLibraryRoot *value,uint64_t number);
/**
 * Provide the sdk runtime library root set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_library_root_set_generation(UmiSdkRuntimeLibraryRoot *value,uint64_t number);
/**
 * Provide the sdk runtime library root set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_library_root_set_state(UmiSdkRuntimeLibraryRoot *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime library root same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_library_root_same_identity(const UmiSdkRuntimeLibraryRoot *left,const UmiSdkRuntimeLibraryRoot *right);
#ifdef __cplusplus
}
#endif
#endif
