/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/compatibility.h
 *
 * PURPOSE:
 *   Record deterministic compatibility decisions and explanatory evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPATIBILITY
#define UMICOM_SDK_RUNTIME_COMPATIBILITY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeCompatibility {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t compatible;
    uint64_t reason_code;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCompatibility;
void umi_sdk_runtime_compatibility_init(UmiSdkRuntimeCompatibility *value, const char *id);
UmiStatus umi_sdk_runtime_compatibility_validate(const UmiSdkRuntimeCompatibility *value);
UmiStatus umi_sdk_runtime_compatibility_set_path(UmiSdkRuntimeCompatibility *value, const char *path);
UmiStatus umi_sdk_runtime_compatibility_set_detail(UmiSdkRuntimeCompatibility *value, const char *detail);
UmiStatus umi_sdk_runtime_compatibility_set_compatible(UmiSdkRuntimeCompatibility *value, uint64_t number);
UmiStatus umi_sdk_runtime_compatibility_set_reason_code(UmiSdkRuntimeCompatibility *value, uint64_t number);
UmiStatus umi_sdk_runtime_compatibility_set_state(UmiSdkRuntimeCompatibility *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_compatibility_same_identity(const UmiSdkRuntimeCompatibility *left, const UmiSdkRuntimeCompatibility *right);
#ifdef __cplusplus
}
#endif
#endif
