/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_event_storage.c
 *
 * PURPOSE:
 *   Verify event text admission, atomic FIFO batches, bounded draining and failure preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/event_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Public cases use the linked SDK. Renamed implementation fixtures exercise
 * corrupted private bounds without adding mutation hooks to the public API. */
#define umi_ai_coding_event_queue_create fixture_events_create
#define umi_ai_coding_event_queue_destroy fixture_events_destroy
#define umi_ai_coding_event_queue_push fixture_events_push
#define umi_ai_coding_event_queue_pop fixture_events_pop
#define umi_ai_coding_event_queue_count fixture_events_count
#define UmiAiCodingEventQueuePushBatch fixture_events_batch
#define UmiAiCodingEventQueueCheckPushBatch fixture_events_check_batch
#define UmiAiCodingEventQueuePopBatch fixture_events_drain
#include "../../src/ai_coding_runtime/event_queue.c"
#undef umi_ai_coding_event_queue_create
#undef umi_ai_coding_event_queue_destroy
#undef umi_ai_coding_event_queue_push
#undef umi_ai_coding_event_queue_pop
#undef umi_ai_coding_event_queue_count
#undef UmiAiCodingEventQueuePushBatch
#undef UmiAiCodingEventQueueCheckPushBatch
#undef UmiAiCodingEventQueuePopBatch
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); goto cleanup; } } while (0)

static void event_init(UmiAiCodingEvent *event, unsigned i)
{
    memset(event, 0, sizeof(*event));
    event->sequence = i;
    event->kind = UMI_AI_CODING_EVENT_MESSAGE;
    event->state = UMI_AI_CODING_RUNTIME_IDLE;
    (void)snprintf(event->task_id, sizeof event->task_id, "task-%u", i);
    (void)snprintf(event->message, sizeof event->message, "message-%u", i);
}

