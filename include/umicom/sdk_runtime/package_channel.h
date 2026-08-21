/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_channel.h
 *
 * PURPOSE:
 *   Describe stable, preview and development runtime channels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_CHANNEL
#define UMICOM_SDK_RUNTIME_PACKAGE_CHANNEL
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimePackageChannel
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimePackageChannel;
void umi_sdk_runtime_package_channel_init(UmiSdkRuntimePackageChannel *value,const char *id);
UmiStatus umi_sdk_runtime_package_channel_validate(const UmiSdkRuntimePackageChannel *value);
UmiStatus umi_sdk_runtime_package_channel_set_path(UmiSdkRuntimePackageChannel *value,const char *path);
UmiStatus umi_sdk_runtime_package_channel_set_detail(UmiSdkRuntimePackageChannel *value,const char *detail);
UmiStatus umi_sdk_runtime_package_channel_set_priority(UmiSdkRuntimePackageChannel *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_channel_set_generation(UmiSdkRuntimePackageChannel *value,uint64_t number);
UmiStatus umi_sdk_runtime_package_channel_set_state(UmiSdkRuntimePackageChannel *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_channel_same_identity(const UmiSdkRuntimePackageChannel *left,const UmiSdkRuntimePackageChannel *right);
#ifdef __cplusplus
}
#endif
#endif
