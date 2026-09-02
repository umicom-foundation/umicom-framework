/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_ctest_bridge.c
 *
 * PURPOSE:
 *   Verify the ctest bridge contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/ctest_bridge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeCtestBridge value;
    UmiTestRuntimeCtestBridge same;
    uint64_t revision;
    umi_test_runtime_ctest_bridge_init(&value, "test-runtime.ctest_bridge");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_ctest_bridge_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_ctest_bridge_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_bridge_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_bridge_set_registered_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_bridge_set_executed_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_bridge_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.registered_count == 7U);
    assert(value.executed_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_ctest_bridge_init(&same, "test-runtime.ctest_bridge");
    assert(umi_test_runtime_ctest_bridge_same_identity(&value, &same));
    assert(umi_test_runtime_ctest_bridge_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
