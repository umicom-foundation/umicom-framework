/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_scheduler.c
 *
 * PURPOSE:
 *   Verify the scheduler contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/scheduler.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeScheduler value;
    UmiTestRuntimeScheduler same;
    uint64_t revision;
    umi_test_runtime_scheduler_init(&value, "test-runtime.scheduler");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_scheduler_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_scheduler_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_scheduler_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_scheduler_set_queued_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_scheduler_set_running_count(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_scheduler_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.queued_count == 7U);
    assert(value.running_count == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_scheduler_init(&same, "test-runtime.scheduler");
    assert(umi_test_runtime_scheduler_same_identity(&value, &same));
    assert(umi_test_runtime_scheduler_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
