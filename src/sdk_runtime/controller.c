/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/controller.c
 *
 * PURPOSE:
 *   Implement implement the sdk runtime slave controller lifecycle and command boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/controller.h"
#include <string.h>
void umi_sdk_runtime_controller_init(UmiSdkRuntimeController *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
UmiStatus umi_sdk_runtime_controller_validate(const UmiSdkRuntimeController *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_sdk_runtime_controller_set_path(UmiSdkRuntimeController *value, const char *path)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_controller_set_detail(UmiSdkRuntimeController *value, const char *detail)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_controller_set_lifecycle_state_code(UmiSdkRuntimeController *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->lifecycle_state_code = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_controller_set_command_count(UmiSdkRuntimeController *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->command_count = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_controller_set_state(UmiSdkRuntimeController *value, UmiSdkRuntimeState state)
{
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
bool umi_sdk_runtime_controller_same_identity(const UmiSdkRuntimeController *left, const UmiSdkRuntimeController *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
