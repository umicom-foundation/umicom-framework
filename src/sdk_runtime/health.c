/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/health.c
 *
 * PURPOSE:
 *   Implement summarise installed sdk/runtime readiness for applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/health.h"
#include <string.h>
/*
 * Initialise sdk runtime health from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_health_init(UmiSdkRuntimeHealth *value, const char *id)
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
 * Check that sdk runtime health satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_health_validate(const UmiSdkRuntimeHealth *value)
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
 * Provide the sdk runtime health set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_path(UmiSdkRuntimeHealth *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime health set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_detail(UmiSdkRuntimeHealth *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime health set ready components operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_health_set_ready_components(UmiSdkRuntimeHealth *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->ready_components = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime health set blocked components operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_health_set_blocked_components(UmiSdkRuntimeHealth *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->blocked_components = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime health set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_state(UmiSdkRuntimeHealth *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime health same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_health_same_identity(const UmiSdkRuntimeHealth *left, const UmiSdkRuntimeHealth *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
