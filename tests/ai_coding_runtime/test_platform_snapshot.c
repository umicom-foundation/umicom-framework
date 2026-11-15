/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_platform_snapshot.c
 *
 * PURPOSE:
 *   Verify complete platform snapshot publication when any child observation refuses capture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Substitute only child read boundaries. The composition implementation and
 * its public snapshot entry point are compiled unchanged in this fixture. */
#define umi_ai_coding_agent_snapshot fixture_agent_snapshot
#define umi_ai_coding_assistant_snapshot fixture_assistant_snapshot
#define umi_ai_coding_repository_index_snapshot fixture_repository_snapshot
#define umi_ai_coding_agent_event_count fixture_event_count
#define umi_ai_coding_task_queue_count fixture_task_count
#include "../../src/ai_coding_runtime/platform.c"
#undef umi_ai_coding_agent_snapshot
#undef umi_ai_coding_assistant_snapshot
#undef umi_ai_coding_repository_index_snapshot
#undef umi_ai_coding_agent_event_count
#undef umi_ai_coding_task_queue_count
#include <stdio.h>

static unsigned calls[5];
static int failure_stage = -1;
static UmiStatus failure_status = UMI_STATUS_IO_ERROR;
static int maximum_counts;

UmiStatus fixture_agent_snapshot(const UmiAiCodingAgent *agent,
    UmiAiCodingAgentSnapshot *snapshot)
{
    (void)agent; ++calls[0]; memset(snapshot, 0, sizeof(*snapshot));
    snapshot->revision = 11U; snapshot->state = UMI_AI_CODING_RUNTIME_GENERATING;
    strcpy(snapshot->task_id, "observed-task");
    return failure_stage == 0 ? failure_status : UMI_STATUS_OK;
}
UmiStatus fixture_assistant_snapshot(const UmiAiCodingAssistantService *assistant,
    UmiAiCodingAssistantSnapshot *snapshot)
{
    (void)assistant; ++calls[1]; memset(snapshot, 0, sizeof(*snapshot));
    snapshot->revision = 22U; snapshot->task_count = 2U;
    strcpy(snapshot->last_request_id, "observed-request");
    return failure_stage == 1 ? failure_status : UMI_STATUS_OK;
}
UmiStatus fixture_repository_snapshot(const UmiAiCodingRepositoryIndex *index,
    UmiAiCodingRepositoryIndexSnapshot *snapshot)
{
    (void)index; ++calls[2]; memset(snapshot, 0, sizeof(*snapshot));
    snapshot->revision = 33U; snapshot->indexed_files = 3U;
    return failure_stage == 2 ? failure_status : UMI_STATUS_OK;
}
size_t fixture_event_count(const UmiAiCodingAgent *agent)
{
    (void)agent; ++calls[3]; return maximum_counts ? SIZE_MAX : 5U;
}
size_t fixture_task_count(const UmiAiCodingTaskQueue *queue)
{
    (void)queue; ++calls[4]; return maximum_counts ? SIZE_MAX : 6U;
}

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    UmiAiCodingRuntimePlatform *platform = calloc(1U, sizeof(*platform));
    UmiAiCodingRuntimePlatform *owner_before = malloc(sizeof(*owner_before));
    UmiAiCodingRuntimePlatformSnapshot output, before;
    UmiStatus expected = UMI_STATUS_OK;
    int rc = 1;
    CHECK(platform != NULL && owner_before != NULL && argc >= 2);
    platform->revision = 44U;
    if (strcmp(argv[1], "failure") == 0) {
        CHECK(argc == 4);
        if (strcmp(argv[2], "agent") == 0) failure_stage = 0;
        else if (strcmp(argv[2], "assistant") == 0) failure_stage = 1;
        else if (strcmp(argv[2], "repository") == 0) failure_stage = 2;
        else goto cleanup;
        if (strcmp(argv[3], "argument") == 0) failure_status = UMI_STATUS_INVALID_ARGUMENT;
        else if (strcmp(argv[3], "state") == 0) failure_status = UMI_STATUS_INVALID_STATE;
        else if (strcmp(argv[3], "io") == 0) failure_status = UMI_STATUS_IO_ERROR;
        else if (strcmp(argv[3], "capacity") == 0) failure_status = UMI_STATUS_CAPACITY_EXCEEDED;
        else goto cleanup;
        expected = failure_status;
    } else if (strcmp(argv[1], "active") == 0) platform->operation_active = 1;
    else if (strcmp(argv[1], "counts") == 0) maximum_counts = 1;
    else if (strcmp(argv[1], "null-owner") == 0 || strcmp(argv[1], "null-output") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    else if (strcmp(argv[1], "idle") != 0) goto cleanup;
    memcpy(owner_before, platform, sizeof(*owner_before));
    memset(&output, 0x5a, sizeof output); memcpy(&before, &output, sizeof before);
    CHECK(umi_ai_coding_runtime_platform_snapshot(
        strcmp(argv[1], "null-owner") == 0 ? NULL : platform,
        strcmp(argv[1], "null-output") == 0 ? NULL : &output) == expected);
    CHECK(memcmp(platform, owner_before, sizeof(*platform)) == 0);
    if (expected != UMI_STATUS_OK) {
        CHECK(memcmp(&output, &before, sizeof output) == 0);
        if (failure_stage >= 0) {
            for (int i = 0; i < 3; ++i) CHECK(calls[i] == (i <= failure_stage ? 1U : 0U));
            CHECK(calls[3] == 0U && calls[4] == 0U);
        } else {
            for (unsigned i = 0U; i < 5U; ++i) CHECK(calls[i] == 0U);
        }
    } else {
        for (unsigned i = 0U; i < 5U; ++i) CHECK(calls[i] == 1U);
        CHECK(output.agent.revision == 11U && output.assistant.revision == 22U);
        CHECK(output.repository.revision == 33U && output.revision == 44U);
        CHECK(output.agent.state == UMI_AI_CODING_RUNTIME_GENERATING);
        CHECK(strcmp(output.agent.task_id, "observed-task") == 0);
        CHECK(strcmp(output.assistant.last_request_id, "observed-request") == 0);
        CHECK(output.repository.indexed_files == 3U && output.assistant.task_count == 2U);
        CHECK(output.queued_events == (maximum_counts ? SIZE_MAX : 5U));
        CHECK(output.queued_tasks == (maximum_counts ? SIZE_MAX : 6U));
    }
    rc = 0;
cleanup:
    free(owner_before); free(platform); return rc;
}
