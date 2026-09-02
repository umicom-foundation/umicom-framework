/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/component.c
 *
 * PURPOSE:
 *   Implement describe one installed framework component and its stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/component.h"
#include <string.h>
/*
 * Initialise sdk runtime component from caller-provided values so later operations receive
 * a known state.
 */
void umi_sdk_runtime_component_init(UmiSdkRuntimeComponent *value, const char *id)
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
 * Check that sdk runtime component satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_component_validate(const UmiSdkRuntimeComponent *value)
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
 * Provide the sdk runtime component set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_component_set_path(UmiSdkRuntimeComponent *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime component set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_component_set_detail(UmiSdkRuntimeComponent *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime component set abi version operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_component_set_abi_version(UmiSdkRuntimeComponent *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->abi_version = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime component set package version operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_component_set_package_version(UmiSdkRuntimeComponent *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->package_version = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime component set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_component_set_state(UmiSdkRuntimeComponent *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime component same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_component_same_identity(const UmiSdkRuntimeComponent *left, const UmiSdkRuntimeComponent *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
