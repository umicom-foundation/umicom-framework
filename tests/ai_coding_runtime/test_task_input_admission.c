/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_task_input_admission.c
 *
 * PURPOSE:
 *   Verify bounded stable coding task construction and refusal preservation across value, agent and queue entry points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Keep actual agent/platform entry logic; stop planning before any provider,
 * filesystem or executor work. Queue storage remains the real Framework owner. */
#define umi_ai_coding_assistant_prepare input_fixture_prepare
#include "../../src/ai_coding_runtime/agent.c"
#undef umi_ai_coding_assistant_prepare
#include "../../src/ai_coding_runtime/platform.c"

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

static UmiAiCodingRequest observed_request;
static int prepare_calls;
UmiStatus input_fixture_prepare(UmiAiCodingAssistantService *assistant,
    const UmiAiCodingRequest *request, UmiAiProviderKind kind, UmiAiCodingTaskPlan *plan)
{
    (void)assistant; (void)kind; (void)plan;
    observed_request = *request; ++prepare_calls;
    return UMI_STATUS_IO_ERROR;
}
static const struct {
    const char *name;
    size_t offset, capacity;
} request_fields[] = {
#define FIELD(name) {#name, offsetof(UmiAiCodingRequest, name), sizeof(((UmiAiCodingRequest *)0)->name)}
    FIELD(request_id), FIELD(session_id), FIELD(runtime_id), FIELD(workspace_root),
    FIELD(active_path), FIELD(language_id), FIELD(instruction)
#undef FIELD
};


/* Compare defined request fields rather than compiler padding. */
static int same_request(const UmiAiCodingRequest *left, const UmiAiCodingRequest *right)
{
    for (size_t i = 0U; i < sizeof(request_fields) / sizeof(request_fields[0]); ++i)
        if (memcmp((const char *)left + request_fields[i].offset,
            (const char *)right + request_fields[i].offset, request_fields[i].capacity) != 0)
            return 0;
    return left->structure_size == right->structure_size && left->abi_version == right->abi_version &&
        left->task == right->task && left->classification == right->classification &&
        left->selection_start_line == right->selection_start_line &&
        left->selection_end_line == right->selection_end_line &&
        left->context_token_budget == right->context_token_budget &&
        left->maximum_context_files == right->maximum_context_files &&
        left->timestamp_ns == right->timestamp_ns && left->sensitive_approved == right->sensitive_approved;
}

static UmiAiCodingAgent *make_agent(void)
{
    UmiAiCodingAgent *agent = calloc(1U, sizeof(*agent));
    if (agent == NULL) return NULL;
    umi_ai_coding_runtime_config_init(&agent->config);
    strcpy(agent->config.provider_id, "fixture-provider");
    strcpy(agent->config.model_id, "fixture-model");
    umi_ai_coding_cancellation_init(&agent->cancellation);
    umi_ai_coding_cancellation_request(&agent->cancellation);
    agent->snapshot.revision = 17U;
    agent->snapshot.state = UMI_AI_CODING_RUNTIME_IDLE;
    strcpy(agent->snapshot.task_id, "prior-task");
    strcpy(agent->patch_outcome.task_id, "prior-task");
    strcpy(agent->patch_outcome.patch_id, "prior-patch");
    agent->has_patch_outcome = 1;
    if (umi_ai_coding_event_queue_create(&agent->events) != UMI_STATUS_OK ||
        umi_ai_coding_history_create(&agent->history) != UMI_STATUS_OK ||
        umi_ai_coding_audit_log_create(&agent->audit) != UMI_STATUS_OK) {
        umi_ai_coding_event_queue_destroy(agent->events);
        umi_ai_coding_history_destroy(agent->history);
        umi_ai_coding_audit_log_destroy(agent->audit);
        free(agent); return NULL;
    }
    return agent;
}
static void free_agent(UmiAiCodingAgent *agent)
{
    if (agent == NULL) return;
    umi_ai_coding_event_queue_destroy(agent->events);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_audit_log_destroy(agent->audit);
    free(agent);
}
static void make_request(UmiAiCodingRequest *request)
{
    umi_ai_coding_request_init(request, UMI_AI_CODING_TASK_REFACTOR);
    strcpy(request->request_id, "new-request");
    strcpy(request->session_id, "session");
    strcpy(request->runtime_id, "runtime");
    strcpy(request->workspace_root, "fixture-workspace");
    strcpy(request->instruction, "Review parser ownership");
}

