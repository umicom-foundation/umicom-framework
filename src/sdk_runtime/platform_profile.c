/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/platform_profile.c
 *
 * PURPOSE:
 *   Implement describe windows ucrt64, linux and future platform runtime expectations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/platform_profile.h"
#include <string.h>
/*
 * Initialise sdk runtime platform profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_platform_profile_init(UmiSdkRuntimePlatformProfile *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
/*
 * Check that sdk runtime platform profile satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_platform_profile_validate(const UmiSdkRuntimePlatformProfile *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime platform profile set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_platform_profile_set_path(UmiSdkRuntimePlatformProfile *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime platform profile set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_platform_profile_set_detail(UmiSdkRuntimePlatformProfile *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime platform profile set architecture bits operation used by this
 * module and its client applications.
 */
UmiStatus umi_sdk_runtime_platform_profile_set_architecture_bits(UmiSdkRuntimePlatformProfile *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->architecture_bits = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Return the number of records represented by sdk runtime platform profile set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_platform_profile_set_dependency_count(UmiSdkRuntimePlatformProfile *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->dependency_count = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime platform profile set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_platform_profile_set_state(UmiSdkRuntimePlatformProfile *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime platform profile same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_platform_profile_same_identity(const UmiSdkRuntimePlatformProfile *left, const UmiSdkRuntimePlatformProfile *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
