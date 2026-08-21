/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/platform_profile.c
 *
 * PURPOSE:
 *   Implement describe windows ucrt64, linux and future platform runtime expectations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/platform_profile.h"
#include <string.h>
void umi_sdk_runtime_platform_profile_init(UmiSdkRuntimePlatformProfile *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
UmiStatus umi_sdk_runtime_platform_profile_validate(const UmiSdkRuntimePlatformProfile *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_sdk_runtime_platform_profile_set_path(UmiSdkRuntimePlatformProfile *value, const char *path)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_platform_profile_set_detail(UmiSdkRuntimePlatformProfile *value, const char *detail)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_platform_profile_set_architecture_bits(UmiSdkRuntimePlatformProfile *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->architecture_bits = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_platform_profile_set_dependency_count(UmiSdkRuntimePlatformProfile *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->dependency_count = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_platform_profile_set_state(UmiSdkRuntimePlatformProfile *value, UmiSdkRuntimeState state)
{
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
bool umi_sdk_runtime_platform_profile_same_identity(const UmiSdkRuntimePlatformProfile *left, const UmiSdkRuntimePlatformProfile *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
