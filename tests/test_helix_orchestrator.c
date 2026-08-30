/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_orchestrator.c
 * PURPOSE: Verify human-gated action execution and evidence journalling.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/helix/orchestrator.h"
static UmiStatus execute(void *context, const UmiHelixAction *action,
                         char *out, size_t capacity)
{
    int *count = context;
    int written;
    (*count)++;
    written = snprintf(out, capacity, "%s %s", action->action_id,
                       *count == 1 ? "failed" : "passed");
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return *count == 1 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
int main(void)
{
    UmiHelixOrchestrator runtime;
    UmiHelixOrchestratorConfig config =
        umi_helix_orchestrator_config_default();
    UmiHelixExecutionAdapter adapter = {0};
    UmiHelixAction action;
    char evidence[128];
    int count = 0;
    adapter.context = &count;
    adapter.read = execute; adapter.filesystem = execute; adapter.build = execute;
    adapter.test = execute; adapter.review = execute; adapter.source_control = execute;
    assert(umi_helix_orchestrator_init(&runtime, &config) == UMI_STATUS_OK);
    assert(umi_helix_orchestrator_set_adapter(&runtime, &adapter) == UMI_STATUS_OK);
    assert(umi_helix_orchestrator_plan(
        &runtime, "op", "Build candidate", "plan123") == UMI_STATUS_OK);
    assert(umi_helix_action_init(&action, "build", UMI_HELIX_ACTION_BUILD,
        "Build isolated candidate", "build/candidate", 3U) == UMI_STATUS_OK);
    assert(umi_helix_orchestrator_prepare_action(&runtime, &action)
           == UMI_STATUS_OK);
    assert(umi_helix_orchestrator_decide(&runtime,
        UMI_HELIX_APPROVAL_APPROVED, "reviewer", "Scope reviewed")
        == UMI_STATUS_OK);
    assert(umi_helix_orchestrator_execute(&runtime, evidence, sizeof(evidence))
           == UMI_STATUS_IO_ERROR);
    assert(runtime.operation.state == UMI_HELIX_OPERATION_APPROVED);
    assert(umi_helix_orchestrator_execute(&runtime, evidence, sizeof(evidence))
           == UMI_STATUS_OK);
    assert(count == 2 && strstr(evidence, "passed") != NULL);
    assert(runtime.operation.state == UMI_HELIX_OPERATION_SUCCEEDED);
    assert(runtime.operation.retry_count == 1U);
    assert(umi_helix_journal_verify(&runtime.journal));
    return 0;
}
