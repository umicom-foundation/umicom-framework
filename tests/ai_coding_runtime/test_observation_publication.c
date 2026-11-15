/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_observation_publication.c
 *
 * PURPOSE:
 *   Verify event, audit and history publication remains unchanged on admission refusal and identity exhaustion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Compile the actual owners together only to inject otherwise unreachable
 * counter and storage boundaries. All mutation paths are the product functions;
 * this fixture uses no provider, process, network or filesystem adapter. */
#include "../../src/ai_coding_runtime/event_queue.c"
#include "../../src/ai_coding_runtime/history.c"
#include "../../src/ai_coding_runtime/audit.c"
#include "../../src/ai_coding_runtime/observation.c"
#include "../../src/ai_coding_runtime/agent.c"

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct StorageBackup {
    UmiAiCodingEventQueue events;
    UmiAiCodingHistory history;
    UmiAiCodingAuditLog audit;
    UmiAiCodingAgentSnapshot snapshot;
    uint64_t sequence;
} StorageBackup;

static UmiAiCodingAgent *fixture_create(void)
{
    UmiAiCodingAgent *agent = calloc(1U, sizeof(*agent));
    if (agent == NULL) return NULL;
    if (umi_ai_coding_event_queue_create(&agent->events) != UMI_STATUS_OK ||
        umi_ai_coding_history_create(&agent->history) != UMI_STATUS_OK ||
        umi_ai_coding_audit_log_create(&agent->audit) != UMI_STATUS_OK) {
        umi_ai_coding_event_queue_destroy(agent->events);
        umi_ai_coding_history_destroy(agent->history);
        umi_ai_coding_audit_log_destroy(agent->audit);
        free(agent); return NULL;
    }
    strcpy(agent->snapshot.task_id, "observation.example");
    agent->snapshot.state = UMI_AI_CODING_RUNTIME_IDLE;
    agent->snapshot.revision = 7U;
    agent->event_sequence = 11U;
    return agent;
}

static void fixture_destroy(UmiAiCodingAgent *agent)
{
    if (agent == NULL) return;
    umi_ai_coding_event_queue_destroy(agent->events);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_audit_log_destroy(agent->audit);
    free(agent);
}

static void capture(StorageBackup *before, const UmiAiCodingAgent *agent)
{
    memcpy(&before->events, agent->events, sizeof(before->events));
    memcpy(&before->history, agent->history, sizeof(before->history));
    memcpy(&before->audit, agent->audit, sizeof(before->audit));
    memcpy(&before->snapshot, &agent->snapshot, sizeof(before->snapshot));
    before->sequence = agent->event_sequence;
}

static int stores_unchanged(const StorageBackup *before, const UmiAiCodingAgent *agent)
{
    return memcmp(&before->events, agent->events, sizeof(before->events)) == 0 &&
        memcmp(&before->history, agent->history, sizeof(before->history)) == 0 &&
        memcmp(&before->audit, agent->audit, sizeof(before->audit)) == 0;
}

static void seed_audit_identity(UmiAiCodingAuditLog *audit, uint64_t sequence)
{
    audit->count = 1U; audit->sequence = sequence;
    audit->items[0].sequence = sequence;
}

