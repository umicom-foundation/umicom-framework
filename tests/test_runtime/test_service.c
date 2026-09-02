/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_service.c
 *
 * PURPOSE:
 *   Verify the service contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeService value;
    UmiTestRuntimeService same;
    uint64_t revision;
    umi_test_runtime_service_init(&value, "test-runtime.service");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_service_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_service_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_service_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_service_set_state(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_service_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_service_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.state == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_service_init(&same, "test-runtime.service");
    assert(umi_test_runtime_service_same_identity(&value, &same));
    assert(umi_test_runtime_service_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
