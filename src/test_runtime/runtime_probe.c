/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_probe.c
 *
 * PURPOSE:
 *   Implement probe compiler runtime, build output and platform dependency directories.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_probe.h"
#include <string.h>

void umi_test_runtime_runtime_probe_init(UmiTestRuntimeRuntimeProbe *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_test_runtime_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_test_runtime_runtime_probe_validate(const UmiTestRuntimeRuntimeProbe *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_runtime_probe_set_name(UmiTestRuntimeRuntimeProbe *value, const char *name)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->name, sizeof(value->name), name);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_test_runtime_runtime_probe_set_detail(UmiTestRuntimeRuntimeProbe *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_test_runtime_runtime_probe_set_probe_count(UmiTestRuntimeRuntimeProbe *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->probe_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_runtime_probe_set_failure_count(UmiTestRuntimeRuntimeProbe *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->failure_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_runtime_probe_touch(UmiTestRuntimeRuntimeProbe *value, uint64_t updated_at_ms)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->updated_at_ms = updated_at_ms;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_test_runtime_runtime_probe_same_identity(const UmiTestRuntimeRuntimeProbe *left, const UmiTestRuntimeRuntimeProbe *right)
{
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
