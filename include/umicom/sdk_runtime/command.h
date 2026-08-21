/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/command.h
 *
 * PURPOSE:
 *   Describe typed Master Controller SDK/runtime commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMMAND
#define UMICOM_SDK_RUNTIME_COMMAND
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeCommand {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t kind;
    uint64_t sequence;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCommand;
void umi_sdk_runtime_command_init(UmiSdkRuntimeCommand *value, const char *id);
UmiStatus umi_sdk_runtime_command_validate(const UmiSdkRuntimeCommand *value);
UmiStatus umi_sdk_runtime_command_set_path(UmiSdkRuntimeCommand *value, const char *path);
UmiStatus umi_sdk_runtime_command_set_detail(UmiSdkRuntimeCommand *value, const char *detail);
UmiStatus umi_sdk_runtime_command_set_kind(UmiSdkRuntimeCommand *value, uint64_t number);
UmiStatus umi_sdk_runtime_command_set_sequence(UmiSdkRuntimeCommand *value, uint64_t number);
UmiStatus umi_sdk_runtime_command_set_state(UmiSdkRuntimeCommand *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_command_same_identity(const UmiSdkRuntimeCommand *left, const UmiSdkRuntimeCommand *right);
#ifdef __cplusplus
}
#endif
#endif
