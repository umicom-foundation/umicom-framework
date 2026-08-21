/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_environment.h
 *
 * PURPOSE:
 *   Build the runtime environment needed by installed applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_ENVIRONMENT
#define UMICOM_SDK_RUNTIME_RUNTIME_ENVIRONMENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeRuntimeEnvironment {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t path_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeRuntimeEnvironment;
void umi_sdk_runtime_runtime_environment_init(UmiSdkRuntimeRuntimeEnvironment *value, const char *id);
UmiStatus umi_sdk_runtime_runtime_environment_validate(const UmiSdkRuntimeRuntimeEnvironment *value);
UmiStatus umi_sdk_runtime_runtime_environment_set_path(UmiSdkRuntimeRuntimeEnvironment *value, const char *path);
UmiStatus umi_sdk_runtime_runtime_environment_set_detail(UmiSdkRuntimeRuntimeEnvironment *value, const char *detail);
UmiStatus umi_sdk_runtime_runtime_environment_set_entry_count(UmiSdkRuntimeRuntimeEnvironment *value, uint64_t number);
UmiStatus umi_sdk_runtime_runtime_environment_set_path_count(UmiSdkRuntimeRuntimeEnvironment *value, uint64_t number);
UmiStatus umi_sdk_runtime_runtime_environment_set_state(UmiSdkRuntimeRuntimeEnvironment *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_runtime_environment_same_identity(const UmiSdkRuntimeRuntimeEnvironment *left, const UmiSdkRuntimeRuntimeEnvironment *right);
#ifdef __cplusplus
}
#endif
#endif
