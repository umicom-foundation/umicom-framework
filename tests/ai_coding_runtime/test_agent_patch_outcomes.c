/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_agent_patch_outcomes.c
 *
 * PURPOSE:
 *   Verify correlated agent patch outcomes survive notification failure and reset only for an admitted new task.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Stub only the already-tested assistant transaction boundary. Compile the
 * actual agent and composition code to verify forwarding and retained outcomes. */
#define UmiAiCodingAssistantApplyPatchWithReport fixture_apply_report
#define UmiAiCodingAssistantRevertPatchWithReport fixture_revert_report
#include "../../src/ai_coding_runtime/agent.c"
#undef UmiAiCodingAssistantApplyPatchWithReport
#undef UmiAiCodingAssistantRevertPatchWithReport
#include "../../src/ai_coding_runtime/platform.c"

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

static struct {
    UmiAiCodingAgent *agent;
    const char *mode;
    int calls, direction, callback_failed;
} outcome_fixture;

static UmiStatus fixture_report(const char *patch_id, int reverting,
    UmiAiCodingPatchTransactionReport *report)
{
    ++outcome_fixture.calls; outcome_fixture.direction = reverting;
    if (strcmp(patch_id, "patch.current") != 0) outcome_fixture.callback_failed = 1;
    UmiAiCodingAgentPatchOutcome observed;
    UmiStatus read_status = UmiAiCodingAgentReadPatchOutcome(outcome_fixture.agent, &observed);
    if (strcmp(outcome_fixture.mode, "previous-during") == 0) {
        if (read_status != UMI_STATUS_OK || strcmp(observed.patch_id, "patch.previous") != 0)
            outcome_fixture.callback_failed = 1;
    } else if (read_status != UMI_STATUS_NOT_FOUND) outcome_fixture.callback_failed = 1;
    if (strcmp(outcome_fixture.mode, "admission-refusal") == 0) return UMI_STATUS_PERMISSION_DENIED;
    memset(report, 0, sizeof(*report));
    report->admitted = 1; report->verified_files = 1U;
    report->mutation_attempts = 1U;
    report->revision_before = 9U; report->revision_after = 10U;
    report->failed_file_index = SIZE_MAX;
    if (strcmp(outcome_fixture.mode, "primary-failure") == 0 ||
        strcmp(outcome_fixture.mode, "recovery-failure") == 0 ||
        strcmp(outcome_fixture.mode, "readback-failure") == 0) {
        report->primary_status = UMI_STATUS_IO_ERROR;
        report->failed_file_index = 0U;
        report->compensation_attempts = 1U; report->restoration_checks = 1U;
        if (strcmp(outcome_fixture.mode, "recovery-failure") == 0) {
            report->compensation_status = UMI_STATUS_PERMISSION_DENIED;
            report->compensation_failures = 1U;
        }
        if (strcmp(outcome_fixture.mode, "readback-failure") == 0) {
            report->compensation_status = UMI_STATUS_IO_ERROR;
            report->restoration_failures = 1U;
        }
        return UMI_STATUS_IO_ERROR;
    }
    report->mutations_completed = 1U;
    return UMI_STATUS_OK;
}

UmiStatus fixture_apply_report(UmiAiCodingAssistantService *service, const char *id,
    const UmiAiCodingFileAdapter *adapter, UmiAiCodingPatchTransactionReport *report)
{
    (void)service; (void)adapter; return fixture_report(id, 0, report);
}
UmiStatus fixture_revert_report(UmiAiCodingAssistantService *service, const char *id,
    const UmiAiCodingFileAdapter *adapter, UmiAiCodingPatchTransactionReport *report)
{
    (void)service; (void)adapter; return fixture_report(id, 1, report);
}

