/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_platform_revisions.c
 *
 * PURPOSE:
 *   Verify failure-aware parent revisions, semantic snapshot comparison and cancellation during reserved operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Substitute child boundaries while compiling the actual platform owner.
 * No fixture invokes providers, repository I/O or a real task executor. */
#define UmiAiCodingAgentCheckIdle fixture_agent_idle
#define UmiAiCodingAssistantCheckIdle fixture_assistant_idle
#define umi_ai_coding_agent_snapshot fixture_agent_snapshot
#define umi_ai_coding_assistant_snapshot fixture_assistant_snapshot
#define umi_ai_coding_repository_index_snapshot fixture_repository_snapshot
#define umi_ai_coding_agent_event_count fixture_event_count
#define umi_ai_coding_task_queue_count fixture_task_count
#define umi_ai_coding_repository_index_refresh fixture_refresh
#define umi_ai_coding_agent_set_validation_plan fixture_set_plan
#define umi_ai_coding_validation_select fixture_select
#define umi_ai_coding_agent_run fixture_run
#define umi_ai_coding_task_queue_push fixture_enqueue
#define umi_ai_coding_task_queue_pop fixture_pop
#define umi_ai_coding_agent_approve_pending fixture_approve
#define umi_ai_coding_agent_apply_pending fixture_apply
#define umi_ai_coding_agent_continue_repair fixture_repair
#define umi_ai_coding_agent_reject_pending fixture_reject
#define umi_ai_coding_agent_cancel fixture_cancel
#include "../../src/ai_coding_runtime/platform.c"
#undef UmiAiCodingAgentCheckIdle
#undef UmiAiCodingAssistantCheckIdle
#undef umi_ai_coding_agent_snapshot
#undef umi_ai_coding_assistant_snapshot
#undef umi_ai_coding_repository_index_snapshot
#undef umi_ai_coding_agent_event_count
#undef umi_ai_coding_task_queue_count
#undef umi_ai_coding_repository_index_refresh
#undef umi_ai_coding_agent_set_validation_plan
#undef umi_ai_coding_validation_select
#undef umi_ai_coding_agent_run
#undef umi_ai_coding_task_queue_push
#undef umi_ai_coding_task_queue_pop
#undef umi_ai_coding_agent_approve_pending
#undef umi_ai_coding_agent_apply_pending
#undef umi_ai_coding_agent_continue_repair
#undef umi_ai_coding_agent_reject_pending
#undef umi_ai_coding_agent_cancel
#include <stdio.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

static struct {
    UmiAiCodingRuntimePlatform *parent;
    UmiAiCodingRuntimePlatformSnapshot children;
    UmiStatus action_status, capture_status;
    int mutate, cancel_during, post_capture_failure, child_busy;
    int calls, cancellations, callback_failure;
    uint64_t admitted_revision;
} parent_fixture;

