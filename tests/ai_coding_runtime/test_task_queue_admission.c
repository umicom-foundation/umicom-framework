/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_task_queue_admission.c
 *
 * PURPOSE:
 *   Verify atomic FIFO batches, bounded drafts, stable identity exhaustion and failure-preserving reads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/task_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Public scenarios below call the linked SDK. A renamed copy of the same
 * implementation permits otherwise unreachable sequence/invariant fixtures
 * without exposing an owner-state mutation API in the product. */
#define umi_ai_coding_task_queue_create fixture_queue_create
#define umi_ai_coding_task_queue_destroy fixture_queue_destroy
#define umi_ai_coding_task_queue_push fixture_queue_push
#define UmiAiCodingTaskQueuePushBatch fixture_queue_batch
/* Keep the new value constructor private in this duplicate implementation. */
#define UmiAiCodingQueuedTaskPrepare fixture_queue_prepare
#define umi_ai_coding_task_queue_pop fixture_queue_pop
#define umi_ai_coding_task_queue_peek fixture_queue_peek
#define umi_ai_coding_task_queue_count fixture_queue_count
#define umi_ai_coding_task_queue_clear fixture_queue_clear
#include "../../src/ai_coding_runtime/task_queue.c"
#undef umi_ai_coding_task_queue_create
#undef umi_ai_coding_task_queue_destroy
#undef umi_ai_coding_task_queue_push
#undef UmiAiCodingTaskQueuePushBatch
#undef UmiAiCodingQueuedTaskPrepare
#undef umi_ai_coding_task_queue_pop
#undef umi_ai_coding_task_queue_peek
#undef umi_ai_coding_task_queue_count
#undef umi_ai_coding_task_queue_clear

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); goto cleanup; } } while (0)
static void make_task(UmiAiCodingQueuedTask *task, unsigned number)
{
    memset(task, 0, sizeof(*task));
    (void)snprintf(task->task_id, sizeof task->task_id, "task-%u", number);
    umi_ai_coding_request_init(&task->request, UMI_AI_CODING_TASK_CHAT);
    task->request.timestamp_ns = number;
    task->sequence = UINT64_MAX; /* Caller identities must be ignored. */
}

