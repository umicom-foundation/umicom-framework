/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_health.c
 *
 * PURPOSE:
 *   Verify the health contract, bounded text and revision behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/health.h"

int main(void)
{
    UmiTestRuntimeHealth value;
    UmiTestRuntimeHealth same;
    uint64_t revision;
    umi_test_runtime_health_init(&value, "test-runtime.health");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_health_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_health_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_health_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_health_set_ready_tests(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_health_set_blocked_tests(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_health_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.ready_tests == 7U);
    assert(value.blocked_tests == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_health_init(&same, "test-runtime.health");
    assert(umi_test_runtime_health_same_identity(&value, &same));
    assert(umi_test_runtime_health_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
