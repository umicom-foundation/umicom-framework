/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/path_normalisation.h
 *
 * PURPOSE:
 *   Normalise package paths for stable comparison and relocation checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PATH_NORMALISATION
#define UMICOM_SDK_RUNTIME_PATH_NORMALISATION
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime path normalisation data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimePathNormalisation
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t input_count;
    uint64_t changed_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimePathNormalisation;
/**
 * Initialise sdk runtime path normalisation from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_path_normalisation_init(UmiSdkRuntimePathNormalisation *value,const char *id);
/**
 * Check that sdk runtime path normalisation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_path_normalisation_validate(const UmiSdkRuntimePathNormalisation *value);
/**
 * Provide the sdk runtime path normalisation set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_path_normalisation_set_path(UmiSdkRuntimePathNormalisation *value,const char *path);
/**
 * Provide the sdk runtime path normalisation set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_path_normalisation_set_detail(UmiSdkRuntimePathNormalisation *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime path normalisation set input
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_path_normalisation_set_input_count(UmiSdkRuntimePathNormalisation *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime path normalisation set changed
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_path_normalisation_set_changed_count(UmiSdkRuntimePathNormalisation *value,uint64_t number);
/**
 * Provide the sdk runtime path normalisation set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_path_normalisation_set_state(UmiSdkRuntimePathNormalisation *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime path normalisation same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_path_normalisation_same_identity(const UmiSdkRuntimePathNormalisation *left,const UmiSdkRuntimePathNormalisation *right);
#ifdef __cplusplus
}
#endif
#endif
