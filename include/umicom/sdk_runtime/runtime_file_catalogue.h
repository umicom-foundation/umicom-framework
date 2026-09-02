/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_file_catalogue.h
 *
 * PURPOSE:
 *   Maintain the runtime-file inventory for clean-machine validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_FILE_CATALOGUE
#define UMICOM_SDK_RUNTIME_RUNTIME_FILE_CATALOGUE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime runtime file catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeRuntimeFileCatalogue
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t file_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeRuntimeFileCatalogue;
/**
 * Initialise sdk runtime runtime file catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_runtime_file_catalogue_init(UmiSdkRuntimeRuntimeFileCatalogue *value,const char *id);
/**
 * Check that sdk runtime runtime file catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_sdk_runtime_runtime_file_catalogue_validate(const UmiSdkRuntimeRuntimeFileCatalogue *value);
/**
 * Provide the sdk runtime runtime file catalogue set path operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_path(UmiSdkRuntimeRuntimeFileCatalogue *value,const char *path);
/**
 * Provide the sdk runtime runtime file catalogue set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_detail(UmiSdkRuntimeRuntimeFileCatalogue *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime runtime file catalogue set file
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_file_count(UmiSdkRuntimeRuntimeFileCatalogue *value,uint64_t number);
/**
 * Provide the sdk runtime runtime file catalogue set generation operation used by this
 * module and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_generation(UmiSdkRuntimeRuntimeFileCatalogue *value,uint64_t number);
/**
 * Provide the sdk runtime runtime file catalogue set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_file_catalogue_set_state(UmiSdkRuntimeRuntimeFileCatalogue *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime runtime file catalogue same identity operation used by this
 * module and its client applications.
 */
bool umi_sdk_runtime_runtime_file_catalogue_same_identity(const UmiSdkRuntimeRuntimeFileCatalogue *left,const UmiSdkRuntimeRuntimeFileCatalogue *right);
#ifdef __cplusplus
}
#endif
#endif
