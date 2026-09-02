/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_channel.h
 *
 * PURPOSE:
 *   Describe stable, preview and development runtime channels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_CHANNEL
#define UMICOM_SDK_RUNTIME_PACKAGE_CHANNEL
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime package channel data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimePackageChannel
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimePackageChannel;
/**
 * Initialise sdk runtime package channel from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_channel_init(UmiSdkRuntimePackageChannel *value,const char *id);
/**
 * Check that sdk runtime package channel satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_channel_validate(const UmiSdkRuntimePackageChannel *value);
/**
 * Provide the sdk runtime package channel set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_path(UmiSdkRuntimePackageChannel *value,const char *path);
/**
 * Provide the sdk runtime package channel set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_detail(UmiSdkRuntimePackageChannel *value,const char *detail);
/**
 * Provide the sdk runtime package channel set priority operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_priority(UmiSdkRuntimePackageChannel *value,uint64_t number);
/**
 * Provide the sdk runtime package channel set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_generation(UmiSdkRuntimePackageChannel *value,uint64_t number);
/**
 * Provide the sdk runtime package channel set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_channel_set_state(UmiSdkRuntimePackageChannel *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime package channel same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_package_channel_same_identity(const UmiSdkRuntimePackageChannel *left,const UmiSdkRuntimePackageChannel *right);
#ifdef __cplusplus
}
#endif
#endif
