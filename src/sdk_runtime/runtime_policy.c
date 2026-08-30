/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/runtime_policy.c
 *
 * PURPOSE:
 *   Implement define source-build, installed-sdk and self-contained runtime policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/runtime_policy.h"
#include <string.h>
void umi_sdk_runtime_runtime_policy_init(UmiSdkRuntimeRuntimePolicy *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
UmiStatus umi_sdk_runtime_runtime_policy_validate(const UmiSdkRuntimeRuntimePolicy *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_sdk_runtime_runtime_policy_set_path(UmiSdkRuntimeRuntimePolicy *value, const char *path)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_runtime_policy_set_detail(UmiSdkRuntimeRuntimePolicy *value, const char *detail)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_runtime_policy_set_allow_source(UmiSdkRuntimeRuntimePolicy *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->allow_source = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_runtime_policy_set_require_installed(UmiSdkRuntimeRuntimePolicy *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->require_installed = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_runtime_policy_set_state(UmiSdkRuntimeRuntimePolicy *value, UmiSdkRuntimeState state)
{
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
bool umi_sdk_runtime_runtime_policy_same_identity(const UmiSdkRuntimeRuntimePolicy *left, const UmiSdkRuntimeRuntimePolicy *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
