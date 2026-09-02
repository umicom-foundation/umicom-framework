/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/abi_requirement.c
 *
 * PURPOSE:
 *   Implement describe minimum and maximum abi compatibility accepted by a consumer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/abi_requirement.h"
#include <string.h>
/*
 * Initialise sdk runtime abi requirement from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_abi_requirement_init(UmiSdkRuntimeAbiRequirement *value, const char *id)
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
 * Check that sdk runtime abi requirement satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_abi_requirement_validate(const UmiSdkRuntimeAbiRequirement *value)
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
 * Provide the sdk runtime abi requirement set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_path(UmiSdkRuntimeAbiRequirement *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime abi requirement set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_detail(UmiSdkRuntimeAbiRequirement *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime abi requirement set minimum abi operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_minimum_abi(UmiSdkRuntimeAbiRequirement *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->minimum_abi = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime abi requirement set maximum abi operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_maximum_abi(UmiSdkRuntimeAbiRequirement *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->maximum_abi = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime abi requirement set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_state(UmiSdkRuntimeAbiRequirement *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime abi requirement same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_abi_requirement_same_identity(const UmiSdkRuntimeAbiRequirement *left, const UmiSdkRuntimeAbiRequirement *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
