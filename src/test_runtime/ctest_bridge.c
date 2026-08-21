/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/ctest_bridge.c
 *
 * PURPOSE:
 *   Implement translate ctest registration and execution evidence into stable framework records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/ctest_bridge.h"
#include <string.h>

void umi_test_runtime_ctest_bridge_init(UmiTestRuntimeCtestBridge *value, const char *id)
{
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->enabled = true;
    value->revision = 1U;
    if (id != NULL) (void)umi_test_runtime_copy_text(value->id, sizeof(value->id), id);
}

UmiStatus umi_test_runtime_ctest_bridge_validate(const UmiTestRuntimeCtestBridge *value)
{
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_test_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_ctest_bridge_set_name(UmiTestRuntimeCtestBridge *value, const char *name)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->name, sizeof(value->name), name);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_test_runtime_ctest_bridge_set_detail(UmiTestRuntimeCtestBridge *value, const char *detail)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->detail, sizeof(value->detail), detail);
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

UmiStatus umi_test_runtime_ctest_bridge_set_registered_count(UmiTestRuntimeCtestBridge *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->registered_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_ctest_bridge_set_executed_count(UmiTestRuntimeCtestBridge *value, uint64_t number)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->executed_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_runtime_ctest_bridge_touch(UmiTestRuntimeCtestBridge *value, uint64_t updated_at_ms)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->updated_at_ms = updated_at_ms;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

bool umi_test_runtime_ctest_bridge_same_identity(const UmiTestRuntimeCtestBridge *left, const UmiTestRuntimeCtestBridge *right)
{
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
