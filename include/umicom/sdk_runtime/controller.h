/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/controller.h
 *
 * PURPOSE:
 *   Implement the SDK Runtime Slave Controller lifecycle and command boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CONTROLLER
#define UMICOM_SDK_RUNTIME_CONTROLLER
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeController {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t lifecycle_state_code;
    uint64_t command_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeController;
void umi_sdk_runtime_controller_init(UmiSdkRuntimeController *value, const char *id);
UmiStatus umi_sdk_runtime_controller_validate(const UmiSdkRuntimeController *value);
UmiStatus umi_sdk_runtime_controller_set_path(UmiSdkRuntimeController *value, const char *path);
UmiStatus umi_sdk_runtime_controller_set_detail(UmiSdkRuntimeController *value, const char *detail);
UmiStatus umi_sdk_runtime_controller_set_lifecycle_state_code(UmiSdkRuntimeController *value, uint64_t number);
UmiStatus umi_sdk_runtime_controller_set_command_count(UmiSdkRuntimeController *value, uint64_t number);
UmiStatus umi_sdk_runtime_controller_set_state(UmiSdkRuntimeController *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_controller_same_identity(const UmiSdkRuntimeController *left, const UmiSdkRuntimeController *right);
#ifdef __cplusplus
}
#endif
#endif