static int publication_case(const char *name)
{
    UmiAiCodingAgent *agent = fixture_create();
    StorageBackup *before = malloc(sizeof(*before));
    UmiAiCodingEvent event = {0};
    UmiAiCodingAuditRecord record = {0};
    UmiAiCodingAgentSnapshot snapshot = {0};
    UmiStatus expected = UMI_STATUS_OK;
    int result = 1, with_history = strcmp(name, "event-only") != 0;
    CHECK(agent != NULL && before != NULL);
    event.sequence = 41U; strcpy(event.task_id, "task"); strcpy(event.message, "observed");
    record.sequence = 99U; strcpy(record.task_id, "task"); strcpy(record.summary, "observed");
    snapshot.revision = 17U; strcpy(snapshot.task_id, "task");
    if (strcmp(name, "full-event") == 0) { agent->events->count = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "bad-event-text") == 0) { memset(event.message, 'x', sizeof(event.message)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "bad-audit-text") == 0) { memset(record.summary, 'x', sizeof(record.summary)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "bad-snapshot-text") == 0) { memset(snapshot.patch_id, 'x', sizeof(snapshot.patch_id)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "event-head") == 0) { agent->events->head = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "event-count") == 0) { agent->events->count = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY + 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "history-count") == 0) { agent->history->count = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY + 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "audit-count") == 0) { agent->audit->count = UMI_AI_CODING_AUDIT_CAPACITY + 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "audit-sequence") == 0) { agent->audit->sequence = 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "audit-exhausted") == 0) { seed_audit_identity(agent->audit, UINT64_MAX); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "audit-last") == 0) seed_audit_identity(agent->audit, UINT64_MAX - 1U);
    else if (strcmp(name, "retention") == 0) {
        agent->history->count = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY;
        agent->audit->count = UMI_AI_CODING_AUDIT_CAPACITY;
        agent->audit->sequence = UMI_AI_CODING_AUDIT_CAPACITY;
        for (size_t i = 0U; i < agent->audit->count; ++i) agent->audit->items[i].sequence = i + 1U;
    } else if (strncmp(name, "null-", 5U) == 0) expected = UMI_STATUS_INVALID_ARGUMENT;
    else CHECK(strcmp(name, "success") == 0 || strcmp(name, "event-only") == 0);
    capture(before, agent);
    UmiStatus status = UmiAiCodingObservationPublish(
        strcmp(name, "null-events") == 0 ? NULL : agent->events,
        !with_history || strcmp(name, "null-history") == 0 ? NULL : agent->history,
        strcmp(name, "null-audit") == 0 ? NULL : agent->audit,
        strcmp(name, "null-event") == 0 ? NULL : &event,
        with_history ? &snapshot : NULL,
        strcmp(name, "null-record") == 0 ? NULL : &record);
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK) CHECK(stores_unchanged(before, agent));
    else {
        CHECK(agent->events->count == before->events.count + 1U);
        size_t tail = (agent->events->head + agent->events->count - 1U) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        CHECK(agent->events->items[tail].sequence == 41U);
        CHECK(agent->audit->sequence == before->audit.sequence + 1U);
        CHECK(strcmp(agent->audit->items[agent->audit->count - 1U].summary, "observed") == 0);
        if (with_history) CHECK(agent->history->items[agent->history->count - 1U].revision == 17U);
        else CHECK(memcmp(&before->history, agent->history, sizeof(before->history)) == 0);
    }
    result = 0;
cleanup:
    free(before); fixture_destroy(agent); return result;
}

/* The final revision now belongs to a terminal state; keep this publication fixture focused on accepted final publication while the dedicated lifecycle fixture checks nonterminal exhaustion. The former assertions remain for review. The former implementation is retained for owner review. */
#if 0
static int agent_case(const char *name, int transition)
{
    UmiAiCodingAgent *agent = fixture_create();
    StorageBackup *before = malloc(sizeof(*before));
    UmiStatus expected = UMI_STATUS_OK;
    char long_message[2048];
    const char *message = "observation";
    int result = 1;
    CHECK(agent != NULL && before != NULL);
    memset(long_message, 'x', sizeof(long_message) - 1U); long_message[sizeof(long_message) - 1U] = '\0';
    if (strcmp(name, "full-event") == 0) { agent->events->count = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "corrupt-event") == 0) { agent->events->head = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "corrupt-audit") == 0) { agent->audit->sequence = 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "exhausted-audit") == 0) { seed_audit_identity(agent->audit, UINT64_MAX); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "invalid-task-text") == 0) { memset(agent->snapshot.task_id, 'x', sizeof(agent->snapshot.task_id)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "invalid-patch-text") == 0) { memset(agent->snapshot.patch_id, 'x', sizeof(agent->snapshot.patch_id)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "sequence-exhausted") == 0) { agent->event_sequence = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "last-sequence") == 0) agent->event_sequence = UINT64_MAX - 1U;
    else if (strcmp(name, "revision-exhausted") == 0) { agent->snapshot.revision = UINT64_MAX; if (transition) expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "last-revision") == 0) agent->snapshot.revision = UINT64_MAX - 1U;
    else if (strcmp(name, "history-corrupt") == 0) { agent->history->count = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY + 1U; if (transition) expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "long-message") == 0) message = long_message;
    else if (strcmp(name, "null-message") == 0) message = NULL;
    else if (strcmp(name, "audit-last") == 0) seed_audit_identity(agent->audit, UINT64_MAX - 1U);
    else if (strcmp(name, "retained-history") == 0) agent->history->count = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY;
    else CHECK(strcmp(name, "success") == 0);
    capture(before, agent);
    UmiStatus status = transition ?
        set_state(agent, UMI_AI_CODING_RUNTIME_GENERATING, UMI_STATUS_UNAVAILABLE, message) :
        emit(agent, UMI_AI_CODING_EVENT_CONTEXT, UMI_STATUS_UNAVAILABLE, message);
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(stores_unchanged(before, agent));
/* Retained records and event identity remain unchanged on refusal, while valid execution-state transitions remain observable independently of queue backpressure. The former implementation is retained for owner review. */
#if 0
        CHECK(memcmp(&before->snapshot, &agent->snapshot, sizeof(before->snapshot)) == 0);
        CHECK(agent->event_sequence == before->sequence);
