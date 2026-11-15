/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_agent_revisions.c
 *
 * PURPOSE:
 *   Verify owner-lifetime task revisions, admission preservation and terminal state capacity without provider or file execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



/* Keep real agent, queue, history and audit behavior. Replace only the planning
 * boundary so every run stops before provider, workspace or executor work. */
#define umi_ai_coding_assistant_prepare fixture_prepare
#include "../../src/ai_coding_runtime/agent.c"
#undef umi_ai_coding_assistant_prepare

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

static struct {
    UmiAiCodingAgent *agent;
    int calls, cancel, callback_failed;
    uint64_t observed_revision;
} revision_fixture;

UmiStatus fixture_prepare(UmiAiCodingAssistantService *assistant,
    const UmiAiCodingRequest *request, UmiAiProviderKind kind, UmiAiCodingTaskPlan *plan)
{
    (void)assistant; (void)kind; (void)plan;
    ++revision_fixture.calls;
    UmiAiCodingAgentSnapshot observed = {0};
    if (umi_ai_coding_agent_snapshot(revision_fixture.agent, &observed) != UMI_STATUS_OK ||
        observed.state != UMI_AI_CODING_RUNTIME_PREPARING ||
        umi_ai_coding_agent_run(revision_fixture.agent, "nested", request) != UMI_STATUS_BUSY)
        revision_fixture.callback_failed = 1;
    revision_fixture.observed_revision = observed.revision;
    if (revision_fixture.cancel) {
        umi_ai_coding_agent_cancel(revision_fixture.agent);
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_IO_ERROR;
}

static void fixture_destroy(UmiAiCodingAgent *agent)
{
    if (agent == NULL) return;
    umi_ai_coding_event_queue_destroy(agent->events);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_audit_log_destroy(agent->audit);
    free(agent);
}
static UmiAiCodingAgent *fixture_create(void)
{
    UmiAiCodingAgent *agent = calloc(1U, sizeof(*agent));
    if (agent == NULL) return NULL;
    if (umi_ai_coding_event_queue_create(&agent->events) != UMI_STATUS_OK ||
        umi_ai_coding_history_create(&agent->history) != UMI_STATUS_OK ||
        umi_ai_coding_audit_log_create(&agent->audit) != UMI_STATUS_OK) {
        fixture_destroy(agent); return NULL;
    }
    umi_ai_coding_runtime_config_init(&agent->config);
    strcpy(agent->config.provider_id, "memory-provider");
    strcpy(agent->config.model_id, "memory-model");
    umi_ai_coding_cancellation_init(&agent->cancellation);
    umi_ai_coding_cancellation_request(&agent->cancellation);
    strcpy(agent->snapshot.task_id, "prior-task");
    agent->snapshot.state = UMI_AI_CODING_RUNTIME_IDLE;
    agent->snapshot.revision = 9U;
    agent->has_patch_outcome = 1;
    strcpy(agent->patch_outcome.task_id, "prior-task");
    strcpy(agent->patch_outcome.patch_id, "prior-patch");
    strcpy(agent->validation_feedback, "prior-feedback");
    memset(&revision_fixture, 0, sizeof(revision_fixture));
    revision_fixture.agent = agent;
    return agent;
}
static UmiStatus fill_queue(UmiAiCodingAgent *agent)
{
    UmiAiCodingEvent event = {0};
    for (size_t i = 0U; i < UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; ++i) {
        UmiStatus status = umi_ai_coding_event_queue_push(agent->events, &event);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
static int is_terminal(UmiAiCodingRuntimeState state)
{
    return state == UMI_AI_CODING_RUNTIME_IDLE || state == UMI_AI_CODING_RUNTIME_COMPLETED ||
        state == UMI_AI_CODING_RUNTIME_FAILED || state == UMI_AI_CODING_RUNTIME_CANCELLED;
}
static int state_value(const char *name, UmiAiCodingRuntimeState *out)
{
    static const char *const names[] = {"idle", "preparing", "context", "generating",
        "review", "applying", "validating", "repairing", "completed", "cancelled", "failed"};
    static const UmiAiCodingRuntimeState states[] = {
        UMI_AI_CODING_RUNTIME_IDLE, UMI_AI_CODING_RUNTIME_PREPARING,
        UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT, UMI_AI_CODING_RUNTIME_GENERATING,
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED, UMI_AI_CODING_RUNTIME_APPLYING,
        UMI_AI_CODING_RUNTIME_VALIDATING, UMI_AI_CODING_RUNTIME_REPAIRING,
        UMI_AI_CODING_RUNTIME_COMPLETED, UMI_AI_CODING_RUNTIME_CANCELLED,
        UMI_AI_CODING_RUNTIME_FAILED};
    for (size_t i = 0U; i < sizeof(states) / sizeof(states[0]); ++i)
        if (strcmp(name, names[i]) == 0) { *out = states[i]; return 1; }
    return 0;
}

static int transition_case(const char *name, const char *mode)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingRuntimeState target;
    UmiAiCodingAgentSnapshot before, retained;
    UmiAiCodingEvent event;
    int result = 1, full = 0;
    CHECK(agent != NULL && state_value(name, &target));
    if (strcmp(mode, "last-headroom") == 0) agent->snapshot.revision = UINT64_MAX - 2U;
    else if (strcmp(mode, "final-slot") == 0) agent->snapshot.revision = UINT64_MAX - 1U;
    else if (strcmp(mode, "exhausted") == 0) agent->snapshot.revision = UINT64_MAX;
    else if (strcmp(mode, "zero") == 0) agent->snapshot.revision = 0U;
    else if (strcmp(mode, "full-final-slot") == 0) {
        agent->snapshot.revision = UINT64_MAX - 1U; full = 1; CHECK(fill_queue(agent) == UMI_STATUS_OK);
    } else CHECK(strcmp(mode, "ordinary") == 0);
    before = agent->snapshot;
    int redirected = before.revision == UINT64_MAX - 1U && !is_terminal(target);
    UmiStatus expected = before.revision == 0U ? UMI_STATUS_INVALID_STATE :
        before.revision == UINT64_MAX || redirected || full ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    CHECK(set_state(agent, target, UMI_STATUS_UNAVAILABLE, "requested transition") == expected);
    if (before.revision == 0U || before.revision == UINT64_MAX) {
        CHECK(memcmp(&before, &agent->snapshot, sizeof(before)) == 0);
        CHECK(umi_ai_coding_agent_event_count(agent) == 0U);
        CHECK(umi_ai_coding_history_count(agent->history) == 0U);
        CHECK(umi_ai_coding_audit_log_count(agent->audit) == 0U);
        CHECK(agent->event_sequence == 0U);
    } else {
        CHECK(agent->snapshot.revision == before.revision + 1U);
        CHECK(agent->snapshot.state == (redirected ? UMI_AI_CODING_RUNTIME_FAILED : target));
        CHECK(agent->snapshot.last_status == (redirected ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_UNAVAILABLE));
        if (full) {
            CHECK(umi_ai_coding_agent_event_count(agent) == UMI_AI_CODING_RUNTIME_EVENT_CAPACITY);
            CHECK(umi_ai_coding_history_count(agent->history) == 0U);
            CHECK(umi_ai_coding_audit_log_count(agent->audit) == 0U);
            CHECK(agent->event_sequence == 0U);
        } else {
            CHECK(umi_ai_coding_history_at(agent->history, 0U, &retained) == UMI_STATUS_OK);
            CHECK(retained.state == agent->snapshot.state && retained.revision == agent->snapshot.revision);
            CHECK(umi_ai_coding_agent_next_event(agent, &event) == UMI_STATUS_OK);
            CHECK(event.state == agent->snapshot.state && event.status == agent->snapshot.last_status);
            CHECK(agent->event_sequence == 1U && umi_ai_coding_audit_log_count(agent->audit) == 1U);
        }
    }
    result = 0;
cleanup:
    fixture_destroy(agent); return result;
}

static int run_case(const char *origin, const char *mode)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingAgent *before = malloc(sizeof(*before));
    UmiAiCodingRequest request = {0};
    int result = 1, full = 0;
    CHECK(agent != NULL && before != NULL && state_value(origin, &agent->snapshot.state));
    CHECK(is_terminal(agent->snapshot.state));
    strcpy(request.request_id, "next-request");
    if (strcmp(mode, "last-admissible") == 0) agent->snapshot.revision = UINT64_MAX - 2U;
    else if (strcmp(mode, "one-left") == 0) agent->snapshot.revision = UINT64_MAX - 1U;
    else if (strcmp(mode, "exhausted") == 0) agent->snapshot.revision = UINT64_MAX;
    else if (strcmp(mode, "zero") == 0) agent->snapshot.revision = 0U;
    else if (strcmp(mode, "full-queue") == 0) { full = 1; CHECK(fill_queue(agent) == UMI_STATUS_OK); }
    else if (strcmp(mode, "cancelled") == 0) revision_fixture.cancel = 1;
    else CHECK(strcmp(mode, "ordinary") == 0);
    /* Byte comparison is limited to this initialized, serialized fixture owner;
     * it is not an application equality policy for snapshots or atomic values. */
    memcpy(before, agent, sizeof(*before));
    int refused = before->snapshot.revision == 0U || before->snapshot.revision >= UINT64_MAX - 1U;
    UmiStatus expected = before->snapshot.revision == 0U ? UMI_STATUS_INVALID_STATE :
        refused || full ? UMI_STATUS_CAPACITY_EXCEEDED :
        revision_fixture.cancel ? UMI_STATUS_CANCELLED : UMI_STATUS_IO_ERROR;
    CHECK(umi_ai_coding_agent_run(agent, "next-task", &request) == expected);
    CHECK(!revision_fixture.callback_failed && !agent->operation_active);
    if (refused) {
        CHECK(memcmp(before, agent, sizeof(*before)) == 0);
        CHECK(revision_fixture.calls == 0);
        CHECK(umi_ai_coding_agent_event_count(agent) == 0U);
        CHECK(umi_ai_coding_history_count(agent->history) == 0U);
        CHECK(umi_ai_coding_audit_log_count(agent->audit) == 0U);
    } else {
        CHECK(agent->snapshot.revision == before->snapshot.revision + 2U);
        CHECK(agent->snapshot.state == (revision_fixture.cancel ? UMI_AI_CODING_RUNTIME_CANCELLED : UMI_AI_CODING_RUNTIME_FAILED));
        CHECK(agent->snapshot.last_status == expected);
        CHECK(strcmp(agent->snapshot.task_id, "next-task") == 0);
        CHECK(!agent->has_patch_outcome);
        CHECK(revision_fixture.calls == (full ? 0 : 1));
        if (!full) CHECK(revision_fixture.observed_revision == before->snapshot.revision + 1U);
        CHECK(umi_ai_coding_cancellation_is_requested(&agent->cancellation) == revision_fixture.cancel);
        CHECK(umi_ai_coding_history_count(agent->history) == (full ? 0U : 2U));
        CHECK(umi_ai_coding_audit_log_count(agent->audit) == (full ? 0U : 2U));
    }
    result = 0;
cleanup:
    free(before); fixture_destroy(agent); return result;
}

static int lifecycle_case(const char *mode)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingRequest request = {0};
    UmiAiCodingAgentSnapshot before;
    int result = 1;
    CHECK(agent != NULL);
    strcpy(request.request_id, "request");
    if (strcmp(mode, "invalid-input") == 0 || strcmp(mode, "busy-run") == 0) {
        int busy = strcmp(mode, "busy-run") == 0;
        agent->operation_active = busy; before = agent->snapshot;
        CHECK(umi_ai_coding_agent_run(agent, busy ? "task" : "", &request) ==
            (busy ? UMI_STATUS_BUSY : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(memcmp(&before, &agent->snapshot, sizeof(before)) == 0);
        CHECK(agent->has_patch_outcome && revision_fixture.calls == 0);
        CHECK(umi_ai_coding_cancellation_is_requested(&agent->cancellation));
    } else if (strcmp(mode, "successive-tasks") == 0) {
        CHECK(umi_ai_coding_agent_run(agent, "first-task", &request) == UMI_STATUS_IO_ERROR);
        uint64_t revision = agent->snapshot.revision, sequence = agent->event_sequence;
        CHECK(umi_ai_coding_agent_run(agent, "second-task", &request) == UMI_STATUS_IO_ERROR);
        CHECK(agent->snapshot.revision == revision + 2U && agent->event_sequence == sequence + 2U);
        CHECK(strcmp(agent->snapshot.task_id, "second-task") == 0 && revision_fixture.calls == 2);
        CHECK(umi_ai_coding_history_count(agent->history) == 4U);
        for (size_t i = 0U; i < 4U; ++i) {
            UmiAiCodingAgentSnapshot retained;
            CHECK(umi_ai_coding_history_at(agent->history, i, &retained) == UMI_STATUS_OK);
            CHECK(retained.revision == 10U + i);
        }
    } else if (strcmp(mode, "apply-final-slot") == 0 || strcmp(mode, "repair-final-slot") == 0 ||
        strcmp(mode, "cancel-final-slot") == 0) {
        int repair = strcmp(mode, "repair-final-slot") == 0;
        int cancel = strcmp(mode, "cancel-final-slot") == 0;
        if (!cancel) umi_ai_coding_cancellation_init(&agent->cancellation);
        agent->snapshot.revision = UINT64_MAX - 1U;
        agent->snapshot.state = repair ? UMI_AI_CODING_RUNTIME_REPAIRING : UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED;
        agent->snapshot.patch_recorded = 1;
        strcpy(agent->snapshot.patch_id, "pending-patch");
        UmiStatus status = repair ? umi_ai_coding_agent_continue_repair(agent) : umi_ai_coding_agent_apply_pending(agent);
        CHECK(status == (cancel ? UMI_STATUS_CANCELLED : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(agent->snapshot.state == (cancel ? UMI_AI_CODING_RUNTIME_CANCELLED : UMI_AI_CODING_RUNTIME_FAILED));
        CHECK(agent->snapshot.revision == UINT64_MAX && !agent->operation_active);
        CHECK(revision_fixture.calls == 0 && agent->has_patch_outcome);
        CHECK(umi_ai_coding_history_count(agent->history) == 1U);
    } else {
        CHECK(strcmp(mode, "rejected-followup") == 0);
        agent->snapshot.revision = UINT64_MAX - 2U;
        CHECK(umi_ai_coding_agent_run(agent, "last-task", &request) == UMI_STATUS_IO_ERROR);
        before = agent->snapshot;
        CHECK(umi_ai_coding_agent_run(agent, "refused-task", &request) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&before, &agent->snapshot, sizeof(before)) == 0);
        CHECK(agent->snapshot.revision == UINT64_MAX && revision_fixture.calls == 1);
        CHECK(umi_ai_coding_history_count(agent->history) == 2U);
    }
    result = 0;
cleanup:
    fixture_destroy(agent); return result;
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "transition") == 0) return transition_case(argv[2], argv[3]);
    if (argc == 4 && strcmp(argv[1], "run") == 0) return run_case(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "lifecycle") == 0) return lifecycle_case(argv[2]);
    return 2;
}
