/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/relocation.h
 *
 * PURPOSE:
 *   Validate relocatable paths after installation or package extraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RELOCATION
#define UMICOM_SDK_RUNTIME_RELOCATION
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime relocation data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeRelocation
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t checked_count;
    uint64_t absolute_path_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeRelocation;
/**
 * Initialise sdk runtime relocation from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_relocation_init(UmiSdkRuntimeRelocation *value,const char *id);
/**
 * Check that sdk runtime relocation satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_relocation_validate(const UmiSdkRuntimeRelocation *value);
/**
 * Provide the sdk runtime relocation set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_relocation_set_path(UmiSdkRuntimeRelocation *value,const char *path);
/**
 * Provide the sdk runtime relocation set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_relocation_set_detail(UmiSdkRuntimeRelocation *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime relocation set checked without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_relocation_set_checked_count(UmiSdkRuntimeRelocation *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime relocation set absolute path
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_relocation_set_absolute_path_count(UmiSdkRuntimeRelocation *value,uint64_t number);
/**
 * Provide the sdk runtime relocation set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_relocation_set_state(UmiSdkRuntimeRelocation *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime relocation same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_relocation_same_identity(const UmiSdkRuntimeRelocation *left,const UmiSdkRuntimeRelocation *right);
#ifdef __cplusplus
}
#endif
#endif
