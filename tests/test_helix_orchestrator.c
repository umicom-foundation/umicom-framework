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
#include "value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>
#include "umicom/helix/orchestrator.h"
/*
 * Exercise execute and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus execute(void *context, const UmiHelixAction *action,
                         char *out, size_t capacity)
{
    int *count = context;
    int written;
    (*count)++;
    written = snprintf(out, capacity, "%s %s", action->action_id,
                       *count == 1 ? "failed" : "passed");
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return *count == 1 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted action value. */
static int UmiHelixActionConstructionCases(void)
{
    UmiHelixAction *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, "build", UMI_HELIX_ACTION_BUILD, "Build isolated candidate", "build/candidate", 3U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->action_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, oversized, UMI_HELIX_ACTION_BUILD, "Build isolated candidate", "build/candidate", 3U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, value->action_id, UMI_HELIX_ACTION_BUILD, "Build isolated candidate", "build/candidate", 3U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->action_id, "build") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->summary) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, "build", UMI_HELIX_ACTION_BUILD, oversized, "build/candidate", 3U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, "build", UMI_HELIX_ACTION_BUILD, value->summary, "build/candidate", 3U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->summary, "Build isolated candidate") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, "build", UMI_HELIX_ACTION_BUILD, "Build isolated candidate", oversized, 3U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_helix_action_init(value, "build", UMI_HELIX_ACTION_BUILD, "Build isolated candidate", value->target, 3U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target, "build/candidate") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiHelixActionConstructionCases() != 0) return 1;

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