UmiStatus fixture_agent_idle(const UmiAiCodingAgent *agent)
{
    (void)agent; return parent_fixture.child_busy ? UMI_STATUS_BUSY : UMI_STATUS_OK;
}
UmiStatus fixture_assistant_idle(const UmiAiCodingAssistantService *assistant)
{
    (void)assistant; return UMI_STATUS_OK;
}
UmiStatus fixture_agent_snapshot(const UmiAiCodingAgent *agent, UmiAiCodingAgentSnapshot *out)
{
    (void)agent;
    if (parent_fixture.capture_status != UMI_STATUS_OK) return parent_fixture.capture_status;
    if (parent_fixture.post_capture_failure && parent_fixture.calls != 0) return UMI_STATUS_IO_ERROR;
    *out = parent_fixture.children.agent; return UMI_STATUS_OK;
}
UmiStatus fixture_assistant_snapshot(const UmiAiCodingAssistantService *assistant, UmiAiCodingAssistantSnapshot *out)
{
    (void)assistant; *out = parent_fixture.children.assistant; return UMI_STATUS_OK;
}
UmiStatus fixture_repository_snapshot(const UmiAiCodingRepositoryIndex *index, UmiAiCodingRepositoryIndexSnapshot *out)
{
    (void)index; *out = parent_fixture.children.repository; return UMI_STATUS_OK;
}
size_t fixture_event_count(const UmiAiCodingAgent *agent)
{
    (void)agent; return parent_fixture.children.queued_events;
}
size_t fixture_task_count(const UmiAiCodingTaskQueue *queue)
{
    (void)queue; return parent_fixture.children.queued_tasks;
}
void fixture_cancel(UmiAiCodingAgent *agent)
{
    (void)agent; ++parent_fixture.cancellations;
}
static UmiStatus child_action(void)
{
    ++parent_fixture.calls;
    if (parent_fixture.mutate) ++parent_fixture.children.agent.revision;
    if (parent_fixture.cancel_during && parent_fixture.cancellations == 0) {
        umi_ai_coding_runtime_platform_cancel(parent_fixture.parent);
        if (parent_fixture.parent->revision != parent_fixture.admitted_revision ||
            umi_ai_coding_runtime_platform_apply(parent_fixture.parent) != UMI_STATUS_BUSY)
            parent_fixture.callback_failure = 1;
    }
    return parent_fixture.action_status;
}
UmiStatus fixture_refresh(UmiAiCodingRepositoryIndex *index, const char *root,
    const UmiAiCodingIgnorePolicy *ignore, UmiAiDataClassification classification)
{
    (void)index; (void)root; (void)ignore; (void)classification; return child_action();
}
UmiStatus fixture_set_plan(UmiAiCodingAgent *agent, const UmiAiCodingValidationPlan *plan)
{
    (void)agent; (void)plan; return child_action();
}
UmiStatus fixture_select(const UmiAiCodingWorkspaceAdapter *workspace, const char *root,
    UmiAiCodingValidationSelection *selection, UmiAiCodingValidationPlan *plan)
{
    (void)workspace; (void)root;
    UmiStatus status = child_action();
    if (status == UMI_STATUS_OK) { memset(selection, 0, sizeof(*selection)); memset(plan, 0, sizeof(*plan)); }
    return status;
}
UmiStatus fixture_run(UmiAiCodingAgent *agent, const char *task_id, const UmiAiCodingRequest *request)
{
    (void)agent; (void)task_id; (void)request; return child_action();
}
UmiStatus fixture_enqueue(UmiAiCodingTaskQueue *queue, const UmiAiCodingQueuedTask *task)
{
    (void)queue; (void)task;
    UmiStatus status = child_action();
    if (status == UMI_STATUS_OK || parent_fixture.mutate) ++parent_fixture.children.queued_tasks;
    return status;
}
UmiStatus fixture_pop(UmiAiCodingTaskQueue *queue, UmiAiCodingQueuedTask *task)
{
    (void)queue;
    UmiStatus status = child_action();
    if (status != UMI_STATUS_OK && !parent_fixture.mutate) return status;
    memset(task, 0, sizeof(*task)); strcpy(task->task_id, "queued-task");
    parent_fixture.children.queued_tasks = 0U;
    /* A consumed task is attempted once; the later agent call can still fail. */
    return UMI_STATUS_OK;
}
UmiStatus fixture_approve(UmiAiCodingAgent *agent, const char *reviewer)
{
    (void)agent; (void)reviewer; return child_action();
}
UmiStatus fixture_apply(UmiAiCodingAgent *agent) { (void)agent; return child_action(); }
UmiStatus fixture_repair(UmiAiCodingAgent *agent) { (void)agent; return child_action(); }
UmiStatus fixture_reject(UmiAiCodingAgent *agent) { (void)agent; return child_action(); }

static void fixture_init(UmiAiCodingRuntimePlatform *parent)
{
    memset(parent, 0, sizeof(*parent)); parent->revision = 3U;
    memset(&parent_fixture, 0, sizeof(parent_fixture));
    parent_fixture.parent = parent; parent_fixture.admitted_revision = parent->revision;
    parent_fixture.children.agent.state = UMI_AI_CODING_RUNTIME_IDLE;
    parent_fixture.children.agent.revision = 1U;
    parent_fixture.children.assistant.revision = 1U;
    parent_fixture.children.repository.revision = 1U;
    parent_fixture.children.queued_tasks = 1U;
}

