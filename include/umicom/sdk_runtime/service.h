/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/service.h
 *
 * PURPOSE:
 *   Own installed package catalogues, resolver state and evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SERVICE
#define UMICOM_SDK_RUNTIME_SERVICE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeService {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t lifecycle_state_code;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeService;
void umi_sdk_runtime_service_init(UmiSdkRuntimeService *value, const char *id);
UmiStatus umi_sdk_runtime_service_validate(const UmiSdkRuntimeService *value);
UmiStatus umi_sdk_runtime_service_set_path(UmiSdkRuntimeService *value, const char *path);
UmiStatus umi_sdk_runtime_service_set_detail(UmiSdkRuntimeService *value, const char *detail);
UmiStatus umi_sdk_runtime_service_set_lifecycle_state_code(UmiSdkRuntimeService *value, uint64_t number);
UmiStatus umi_sdk_runtime_service_set_generation(UmiSdkRuntimeService *value, uint64_t number);
UmiStatus umi_sdk_runtime_service_set_state(UmiSdkRuntimeService *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_service_same_identity(const UmiSdkRuntimeService *left, const UmiSdkRuntimeService *right);
#ifdef __cplusplus
}
#endif
#endif