#endif
        if (transition && before->snapshot.revision != UINT64_MAX &&
            strcmp(name, "invalid-task-text") != 0 && strcmp(name, "invalid-patch-text") != 0) {
            CHECK(agent->snapshot.state == UMI_AI_CODING_RUNTIME_GENERATING);
            CHECK(agent->snapshot.last_status == UMI_STATUS_UNAVAILABLE);
            CHECK(agent->snapshot.revision == before->snapshot.revision + 1U);
        } else CHECK(memcmp(&before->snapshot, &agent->snapshot, sizeof(before->snapshot)) == 0);
        CHECK(agent->event_sequence == before->sequence);
    } else {
        CHECK(agent->event_sequence == before->sequence + 1U);
        CHECK(agent->events->count == before->events.count + 1U);
        CHECK(agent->audit->sequence == before->audit.sequence + 1U);
        if (transition) {
            CHECK(agent->snapshot.revision == before->snapshot.revision + 1U);
            CHECK(agent->snapshot.state == UMI_AI_CODING_RUNTIME_GENERATING);
            CHECK(agent->snapshot.last_status == UMI_STATUS_UNAVAILABLE);
            CHECK(agent->history->items[agent->history->count - 1U].revision == agent->snapshot.revision);
        } else {
            CHECK(memcmp(&before->snapshot, &agent->snapshot, sizeof(before->snapshot)) == 0);
            CHECK(memcmp(&before->history, agent->history, sizeof(before->history)) == 0);
        }
        size_t tail = (agent->events->head + agent->events->count - 1U) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        CHECK(agent->events->items[tail].sequence == agent->event_sequence);
        CHECK(agent->events->items[tail].state == agent->snapshot.state);
        if (message == NULL) CHECK(agent->events->items[tail].message[0] == '\0');
        if (strcmp(name, "long-message") == 0) CHECK(strlen(agent->events->items[tail].message) == sizeof(agent->events->items[tail].message) - 1U);
    }
    result = 0;
cleanup:
    free(before); fixture_destroy(agent); return result;
}

#endif
static int agent_case(const char *name, int transition)
{
    UmiAiCodingAgent *agent = fixture_create();
    StorageBackup *before = malloc(sizeof(*before));
    UmiStatus expected = UMI_STATUS_OK;
    UmiAiCodingRuntimeState requested_state = UMI_AI_CODING_RUNTIME_GENERATING;
    char long_message[2048];
    const char *message = "observation";
    int result = 1;
    CHECK(agent != NULL && before != NULL);
    memset(long_message, 'x', sizeof(long_message) - 1U); long_message[sizeof(long_message) - 1U] = '\0';
    if (strcmp(name, "full-event") == 0) { agent->events->count = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "corrupt-event") == 0) { agent->events->head = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "corrupt-audit") == 0) { agent->audit->sequence = 1U; expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "exhausted-audit") == 0) { seed_audit_identity(agent->audit, UINT64_MAX); expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "invalid-task-text") == 0) { memset(agent->snapshot.task_id, 'x', sizeof(agent->snapshot.task_id)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "invalid-patch-text") == 0) { memset(agent->snapshot.patch_id, 'x', sizeof(agent->snapshot.patch_id)); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "sequence-exhausted") == 0) { agent->event_sequence = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "last-sequence") == 0) agent->event_sequence = UINT64_MAX - 1U;
    else if (strcmp(name, "revision-exhausted") == 0) { agent->snapshot.revision = UINT64_MAX; if (transition) expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "last-revision") == 0) {
        agent->snapshot.revision = UINT64_MAX - 1U;
        requested_state = UMI_AI_CODING_RUNTIME_COMPLETED;
    }
    else if (strcmp(name, "history-corrupt") == 0) { agent->history->count = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY + 1U; if (transition) expected = UMI_STATUS_INVALID_STATE; }
    else if (strcmp(name, "long-message") == 0) message = long_message;
    else if (strcmp(name, "null-message") == 0) message = NULL;
    else if (strcmp(name, "audit-last") == 0) seed_audit_identity(agent->audit, UINT64_MAX - 1U);
    else if (strcmp(name, "retained-history") == 0) agent->history->count = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY;
    else CHECK(strcmp(name, "success") == 0);
    capture(before, agent);
    UmiStatus status = transition ?
        set_state(agent, requested_state, UMI_STATUS_UNAVAILABLE, message) :
        emit(agent, UMI_AI_CODING_EVENT_CONTEXT, UMI_STATUS_UNAVAILABLE, message);
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(stores_unchanged(before, agent));
/* Retained records and event identity remain unchanged on refusal, while valid execution-state transitions remain observable independently of queue backpressure. The former implementation is retained for owner review. */
#if 0
        CHECK(memcmp(&before->snapshot, &agent->snapshot, sizeof(before->snapshot)) == 0);
        CHECK(agent->event_sequence == before->sequence);