static UmiStatus invoke(UmiAiCodingRuntimePlatform *parent, const char *operation)
{
    UmiAiCodingValidationPlan plan = {0};
    UmiAiCodingValidationSelection selection;
    UmiAiCodingRequest request = {0};
    if (strcmp(operation, "refresh_repository") == 0)
        return umi_ai_coding_runtime_platform_refresh_repository(parent, UMI_AI_DATA_PUBLIC);
    if (strcmp(operation, "set_validation_plan") == 0)
        return umi_ai_coding_runtime_platform_set_validation_plan(parent, &plan);
    if (strcmp(operation, "detect_validation") == 0)
        return umi_ai_coding_runtime_platform_detect_validation(parent, &selection);
    if (strcmp(operation, "run") == 0)
        return umi_ai_coding_runtime_platform_run(parent, "task", &request);
    if (strcmp(operation, "enqueue") == 0)
        return umi_ai_coding_runtime_platform_enqueue(parent, "task", &request);
    if (strcmp(operation, "run_next") == 0)
        return umi_ai_coding_runtime_platform_run_next(parent);
    if (strcmp(operation, "approve") == 0)
        return umi_ai_coding_runtime_platform_approve(parent, "reviewer");
    if (strcmp(operation, "apply") == 0)
        return umi_ai_coding_runtime_platform_apply(parent);
    if (strcmp(operation, "continue_repair") == 0)
        return umi_ai_coding_runtime_platform_continue_repair(parent);
    if (strcmp(operation, "reject") == 0)
        return umi_ai_coding_runtime_platform_reject(parent);
    return UMI_STATUS_INTERNAL_ERROR;
}

static int admission_case(const char *operation, const char *mode)
{
    UmiAiCodingRuntimePlatform *parent = calloc(1U, sizeof(*parent));
    int result = 1;
    CHECK(parent != NULL); fixture_init(parent);
    UmiStatus expected = UMI_STATUS_BUSY;
    if (strcmp(mode, "null") == 0) expected = UMI_STATUS_INVALID_ARGUMENT;
    else if (strcmp(mode, "exhausted") == 0) { parent->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "zero") == 0) { parent->revision = 0U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(mode, "busy") == 0) parent->operation_active = 1;
    else if (strcmp(mode, "child-busy") == 0) parent_fixture.child_busy = 1;
    else if (strcmp(mode, "snapshot-refused") == 0) { parent_fixture.capture_status = UMI_STATUS_IO_ERROR; expected = UMI_STATUS_IO_ERROR; }
    else CHECK(0);
    uint64_t revision = parent->revision; int active = parent->operation_active;
    CHECK(invoke(strcmp(mode, "null") == 0 ? NULL : parent, operation) == expected);
    CHECK(parent_fixture.calls == 0 && parent_fixture.cancellations == 0);
    CHECK(parent->revision == revision && parent->operation_active == active);
    result = 0;
cleanup:
    free(parent); return result;
}

static int result_case(const char *operation, const char *mode)
{
    UmiAiCodingRuntimePlatform *parent = calloc(1U, sizeof(*parent));
    int result = 1, changed = 1;
    CHECK(parent != NULL); fixture_init(parent);
    parent_fixture.mutate = 1;
    if (strcmp(mode, "success") == 0) {}
    else if (strcmp(mode, "last-revision") == 0) parent->revision = UINT64_MAX - 1U;
    else {
        parent_fixture.action_status = UMI_STATUS_IO_ERROR;
        if (strcmp(mode, "changed-failure") == 0) {}
        else {
            parent_fixture.mutate = 0;
            if (strcmp(mode, "unchanged-failure") == 0) changed = 0;
            else if (strcmp(mode, "cancel-failure") == 0) {
                parent_fixture.cancel_during = 1; parent->revision = UINT64_MAX - 1U;
            } else if (strcmp(mode, "readback-failure") == 0) parent_fixture.post_capture_failure = 1;
            else CHECK(0);
        }
    }
    uint64_t revision = parent->revision; parent_fixture.admitted_revision = revision;
    CHECK(invoke(parent, operation) == parent_fixture.action_status);
    CHECK(parent_fixture.calls > 0 && !parent_fixture.callback_failure);
    CHECK(parent->revision == revision + (uint64_t)changed);
    CHECK(parent->operation_active == 0 && parent->cancellation_observed == 0);
    if (parent_fixture.cancel_during) CHECK(parent_fixture.cancellations == 1);
    if (strcmp(operation, "run_next") == 0 && parent_fixture.mutate)
        CHECK(parent_fixture.children.queued_tasks == 0U);
    result = 0;
cleanup:
    free(parent); return result;
}