static int reject_case(const char *name)
{
    UmiAiCodingTaskQueue *queue = NULL;
    UmiAiCodingQueuedTask *tasks = calloc(2U, sizeof(*tasks));
    UmiAiCodingQueuedTask seed, result;
    int rc = 1;
    CHECK(tasks != NULL);
    make_task(&seed, 0U); make_task(&tasks[0], 1U); make_task(&tasks[1], 2U);
#define BAD_TEXT(member) memset(tasks[1].member, 'x', sizeof tasks[1].member)
    if (strcmp(name, "task-id") == 0) { BAD_TEXT(task_id); }
    else if (strcmp(name, "request-id") == 0) { BAD_TEXT(request.request_id); }
    else if (strcmp(name, "session-id") == 0) { BAD_TEXT(request.session_id); }
    else if (strcmp(name, "runtime-id") == 0) { BAD_TEXT(request.runtime_id); }
    else if (strcmp(name, "workspace") == 0) { BAD_TEXT(request.workspace_root); }
    else if (strcmp(name, "active-path") == 0) { BAD_TEXT(request.active_path); }
    else if (strcmp(name, "language") == 0) { BAD_TEXT(request.language_id); }
    else if (strcmp(name, "instruction") == 0) { BAD_TEXT(request.instruction); }
    else if (strcmp(name, "empty-id") == 0) tasks[1].task_id[0] = '\0';
    else if (strcmp(name, "null-task") != 0) goto cleanup;
#undef BAD_TEXT
    CHECK(umi_ai_coding_task_queue_create(&queue) == UMI_STATUS_OK);
    CHECK(umi_ai_coding_task_queue_push(queue, &seed) == UMI_STATUS_OK);
    CHECK(UmiAiCodingTaskQueuePushBatch(queue,
        strcmp(name, "null-task") == 0 ? NULL : tasks, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_ai_coding_task_queue_count(queue) == 1U);
    CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
    CHECK(result.sequence == 1U && strcmp(result.task_id, "task-0") == 0);
    CHECK(umi_ai_coding_task_queue_push(queue, &tasks[0]) == UMI_STATUS_OK);
    CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
    CHECK(result.sequence == 2U && strcmp(result.task_id, "task-1") == 0);
    rc = 0;
cleanup:
    umi_ai_coding_task_queue_destroy(queue); free(tasks); return rc;
}

static int fifo_case(const char *name)
{
    UmiAiCodingTaskQueue *queue = NULL;
    UmiAiCodingQueuedTask *tasks = calloc(UMI_AI_CODING_TASK_QUEUE_CAPACITY + 1U, sizeof(*tasks));
    UmiAiCodingQueuedTask result;
    int rc = 1;
    CHECK(tasks != NULL);
    for (unsigned i = 0U; i <= UMI_AI_CODING_TASK_QUEUE_CAPACITY; ++i) make_task(&tasks[i], i);
    CHECK(umi_ai_coding_task_queue_create(&queue) == UMI_STATUS_OK);
    if (strcmp(name, "empty") == 0) {
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_task_queue_count(queue) == 0U);
    } else if (strcmp(name, "huge") == 0) {
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ai_coding_task_queue_count(queue) == 0U);
    } else if (strcmp(name, "duplicates") == 0) {
        tasks[1] = tasks[0];
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, 2U) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK && result.sequence == 1U);
        CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK && result.sequence == 2U);
        CHECK(strcmp(result.task_id, "task-0") == 0);
    } else if (strcmp(name, "copy") == 0) {
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, 2U) == UMI_STATUS_OK);
        memset(tasks, 0xa5, 2U * sizeof(*tasks));
        CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
        CHECK(strcmp(result.task_id, "task-0") == 0 && result.request.timestamp_ns == 0U);
        CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
        CHECK(strcmp(result.task_id, "task-1") == 0 && result.request.timestamp_ns == 1U);
    } else if (strcmp(name, "clear") == 0) {
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, 2U) == UMI_STATUS_OK);
        umi_ai_coding_task_queue_clear(queue);
        CHECK(umi_ai_coding_task_queue_count(queue) == 0U);
        CHECK(umi_ai_coding_task_queue_push(queue, &tasks[2]) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK && result.sequence == 3U);
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(umi_ai_coding_task_queue_push(queue, &tasks[0]) == UMI_STATUS_OK);
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, UMI_AI_CODING_TASK_QUEUE_CAPACITY) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ai_coding_task_queue_count(queue) == 1U);
        CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK && result.sequence == 1U);
    } else if (strcmp(name, "batch") == 0) {
        CHECK(umi_ai_coding_task_queue_push(queue, &tasks[0]) == UMI_STATUS_OK);
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, &tasks[1], 3U) == UMI_STATUS_OK);
        for (unsigned i = 0U; i < 4U; ++i) {
            CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
            CHECK(result.sequence == (uint64_t)i + 1U && result.request.timestamp_ns == i);
        }
        CHECK(umi_ai_coding_task_queue_count(queue) == 0U);
    } else if (strcmp(name, "full") == 0) {
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, UMI_AI_CODING_TASK_QUEUE_CAPACITY) == UMI_STATUS_OK);
        CHECK(UmiAiCodingTaskQueuePushBatch(queue, NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_task_queue_push(queue, tasks) == UMI_STATUS_CAPACITY_EXCEEDED);
        for (unsigned i = 0U; i < UMI_AI_CODING_TASK_QUEUE_CAPACITY; ++i) {
            CHECK(umi_ai_coding_task_queue_peek(queue, &result) == UMI_STATUS_OK);
            CHECK(result.sequence == (uint64_t)i + 1U && result.request.timestamp_ns == i);
            CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
            CHECK(result.sequence == (uint64_t)i + 1U && result.request.timestamp_ns == i);
        }
        CHECK(umi_ai_coding_task_queue_count(queue) == 0U);
    } else if (strcmp(name, "wrap") == 0) {
        uint64_t assigned = 0U;
        for (unsigned cycle = 0U; cycle < 20U; ++cycle) {
            CHECK(UmiAiCodingTaskQueuePushBatch(queue, tasks, 19U) == UMI_STATUS_OK);
            for (unsigned i = 0U; i < 19U; ++i) {
                CHECK(umi_ai_coding_task_queue_pop(queue, &result) == UMI_STATUS_OK);
                CHECK(result.sequence == ++assigned && result.request.timestamp_ns == i);
            }
        }
    } else goto cleanup;
    rc = 0;