/* Common invalid input reaches value construction, immediate execution and
 * platform queue construction. Check the owner that each entry can mutate. */
static int entry_case(const char *entry, const char *mode)
{
    UmiAiCodingRequest request;
    UmiAiCodingQueuedTask output, before_output, queued;
    UmiAiCodingAgent *agent = NULL, *before_agent = NULL;
    UmiAiCodingRuntimePlatform platform = {0};
    char id[UMI_AI_CODING_RUNTIME_ID_CAPACITY + 1U] = "new-task";
    const char *task_id = id;
    const UmiAiCodingRequest *request_pointer = &request;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int result = 1;
    make_request(&request); prepare_calls = 0;
    memset(&output, 0x5a, sizeof(output)); memcpy(&before_output, &output, sizeof(output));
    if (strcmp(mode, "null-id") == 0) task_id = NULL;
    else if (strcmp(mode, "null-request") == 0) request_pointer = NULL;
    else if (strcmp(mode, "empty-id") == 0) id[0] = '\0';
    else if (strcmp(mode, "overlong-id") == 0 || strcmp(mode, "unterminated-id") == 0) {
        memset(id, 'x', sizeof(id));
        if (strcmp(mode, "overlong-id") == 0) id[sizeof(id) - 1U] = '\0';
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "normal") == 0) expected = UMI_STATUS_OK;
    else if (strcmp(mode, "maximum-id") == 0) {
        memset(id, 'x', sizeof(id)); id[UMI_AI_CODING_RUNTIME_ID_CAPACITY - 1U] = '\0';
        expected = UMI_STATUS_OK;
    } else if (strcmp(mode, "draft") == 0) {
        memset(&request, 0, sizeof(request)); request.classification = (UmiAiDataClassification)999;
        expected = UMI_STATUS_OK; /* Text storage is not execution permission. */
    } else {
        int found = 0;
        for (size_t i = 0U; i < sizeof(request_fields) / sizeof(request_fields[0]); ++i)
            if (strcmp(mode, request_fields[i].name) == 0) {
                memset((char *)&request + request_fields[i].offset, 'x', request_fields[i].capacity);
                found = 1; break;
            }
        CHECK(found);
    }

    if (strcmp(entry, "value") == 0) {
        CHECK(UmiAiCodingQueuedTaskPrepare(task_id, request_pointer, &output) == expected);
        if (expected == UMI_STATUS_OK) {
            CHECK(strcmp(output.task_id, id) == 0 && output.sequence == 0U);
            CHECK(same_request(&output.request, &request));
        } else CHECK(memcmp(&output, &before_output, sizeof(output)) == 0);
    } else if (strcmp(entry, "agent") == 0) {
        agent = make_agent(); before_agent = malloc(sizeof(*before_agent));
        CHECK(agent != NULL && before_agent != NULL);
        /* Serialized byte preservation checks the refusal path, not semantic
         * equality between two independently constructed application records. */
        memcpy(before_agent, agent, sizeof(*before_agent));
        CHECK(umi_ai_coding_agent_run(agent, task_id, request_pointer) ==
            (expected == UMI_STATUS_OK ? UMI_STATUS_IO_ERROR : expected));
        if (expected == UMI_STATUS_OK) {
            CHECK(strcmp(agent->snapshot.task_id, id) == 0);
            CHECK(strcmp(agent->snapshot.request_id, request.request_id) == 0);
            CHECK(strcmp(observed_request.instruction, request.instruction) == 0);
            CHECK(agent->snapshot.revision == 19U && prepare_calls == 1);
        } else {
            CHECK(memcmp(before_agent, agent, sizeof(*before_agent)) == 0 && prepare_calls == 0);
            CHECK(umi_ai_coding_agent_event_count(agent) == 0U);
            CHECK(umi_ai_coding_history_count(agent->history) == 0U);
        }
    } else {
        CHECK(strcmp(entry, "enqueue") == 0);
        CHECK(umi_ai_coding_task_queue_create(&platform.task_queue) == UMI_STATUS_OK);
        platform.revision = 7U;
        UmiAiCodingRequest seed_request = {0};
        CHECK(UmiAiCodingQueuedTaskPrepare("retained", &seed_request, &queued) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_task_queue_push(platform.task_queue, &queued) == UMI_STATUS_OK);
        CHECK(coding_platform_enqueue_internal(&platform, task_id, request_pointer) == expected);
        CHECK(platform.revision == 7U); /* The outer platform owns parent revisions. */
        CHECK(umi_ai_coding_task_queue_count(platform.task_queue) == (expected == UMI_STATUS_OK ? 2U : 1U));
        CHECK(umi_ai_coding_task_queue_pop(platform.task_queue, &queued) == UMI_STATUS_OK);
        CHECK(strcmp(queued.task_id, "retained") == 0 && queued.sequence == 1U);
        if (expected == UMI_STATUS_OK) {
            CHECK(umi_ai_coding_task_queue_pop(platform.task_queue, &queued) == UMI_STATUS_OK);
            CHECK(strcmp(queued.task_id, id) == 0 && queued.sequence == 2U);
            CHECK(same_request(&queued.request, &request));
        }
    }
    result = 0;