static int cancel_case(const char *mode)
{
    UmiAiCodingRuntimePlatform *parent = calloc(1U, sizeof(*parent));
    UmiAiCodingRuntimePlatformSnapshot before;
    int result = 1;
    CHECK(parent != NULL); fixture_init(parent);
    if (strcmp(mode, "null") == 0) {
        umi_ai_coding_runtime_platform_cancel(NULL);
        CHECK(parent_fixture.cancellations == 0);
    } else if (strcmp(mode, "active") == 0 || strcmp(mode, "active-last") == 0) {
        if (strcmp(mode, "active-last") == 0) parent->revision = UINT64_MAX - 1U;
        uint64_t revision = parent->revision;
        CHECK(coding_platform_begin_observation(parent, &before) == UMI_STATUS_OK);
        umi_ai_coding_runtime_platform_cancel(parent);
        CHECK(parent->revision == revision && parent->cancellation_observed == 1);
        CHECK(coding_platform_finish_observation(parent, &before, UMI_STATUS_IO_ERROR) == UMI_STATUS_IO_ERROR);
        CHECK(parent->revision == revision + 1U && parent->operation_active == 0);
        CHECK(parent_fixture.cancellations == 1);
    } else {
        if (strcmp(mode, "exhausted") == 0) parent->revision = UINT64_MAX;
        else if (strcmp(mode, "zero") == 0) parent->revision = 0U;
        else CHECK(strcmp(mode, "idle") == 0);
        uint64_t revision = parent->revision;
        umi_ai_coding_runtime_platform_cancel(parent);
        CHECK(parent_fixture.cancellations == 1);
        CHECK(parent->revision == revision + (uint64_t)(revision != 0U && revision != UINT64_MAX));
    }
    result = 0;
cleanup:
    free(parent); return result;
}

