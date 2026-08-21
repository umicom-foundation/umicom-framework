/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/event.h
 *
 * PURPOSE:
 *   Describe typed SDK/runtime lifecycle events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_EVENT
#define UMICOM_SDK_RUNTIME_EVENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeEvent {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t kind;
    uint64_t sequence;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeEvent;
void umi_sdk_runtime_event_init(UmiSdkRuntimeEvent *value, const char *id);
UmiStatus umi_sdk_runtime_event_validate(const UmiSdkRuntimeEvent *value);
UmiStatus umi_sdk_runtime_event_set_path(UmiSdkRuntimeEvent *value, const char *path);
UmiStatus umi_sdk_runtime_event_set_detail(UmiSdkRuntimeEvent *value, const char *detail);
UmiStatus umi_sdk_runtime_event_set_kind(UmiSdkRuntimeEvent *value, uint64_t number);
UmiStatus umi_sdk_runtime_event_set_sequence(UmiSdkRuntimeEvent *value, uint64_t number);
UmiStatus umi_sdk_runtime_event_set_state(UmiSdkRuntimeEvent *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_event_same_identity(const UmiSdkRuntimeEvent *left, const UmiSdkRuntimeEvent *right);
#ifdef __cplusplus
}
#endif
#endif