static UmiStatus unused_read(void *user, const char *path, char *out, size_t capacity, size_t *length)
{
    (void)user; (void)path; (void)out; (void)capacity; (void)length;
    return UMI_STATUS_NOT_FOUND;
}
static UmiStatus unused_write(void *user, const char *path, const char *text, size_t length)
{
    (void)user; (void)path; (void)text; (void)length; return UMI_STATUS_IO_ERROR;
}
static UmiStatus unused_remove(void *user, const char *path)
{
    (void)user; (void)path; return UMI_STATUS_NOT_FOUND;
}
static UmiStatus unused_exists(void *user, const char *path, int *exists)
{
    (void)user; (void)path; *exists = 0; return UMI_STATUS_OK;
}

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
    strcpy(agent->snapshot.task_id, "task.current");
    strcpy(agent->snapshot.patch_id, "patch.current");
    agent->snapshot.state = UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED;
    agent->snapshot.revision = 7U;
    agent->bindings.workspace = (UmiAiCodingWorkspaceAdapter){
        sizeof(UmiAiCodingWorkspaceAdapter), UMI_AI_CODING_RUNTIME_API_VERSION,
        unused_read, unused_write, unused_remove, unused_exists, NULL};
    memset(&outcome_fixture, 0, sizeof(outcome_fixture)); outcome_fixture.agent = agent;
    return agent;
}
static void fixture_destroy(UmiAiCodingAgent *agent)
{
    if (agent == NULL) return;
    umi_ai_coding_event_queue_destroy(agent->events);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_audit_log_destroy(agent->audit);
    free(agent); outcome_fixture.agent = NULL;
}
static void seed_previous(UmiAiCodingAgent *agent)
{
    agent->has_patch_outcome = 1;
    strcpy(agent->patch_outcome.task_id, "task.previous");
    strcpy(agent->patch_outcome.patch_id, "patch.previous");
    agent->patch_outcome.transaction.admitted = 1;
}
static UmiStatus fill_events(UmiAiCodingAgent *agent, size_t count)
{
    UmiAiCodingEvent event = {0};
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = umi_ai_coding_event_queue_push(agent->events, &event);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static int flow_case(const char *mode, int reverting)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingAgentPatchOutcome outcome, sentinel;
    UmiAiCodingRuntimePlatform platform = {0};
    int result = 1;
    CHECK(agent != NULL); platform.agent = agent; outcome_fixture.mode = mode;
    if (strcmp(mode, "previous-during") == 0) seed_previous(agent);
    if (strcmp(mode, "invalid-snapshot") == 0) memset(agent->snapshot.task_id, 'x', sizeof(agent->snapshot.task_id));
    if (strcmp(mode, "notification-failure") == 0)
        CHECK(fill_events(agent, UMI_AI_CODING_RUNTIME_EVENT_CAPACITY - (reverting ? 0U : 1U)) == UMI_STATUS_OK);
    UmiStatus expected = strcmp(mode, "admission-refusal") == 0 ? UMI_STATUS_PERMISSION_DENIED :
        strcmp(mode, "notification-failure") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED :
        strcmp(mode, "invalid-snapshot") == 0 ? UMI_STATUS_INVALID_ARGUMENT :
        (strcmp(mode, "primary-failure") == 0 || strcmp(mode, "recovery-failure") == 0 ||
         strcmp(mode, "readback-failure") == 0) ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
    UmiStatus status = reverting ? revert_patch(agent) : apply_patch(agent);
    CHECK(status == expected);
    memset(&outcome, 0x5a, sizeof(outcome)); memcpy(&sentinel, &outcome, sizeof(outcome));
    size_t event_count = umi_ai_coding_agent_event_count(agent);
    if (strcmp(mode, "invalid-snapshot") == 0) {
        CHECK(outcome_fixture.calls == 0);
        CHECK(UmiAiCodingRuntimePlatformReadPatchOutcome(&platform, &outcome) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&outcome, &sentinel, sizeof(outcome)) == 0);
    } else {
        CHECK(outcome_fixture.calls == 1 && outcome_fixture.direction == reverting && !outcome_fixture.callback_failed);
        CHECK(UmiAiCodingRuntimePlatformReadPatchOutcome(&platform, &outcome) == UMI_STATUS_OK);
        CHECK(strcmp(outcome.task_id, "task.current") == 0 && strcmp(outcome.patch_id, "patch.current") == 0);
        CHECK(outcome.reverting == reverting);
        UmiStatus patch_status = strcmp(mode, "notification-failure") == 0 ? UMI_STATUS_OK : expected;
        CHECK(outcome.status == patch_status);
        if (strcmp(mode, "admission-refusal") == 0) CHECK(outcome.transaction.admitted == 0);
        else {
            CHECK(outcome.transaction.admitted == 1 && outcome.transaction.primary_status == patch_status);
            CHECK(outcome.transaction.mutation_attempts == 1U && outcome.transaction.revision_after == 10U);
            CHECK(outcome.transaction.compensation_failures == (size_t)(strcmp(mode, "recovery-failure") == 0));
            CHECK(outcome.transaction.restoration_failures == (size_t)(strcmp(mode, "readback-failure") == 0));
        }
        CHECK(umi_ai_coding_agent_event_count(agent) == event_count);
    }
    result = 0;