static int semantic_case(const char *name)
{
    UmiAiCodingRuntimePlatformSnapshot before = {0}, after = {0};
    int result = 1, equal = 0;
    if (strcmp(name, "padding-and-tail") == 0) {
        memset(&after, 0x5a, sizeof(after));
        after.agent.state = before.agent.state;
        after.agent.last_status = before.agent.last_status;
        after.agent.iteration = before.agent.iteration;
        after.agent.maximum_iterations = before.agent.maximum_iterations;
        after.agent.context_files = before.agent.context_files;
        after.agent.patch_files = before.agent.patch_files;
        after.agent.validation_steps = before.agent.validation_steps;
        after.agent.validation_failures = before.agent.validation_failures;
        after.agent.revision = before.agent.revision;
        after.agent.cancelled = before.agent.cancelled;
        after.agent.patch_recorded = before.agent.patch_recorded;
        after.agent.patch_applied = before.agent.patch_applied;
        after.agent.validation_passed = before.agent.validation_passed;
        after.agent.rolled_back = before.agent.rolled_back;
        after.assistant.revision = before.assistant.revision;
        after.assistant.repository_files = before.assistant.repository_files;
        after.assistant.task_count = before.assistant.task_count;
        after.assistant.patch_count = before.assistant.patch_count;
        after.assistant.draft_patches = before.assistant.draft_patches;
        after.assistant.approved_patches = before.assistant.approved_patches;
        after.assistant.applied_patches = before.assistant.applied_patches;
        after.assistant.reverted_patches = before.assistant.reverted_patches;
        after.assistant.patch_policy.maximum_files = before.assistant.patch_policy.maximum_files;
        after.assistant.patch_policy.maximum_changed_lines = before.assistant.patch_policy.maximum_changed_lines;
        after.assistant.patch_policy.allow_create = before.assistant.patch_policy.allow_create;
        after.assistant.patch_policy.allow_delete = before.assistant.patch_policy.allow_delete;
        after.assistant.patch_policy.require_approval = before.assistant.patch_policy.require_approval;
        after.repository.scanned_files = before.repository.scanned_files;
        after.repository.indexed_files = before.repository.indexed_files;
        after.repository.ignored_non_text_files = before.repository.ignored_non_text_files;
        after.repository.capacity_dropped_files = before.repository.capacity_dropped_files;
        after.repository.revision = before.repository.revision;
        after.repository.truncated = before.repository.truncated;
        after.queued_events = before.queued_events;
        after.queued_tasks = before.queued_tasks;
        after.agent.task_id[0] = '\0';
        after.agent.request_id[0] = '\0';
        after.agent.provider_id[0] = '\0';
        after.agent.model_id[0] = '\0';
        after.agent.patch_id[0] = '\0';
        after.assistant.last_request_id[0] = '\0';
        after.assistant.last_patch_id[0] = '\0';
        equal = 1;
    } else if (strcmp(name, "parent-revision") == 0) { after.revision = 99U; equal = 1; }
    else if (strcmp(name, "agent.state") == 0) ++after.agent.state;
    else if (strcmp(name, "agent.last_status") == 0) ++after.agent.last_status;
    else if (strcmp(name, "agent.iteration") == 0) ++after.agent.iteration;
    else if (strcmp(name, "agent.maximum_iterations") == 0) ++after.agent.maximum_iterations;
    else if (strcmp(name, "agent.context_files") == 0) ++after.agent.context_files;
    else if (strcmp(name, "agent.patch_files") == 0) ++after.agent.patch_files;
    else if (strcmp(name, "agent.validation_steps") == 0) ++after.agent.validation_steps;
    else if (strcmp(name, "agent.validation_failures") == 0) ++after.agent.validation_failures;
    else if (strcmp(name, "agent.revision") == 0) ++after.agent.revision;
    else if (strcmp(name, "agent.cancelled") == 0) ++after.agent.cancelled;
    else if (strcmp(name, "agent.patch_recorded") == 0) ++after.agent.patch_recorded;
    else if (strcmp(name, "agent.patch_applied") == 0) ++after.agent.patch_applied;
    else if (strcmp(name, "agent.validation_passed") == 0) ++after.agent.validation_passed;
    else if (strcmp(name, "agent.rolled_back") == 0) ++after.agent.rolled_back;
    else if (strcmp(name, "assistant.revision") == 0) ++after.assistant.revision;
    else if (strcmp(name, "assistant.repository_files") == 0) ++after.assistant.repository_files;
    else if (strcmp(name, "assistant.task_count") == 0) ++after.assistant.task_count;
    else if (strcmp(name, "assistant.patch_count") == 0) ++after.assistant.patch_count;
    else if (strcmp(name, "assistant.draft_patches") == 0) ++after.assistant.draft_patches;
    else if (strcmp(name, "assistant.approved_patches") == 0) ++after.assistant.approved_patches;
    else if (strcmp(name, "assistant.applied_patches") == 0) ++after.assistant.applied_patches;
    else if (strcmp(name, "assistant.reverted_patches") == 0) ++after.assistant.reverted_patches;
    else if (strcmp(name, "assistant.patch_policy.maximum_files") == 0) ++after.assistant.patch_policy.maximum_files;
    else if (strcmp(name, "assistant.patch_policy.maximum_changed_lines") == 0) ++after.assistant.patch_policy.maximum_changed_lines;
    else if (strcmp(name, "assistant.patch_policy.allow_create") == 0) ++after.assistant.patch_policy.allow_create;
    else if (strcmp(name, "assistant.patch_policy.allow_delete") == 0) ++after.assistant.patch_policy.allow_delete;
    else if (strcmp(name, "assistant.patch_policy.require_approval") == 0) ++after.assistant.patch_policy.require_approval;
    else if (strcmp(name, "repository.scanned_files") == 0) ++after.repository.scanned_files;
    else if (strcmp(name, "repository.indexed_files") == 0) ++after.repository.indexed_files;
    else if (strcmp(name, "repository.ignored_non_text_files") == 0) ++after.repository.ignored_non_text_files;
    else if (strcmp(name, "repository.capacity_dropped_files") == 0) ++after.repository.capacity_dropped_files;
    else if (strcmp(name, "repository.revision") == 0) ++after.repository.revision;
    else if (strcmp(name, "repository.truncated") == 0) ++after.repository.truncated;
    else if (strcmp(name, "queued_events") == 0) ++after.queued_events;
    else if (strcmp(name, "queued_tasks") == 0) ++after.queued_tasks;
    else if (strcmp(name, "agent.task_id") == 0) strcpy(after.agent.task_id, "different");
    else if (strcmp(name, "agent.request_id") == 0) strcpy(after.agent.request_id, "different");
    else if (strcmp(name, "agent.provider_id") == 0) strcpy(after.agent.provider_id, "different");
    else if (strcmp(name, "agent.model_id") == 0) strcpy(after.agent.model_id, "different");
    else if (strcmp(name, "agent.patch_id") == 0) strcpy(after.agent.patch_id, "different");
    else if (strcmp(name, "assistant.last_request_id") == 0) strcpy(after.assistant.last_request_id, "different");
    else if (strcmp(name, "assistant.last_patch_id") == 0) strcpy(after.assistant.last_patch_id, "different");
    else CHECK(0);
    CHECK(coding_platform_observations_equal(&before, &after) == equal);
    result = 0;
cleanup:
    return result;
}


int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "admission") == 0) return admission_case(argv[2], argv[3]);
    if (argc == 4 && strcmp(argv[1], "result") == 0) return result_case(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "cancel") == 0) return cancel_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "semantic") == 0) return semantic_case(argv[2]);
    return 2;
}
