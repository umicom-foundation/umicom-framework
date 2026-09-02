/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/event.h
 *
 * PURPOSE:
 *   Describe typed SDK/runtime lifecycle events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_EVENT
#define UMICOM_SDK_RUNTIME_EVENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime event data shared with callers of this public contract.
 */
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
/**
 * Initialise sdk runtime event from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_event_init(UmiSdkRuntimeEvent *value, const char *id);
/**
 * Check that sdk runtime event satisfies its contract before another service relies on it.
 */
UmiStatus umi_sdk_runtime_event_validate(const UmiSdkRuntimeEvent *value);
/**
 * Provide the sdk runtime event set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_event_set_path(UmiSdkRuntimeEvent *value, const char *path);
/**
 * Provide the sdk runtime event set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_event_set_detail(UmiSdkRuntimeEvent *value, const char *detail);
/**
 * Provide the sdk runtime event set kind operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_event_set_kind(UmiSdkRuntimeEvent *value, uint64_t number);
/**
 * Provide the sdk runtime event set sequence operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_event_set_sequence(UmiSdkRuntimeEvent *value, uint64_t number);
/**
 * Provide the sdk runtime event set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_event_set_state(UmiSdkRuntimeEvent *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime event same identity operation used by this module and its client
 * applications.
 */
bool umi_sdk_runtime_event_same_identity(const UmiSdkRuntimeEvent *left, const UmiSdkRuntimeEvent *right);
#ifdef __cplusplus
}
#endif
#endif