cleanup:
    fixture_destroy(agent); return result;
}

static int read_case(const char *mode)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingRuntimePlatform platform = {0};
    UmiAiCodingAgentPatchOutcome outcome, sentinel;
    int result = 1;
    CHECK(agent != NULL); platform.agent = agent;
    if (strcmp(mode, "missing") != 0) seed_previous(agent);
    if (strcmp(mode, "corrupt-task") == 0) memset(agent->patch_outcome.task_id, 'x', sizeof(agent->patch_outcome.task_id));
    if (strcmp(mode, "corrupt-patch") == 0) memset(agent->patch_outcome.patch_id, 'x', sizeof(agent->patch_outcome.patch_id));
    if (strcmp(mode, "active-read") == 0) agent->operation_active = 1;
    memset(&outcome, 0x5a, sizeof(outcome)); memcpy(&sentinel, &outcome, sizeof(outcome));
    UmiStatus status = strcmp(mode, "null-platform") == 0 ? UmiAiCodingRuntimePlatformReadPatchOutcome(NULL, &outcome) :
        UmiAiCodingAgentReadPatchOutcome(strcmp(mode, "null-agent") == 0 ? NULL : agent,
            strcmp(mode, "null-output") == 0 ? NULL : &outcome);
    UmiStatus expected = strncmp(mode, "null-", 5U) == 0 ? UMI_STATUS_INVALID_ARGUMENT :
        strcmp(mode, "missing") == 0 ? UMI_STATUS_NOT_FOUND :
        strncmp(mode, "corrupt-", 8U) == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    CHECK(status == expected);
    if (expected != UMI_STATUS_OK) CHECK(memcmp(&outcome, &sentinel, sizeof(outcome)) == 0);
    else {
        CHECK(strcmp(outcome.patch_id, "patch.previous") == 0);
        outcome.patch_id[0] = 'X'; outcome.transaction.mutation_attempts = 999U;
        CHECK(UmiAiCodingRuntimePlatformReadPatchOutcome(&platform, &outcome) == UMI_STATUS_OK);
        CHECK(strcmp(outcome.patch_id, "patch.previous") == 0 && outcome.transaction.mutation_attempts == 0U);
    }
    result = 0;
cleanup:
    fixture_destroy(agent); return result;
}

static int lifecycle_case(const char *mode)
{
    UmiAiCodingAgent *agent = fixture_create();
    UmiAiCodingRequest request = {0};
    UmiAiCodingAgentPatchOutcome outcome;
    int result = 1;
    CHECK(agent != NULL); seed_previous(agent);
    if (strcmp(mode, "busy-run") != 0) agent->snapshot.state = UMI_AI_CODING_RUNTIME_IDLE;
    if (strcmp(mode, "admitted-reset") == 0) CHECK(fill_events(agent, UMI_AI_CODING_RUNTIME_EVENT_CAPACITY) == UMI_STATUS_OK);
    UmiStatus status = coding_agent_run_internal(agent, strcmp(mode, "empty-task") == 0 ? "" : "next-task", &request);
    if (strcmp(mode, "admitted-reset") == 0) {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiCodingAgentReadPatchOutcome(agent, &outcome) == UMI_STATUS_NOT_FOUND);
    } else {
        CHECK(status == (strcmp(mode, "empty-task") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_BUSY));
        CHECK(UmiAiCodingAgentReadPatchOutcome(agent, &outcome) == UMI_STATUS_OK);
        CHECK(strcmp(outcome.patch_id, "patch.previous") == 0);
    }
    CHECK(outcome_fixture.calls == 0);
    result = 0;
cleanup:
    fixture_destroy(agent); return result;
}

int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "flow") == 0) return flow_case(argv[2], strcmp(argv[3], "revert") == 0);
    if (argc == 3 && strcmp(argv[1], "read") == 0) return read_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "lifecycle") == 0) return lifecycle_case(argv[2]);
    return 2;
}