cleanup:
    umi_ai_coding_task_queue_destroy(queue); free(tasks); return rc;
}

static int invariant_case(const char *name)
{
    UmiAiCodingTaskQueue *queue = NULL;
    UmiAiCodingTaskQueue *before = malloc(sizeof(*before));
    UmiAiCodingQueuedTask tasks[2], output, output_before;
    int rc = 1;
    CHECK(before != NULL);
    make_task(&tasks[0], 1U); make_task(&tasks[1], 2U);
    CHECK(fixture_queue_create(&queue) == UMI_STATUS_OK);
    if (strcmp(name, "last-identity") == 0) {
        queue->sequence = UINT64_MAX - 1U;
        CHECK(fixture_queue_push(queue, tasks) == UMI_STATUS_OK);
        CHECK(fixture_queue_pop(queue, &output) == UMI_STATUS_OK && output.sequence == UINT64_MAX);
        CHECK(fixture_queue_push(queue, tasks) == UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(name, "batch-overflow") == 0 || strcmp(name, "exhausted") == 0 || strcmp(name, "clear-exhausted") == 0) {
        queue->sequence = strcmp(name, "batch-overflow") == 0 ? UINT64_MAX - 1U : UINT64_MAX;
        if (strcmp(name, "clear-exhausted") == 0) fixture_queue_clear(queue);
        memcpy(before, queue, sizeof(*before));
        CHECK(fixture_queue_batch(queue, tasks, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(before, queue, sizeof(*before)) == 0);
        CHECK(fixture_queue_batch(queue, NULL, 0U) == UMI_STATUS_OK);
        CHECK(memcmp(before, queue, sizeof(*before)) == 0);
    } else if (strcmp(name, "head") == 0 || strcmp(name, "count") == 0) {
        if (strcmp(name, "head") == 0) queue->head = SIZE_MAX;
        else queue->count = SIZE_MAX;
        memcpy(before, queue, sizeof(*before));
        memset(&output, 0x5a, sizeof output);
        memcpy(&output_before, &output, sizeof output);
        CHECK(fixture_queue_batch(queue, tasks, 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(fixture_queue_pop(queue, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(fixture_queue_peek(queue, &output) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(before, queue, sizeof(*before)) == 0);
        CHECK(memcmp(&output_before, &output, sizeof output) == 0);
    } else goto cleanup;
    rc = 0;
cleanup:
    fixture_queue_destroy(queue); free(before); return rc;
}

static int output_case(const char *name)
{
    UmiAiCodingTaskQueue *queue = NULL;
    UmiAiCodingQueuedTask output, before;
    int rc = 1;
    memset(&output, 0x5a, sizeof output); memcpy(&before, &output, sizeof output);
    CHECK(umi_ai_coding_task_queue_create(&queue) == UMI_STATUS_OK);
    if (strcmp(name, "empty") == 0) {
        CHECK(umi_ai_coding_task_queue_pop(queue, &output) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_ai_coding_task_queue_peek(queue, &output) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "null-owner") == 0) {
        CHECK(umi_ai_coding_task_queue_pop(NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_coding_task_queue_peek(NULL, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiCodingTaskQueuePushBatch(NULL, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "null-result") == 0) {
        CHECK(umi_ai_coding_task_queue_pop(queue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_coding_task_queue_peek(queue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else goto cleanup;
    CHECK(memcmp(&before, &output, sizeof output) == 0);
    rc = 0;
cleanup:
    umi_ai_coding_task_queue_destroy(queue); return rc;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "reject") == 0) return reject_case(argv[2]);
    if (strcmp(argv[1], "fifo") == 0) return fifo_case(argv[2]);
    if (strcmp(argv[1], "invariant") == 0) return invariant_case(argv[2]);
    if (strcmp(argv[1], "output") == 0) return output_case(argv[2]);
    return 2;
}