#endif
        if (transition && before->snapshot.revision != UINT64_MAX &&
            strcmp(name, "invalid-task-text") != 0 && strcmp(name, "invalid-patch-text") != 0) {
            CHECK(agent->snapshot.state == requested_state);
            CHECK(agent->snapshot.last_status == UMI_STATUS_UNAVAILABLE);
            CHECK(agent->snapshot.revision == before->snapshot.revision + 1U);
        } else CHECK(memcmp(&before->snapshot, &agent->snapshot, sizeof(before->snapshot)) == 0);
        CHECK(agent->event_sequence == before->sequence);
    } else {
        CHECK(agent->event_sequence == before->sequence + 1U);
        CHECK(agent->events->count == before->events.count + 1U);
        CHECK(agent->audit->sequence == before->audit.sequence + 1U);
        if (transition) {
            CHECK(agent->snapshot.revision == before->snapshot.revision + 1U);
            CHECK(agent->snapshot.state == requested_state);
            CHECK(agent->snapshot.last_status == UMI_STATUS_UNAVAILABLE);
            CHECK(agent->history->items[agent->history->count - 1U].revision == agent->snapshot.revision);
        } else {
            CHECK(memcmp(&before->snapshot, &agent->snapshot, sizeof(before->snapshot)) == 0);
            CHECK(memcmp(&before->history, agent->history, sizeof(before->history)) == 0);
        }
        size_t tail = (agent->events->head + agent->events->count - 1U) % UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
        CHECK(agent->events->items[tail].sequence == agent->event_sequence);
        CHECK(agent->events->items[tail].state == agent->snapshot.state);
        if (message == NULL) CHECK(agent->events->items[tail].message[0] == '\0');
        if (strcmp(name, "long-message") == 0) CHECK(strlen(agent->events->items[tail].message) == sizeof(agent->events->items[tail].message) - 1U);
    }
    result = 0;
cleanup:
    free(before); fixture_destroy(agent); return result;
}


static int run_full_queue_case(void)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingRequest request = {0};
    StorageBackup *before = malloc(sizeof(*before));
    int result = 1;
    CHECK(agent != NULL && before != NULL);
    agent->events->count = UMI_AI_CODING_RUNTIME_EVENT_CAPACITY;
    capture(before, agent);
    /* The first state notification fails before planning or provider execution.
     * The run must still end in FAILED so later owner recovery can proceed. */
    CHECK(coding_agent_run_internal(agent, "full-queue-task", &request) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(agent->snapshot.state == UMI_AI_CODING_RUNTIME_FAILED);
    CHECK(agent->snapshot.last_status == UMI_STATUS_CAPACITY_EXCEEDED);
/* Task transitions now preserve the owner-lifetime revision instead of resetting it. Compare the two transitions with the captured starting identity; retain the earlier per-task expectation for review. The former implementation is retained for owner review. */
#if 0
    CHECK(agent->snapshot.revision == 3U);
#endif
    CHECK(agent->snapshot.revision == before->snapshot.revision + 2U);
    CHECK(agent->event_sequence == before->sequence);
    CHECK(stores_unchanged(before, agent));
    result = 0;
cleanup:
    free(before); fixture_destroy(agent); return result;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "run-full-queue") == 0) return run_full_queue_case();
    if (argc == 3 && strcmp(argv[1], "publication") == 0) return publication_case(argv[2]);
    if (argc == 4 && strcmp(argv[1], "agent") == 0)
        return agent_case(argv[2], strcmp(argv[3], "state") == 0);
    return 2;
}