cleanup:
    free(before_agent); free_agent(agent);
    umi_ai_coding_task_queue_destroy(platform.task_queue);
    return result;
}

static int alias_case(const char *mode)
{
    UmiAiCodingQueuedTask task;
    UmiAiCodingAgent *agent = NULL;
    UmiAiCodingRequest request;
    int result = 1;
    make_request(&request); prepare_calls = 0;
    if (strcmp(mode, "value") == 0) {
        memset(&task, 0, sizeof(task)); strcpy(task.task_id, "alias-task");
        task.request = request; task.sequence = 42U;
        CHECK(UmiAiCodingQueuedTaskPrepare(task.task_id, &task.request, &task) == UMI_STATUS_OK);
        CHECK(strcmp(task.task_id, "alias-task") == 0 && task.sequence == 0U);
        CHECK(strcmp(task.request.request_id, "new-request") == 0);
        CHECK(strcmp(task.request.instruction, request.instruction) == 0);
    } else {
        CHECK(strcmp(mode, "agent") == 0);
        agent = make_agent(); CHECK(agent != NULL);
        agent->plan.request = request;
        strcpy(agent->snapshot.task_id, "alias-task");
        CHECK(umi_ai_coding_agent_run(agent, agent->snapshot.task_id, &agent->plan.request) == UMI_STATUS_IO_ERROR);
        CHECK(strcmp(agent->snapshot.task_id, "alias-task") == 0);
        CHECK(strcmp(agent->snapshot.request_id, request.request_id) == 0);
        CHECK(strcmp(observed_request.instruction, request.instruction) == 0 && prepare_calls == 1);
    }
    result = 0;
cleanup:
    free_agent(agent); return result;
}

static int text_boundary_case(const char *field)
{
    UmiAiCodingRequest request = {0};
    UmiAiCodingQueuedTask output;
    int result = 1, found = 0;
    for (size_t i = 0U; i < sizeof(request_fields) / sizeof(request_fields[0]); ++i)
        if (strcmp(field, request_fields[i].name) == 0) {
            char *text = (char *)&request + request_fields[i].offset;
            memset(text, 'x', request_fields[i].capacity - 1U);
            text[request_fields[i].capacity - 1U] = '\0'; found = 1; break;
        }
    CHECK(found);
    CHECK(UmiAiCodingQueuedTaskPrepare("boundary", &request, &output) == UMI_STATUS_OK);
    CHECK(same_request(&output.request, &request));
    result = 0;
cleanup:
    return result;
}
int main(int argc, char **argv)
{
    if (argc == 4 && strcmp(argv[1], "entry") == 0) return entry_case(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "alias") == 0) return alias_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "boundary") == 0) return text_boundary_case(argv[2]);
    if (argc == 2 && strcmp(argv[1], "null-output") == 0) {
        UmiAiCodingRequest request = {0};
        return UmiAiCodingQueuedTaskPrepare("task", &request, NULL) == UMI_STATUS_INVALID_ARGUMENT ? 0 : 1;
    }
    return 2;
}