static int public_case(const char *name)
{
    UmiAiCodingEventQueue *queue = NULL;
    UmiAiCodingEvent *input = calloc(UMI_AI_CODING_RUNTIME_EVENT_CAPACITY + 1U, sizeof(*input));
    UmiAiCodingEvent *output = malloc((UMI_AI_CODING_RUNTIME_EVENT_CAPACITY + 1U) * sizeof(*output));
    size_t copied = 77U;
    int rc = 1;
    CHECK(input != NULL && output != NULL);
    for (unsigned i = 0U; i <= UMI_AI_CODING_RUNTIME_EVENT_CAPACITY; ++i) event_init(&input[i], i);
    memset(output, 0x5a, (UMI_AI_CODING_RUNTIME_EVENT_CAPACITY + 1U) * sizeof(*output));
    CHECK(umi_ai_coding_event_queue_create(&queue) == UMI_STATUS_OK);
    if (strcmp(name, "null-record") == 0) {
        CHECK(UmiAiCodingEventValidateText(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_coding_event_queue_push(queue, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "empty") == 0) {
        CHECK(UmiAiCodingEventQueuePopBatch(queue, output, 2U, &copied) == UMI_STATUS_OK && copied == 0U);
        CHECK(((unsigned char *)output)[0] == 0x5a);
        CHECK(umi_ai_coding_event_queue_pop(queue, output) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "huge") == 0) {
        CHECK(UmiAiCodingEventQueuePushBatch(queue, input, SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ai_coding_event_queue_count(queue) == 0U);
    } else if (strcmp(name, "zero") == 0) {
        CHECK(UmiAiCodingEventQueuePushBatch(queue, input, 2U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePushBatch(queue, NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePopBatch(queue, NULL, 0U, &copied) == UMI_STATUS_OK && copied == 0U);
        CHECK(umi_ai_coding_event_queue_count(queue) == 2U);
    } else if (strcmp(name, "null-output") == 0 || strcmp(name, "null-count") == 0 || strcmp(name, "null-owner") == 0) {
        CHECK(UmiAiCodingEventQueuePushBatch(queue, input, 2U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePopBatch(strcmp(name, "null-owner") == 0 ? NULL : queue,
            strcmp(name, "null-output") == 0 ? NULL : output, 2U,
            strcmp(name, "null-count") == 0 ? NULL : &copied) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(copied == 77U && umi_ai_coding_event_queue_count(queue) == 2U);
        CHECK(((unsigned char *)output)[0] == 0x5a);
    } else if (strcmp(name, "scalar") == 0) {
        input[0].sequence = UINT64_MAX;
        input[0].kind = (UmiAiCodingEventKind)99;
        input[0].state = (UmiAiCodingRuntimeState)99;
        input[0].status = (UmiStatus)-1;
        input[0].task_id[0] = '\0'; input[0].message[0] = '\0';
        CHECK(UmiAiCodingEventValidateText(input) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_event_queue_push(queue, input) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_event_queue_pop(queue, output) == UMI_STATUS_OK);
        CHECK(output[0].sequence == UINT64_MAX && output[0].kind == (UmiAiCodingEventKind)99);
        CHECK(output[0].state == (UmiAiCodingRuntimeState)99 && output[0].status == (UmiStatus)-1);
    } else if (strcmp(name, "copy") == 0) {
        CHECK(UmiAiCodingEventQueuePushBatch(queue, input, 2U) == UMI_STATUS_OK);
        memset(input, 0xa5, 2U * sizeof(*input));
        CHECK(UmiAiCodingEventQueuePopBatch(queue, output, 2U, &copied) == UMI_STATUS_OK && copied == 2U);
        CHECK(strcmp(output[1].message, "message-1") == 0);
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(umi_ai_coding_event_queue_push(queue, input) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePushBatch(queue, input, UMI_AI_CODING_RUNTIME_EVENT_CAPACITY) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ai_coding_event_queue_count(queue) == 1U);
        CHECK(UmiAiCodingEventQueuePushBatch(queue, &input[1], 2U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePopBatch(queue, output, 3U, &copied) == UMI_STATUS_OK && copied == 3U);
        CHECK(output[0].sequence == 0U && output[2].sequence == 2U);
    } else if (strcmp(name, "full") == 0) {
        CHECK(UmiAiCodingEventQueuePushBatch(queue, input, UMI_AI_CODING_RUNTIME_EVENT_CAPACITY) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_event_queue_push(queue, input) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiAiCodingEventQueuePushBatch(queue, NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePopBatch(queue, output, UMI_AI_CODING_RUNTIME_EVENT_CAPACITY + 1U, &copied) == UMI_STATUS_OK);
        CHECK(copied == UMI_AI_CODING_RUNTIME_EVENT_CAPACITY);
        for (size_t i = 0U; i < copied; ++i) CHECK(output[i].sequence == i);
        CHECK(((unsigned char *)&output[copied])[0] == 0x5a);
    } else if (strcmp(name, "paged") == 0) {
        CHECK(umi_ai_coding_event_queue_push(queue, input) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePushBatch(queue, &input[1], 4U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingEventQueuePopBatch(queue, output, 2U, &copied) == UMI_STATUS_OK && copied == 2U);
        CHECK(output[0].sequence == 0U && output[1].sequence == 1U);
        CHECK(umi_ai_coding_event_queue_count(queue) == 3U);
        CHECK(UmiAiCodingEventQueuePopBatch(queue, output, 4U, &copied) == UMI_STATUS_OK && copied == 3U);
        CHECK(output[0].sequence == 2U && output[2].sequence == 4U);
    } else if (strcmp(name, "wrap") == 0) {
        for (unsigned cycle = 0U; cycle < 20U; ++cycle) {
            CHECK(UmiAiCodingEventQueuePushBatch(queue, input, 53U) == UMI_STATUS_OK);
            CHECK(UmiAiCodingEventQueuePopBatch(queue, output, 53U, &copied) == UMI_STATUS_OK && copied == 53U);
            for (size_t i = 0U; i < copied; ++i) CHECK(output[i].sequence == i);
        }
    } else goto cleanup;
    rc = 0;
cleanup:
    umi_ai_coding_event_queue_destroy(queue); free(input); free(output); return rc;
}

static int text_case(const char *mode, const char *name)
{
    UmiAiCodingEventQueue *queue = NULL;
    UmiAiCodingEvent events[3], output;
    int rc = 1;
    for (unsigned i = 0U; i < 3U; ++i) event_init(&events[i], i);
    char *text = strcmp(name, "task-id") == 0 ? events[2].task_id : events[2].message;
    size_t size = strcmp(name, "task-id") == 0 ? sizeof events[2].task_id : sizeof events[2].message;
    memset(text, 'x', size);
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "last-byte") == 0) { text[size - 1U] = '\0'; expected = UMI_STATUS_OK; }
    CHECK(UmiAiCodingEventValidateText(&events[2]) == expected);
    CHECK(umi_ai_coding_event_queue_create(&queue) == UMI_STATUS_OK);
    CHECK(umi_ai_coding_event_queue_push(queue, &events[0]) == UMI_STATUS_OK);
    CHECK(UmiAiCodingEventQueuePushBatch(queue, &events[1], 2U) == expected);
    CHECK(umi_ai_coding_event_queue_count(queue) == (expected == UMI_STATUS_OK ? 3U : 1U));
    CHECK(umi_ai_coding_event_queue_pop(queue, &output) == UMI_STATUS_OK && output.sequence == 0U);
    rc = 0;
cleanup:
    umi_ai_coding_event_queue_destroy(queue); return rc;
}

static int private_case(const char *name)
{
    UmiAiCodingEventQueue *queue = NULL, *before = malloc(sizeof(*before));
    UmiAiCodingEvent input[2], output[2], saved[2];
    size_t copied = 77U;
    int rc = 1;
    CHECK(before != NULL);
    event_init(&input[0], 0U); event_init(&input[1], 1U);
    CHECK(fixture_events_create(&queue) == UMI_STATUS_OK);
    CHECK(fixture_events_batch(queue, input, 2U) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(name, "head") == 0) queue->head = SIZE_MAX;
    else if (strcmp(name, "count") == 0) queue->count = SIZE_MAX;
    else if (strcmp(name, "stored-text") == 0) {
        memset(queue->items[1].message, 'x', sizeof queue->items[1].message);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else goto cleanup;
    memcpy(before, queue, sizeof(*before));
    memset(output, 0x5a, sizeof output); memcpy(saved, output, sizeof saved);
    CHECK(fixture_events_drain(queue, output, 2U, &copied) == expected);
    CHECK(copied == 77U && memcmp(output, saved, sizeof output) == 0);
    CHECK(memcmp(queue, before, sizeof(*before)) == 0);
    if (expected == UMI_STATUS_INVALID_STATE)
        CHECK(fixture_events_batch(queue, input, 1U) == expected);
    rc = 0;
cleanup:
    fixture_events_destroy(queue); free(before); return rc;
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "public") == 0) return public_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "private") == 0) return private_case(argv[2]);
    if (argc == 4 && strcmp(argv[1], "text") == 0) return text_case(argv[2], argv[3]);
    return 2;
}
