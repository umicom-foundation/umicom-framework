/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/controller.h
 *
 * PURPOSE:
 *   Implement the SDK Runtime Slave Controller lifecycle and command boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CONTROLLER
#define UMICOM_SDK_RUNTIME_CONTROLLER
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime controller data shared with callers of this public contract.
 */
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
/**
 * Initialise sdk runtime controller from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_controller_init(UmiSdkRuntimeController *value, const char *id);
/**
 * Check that sdk runtime controller satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_controller_validate(const UmiSdkRuntimeController *value);
/**
 * Provide the sdk runtime controller set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_controller_set_path(UmiSdkRuntimeController *value, const char *path);
/**
 * Provide the sdk runtime controller set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_controller_set_detail(UmiSdkRuntimeController *value, const char *detail);
/**
 * Provide the sdk runtime controller set lifecycle state code operation used by this
 * module and its client applications.
 */
UmiStatus umi_sdk_runtime_controller_set_lifecycle_state_code(UmiSdkRuntimeController *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime controller set command without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_controller_set_command_count(UmiSdkRuntimeController *value, uint64_t number);
/**
 * Provide the sdk runtime controller set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_controller_set_state(UmiSdkRuntimeController *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime controller same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_controller_same_identity(const UmiSdkRuntimeController *left, const UmiSdkRuntimeController *right);
#ifdef __cplusplus
}
#endif
#endif
