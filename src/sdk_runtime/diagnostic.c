/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/diagnostic.c
 *
 * PURPOSE:
 *   Implement represent actionable sdk/runtime diagnostic evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/diagnostic.h"
#include <string.h>
void umi_sdk_runtime_diagnostic_init(UmiSdkRuntimeDiagnostic *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
UmiStatus umi_sdk_runtime_diagnostic_validate(const UmiSdkRuntimeDiagnostic *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_sdk_runtime_diagnostic_set_path(UmiSdkRuntimeDiagnostic *value, const char *path)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_diagnostic_set_detail(UmiSdkRuntimeDiagnostic *value, const char *detail)
{
    UmiStatus status; if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
UmiStatus umi_sdk_runtime_diagnostic_set_severity(UmiSdkRuntimeDiagnostic *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->severity = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_diagnostic_set_sequence(UmiSdkRuntimeDiagnostic *value, uint64_t number)
{ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->sequence = number; value->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_sdk_runtime_diagnostic_set_state(UmiSdkRuntimeDiagnostic *value, UmiSdkRuntimeState state)
{
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
bool umi_sdk_runtime_diagnostic_same_identity(const UmiSdkRuntimeDiagnostic *left, const UmiSdkRuntimeDiagnostic *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
