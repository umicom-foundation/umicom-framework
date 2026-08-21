/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/process_environment.c
 *
 * PURPOSE:
 *   Implement build the exact environment overlay inherited by child processes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/process_environment.h"
#include <string.h>

void umi_test_runtime_process_environment_init(UmiTestRuntimeProcessEnvironment *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_test_runtime_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_test_runtime_process_environment_validate(const UmiTestRuntimeProcessEnvironment *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_process_environment_set_name(UmiTestRuntimeProcessEnvironment *value, const char *name)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->name, sizeof(value->name), name);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_test_runtime_process_environment_set_detail(UmiTestRuntimeProcessEnvironment *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_test_runtime_process_environment_set_entry_count(UmiTestRuntimeProcessEnvironment *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->entry_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_process_environment_set_path_entry_count(UmiTestRuntimeProcessEnvironment *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->path_entry_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_process_environment_touch(UmiTestRuntimeProcessEnvironment *value, uint64_t updated_at_ms)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->updated_at_ms = updated_at_ms;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_test_runtime_process_environment_same_identity(const UmiTestRuntimeProcessEnvironment *left, const UmiTestRuntimeProcessEnvironment *right)
{
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
