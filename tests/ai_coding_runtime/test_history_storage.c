/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_history_storage.c
 *
 * PURPOSE:
 *   Verify atomic snapshot retention, bounded range copying and preserved historical metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define umi_ai_coding_history_create fixture_history_create
#define umi_ai_coding_history_destroy fixture_history_destroy
#define umi_ai_coding_history_record fixture_history_record
#define umi_ai_coding_history_at fixture_history_at
#define umi_ai_coding_history_count fixture_history_count
#define UmiAiCodingHistoryRecordBatch fixture_history_batch
#define UmiAiCodingHistoryCheckRecordBatch fixture_history_check_batch
#define UmiAiCodingHistoryCopyRange fixture_history_range
#include "../../src/ai_coding_runtime/history.c"
#undef umi_ai_coding_history_create
#undef umi_ai_coding_history_destroy
#undef umi_ai_coding_history_record
#undef umi_ai_coding_history_at
#undef umi_ai_coding_history_count
#undef UmiAiCodingHistoryRecordBatch
#undef UmiAiCodingHistoryCheckRecordBatch
#undef UmiAiCodingHistoryCopyRange
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); goto cleanup; } } while (0)

static void snapshot_init(UmiAiCodingAgentSnapshot *snapshot, unsigned i)
{
    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->revision = i;
    snapshot->state = UMI_AI_CODING_RUNTIME_COMPLETED;
    (void)snprintf(snapshot->task_id, sizeof snapshot->task_id, "task-%u", i);
}

static int public_case(const char *name)
{
    UmiAiCodingHistory *history = NULL;
    UmiAiCodingAgentSnapshot *input = calloc(UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY, sizeof(*input));
    UmiAiCodingAgentSnapshot *output = malloc((UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY + 1U) * sizeof(*output));
    size_t copied = 77U;
    int rc = 1;
    CHECK(input != NULL && output != NULL);
    for (unsigned i = 0U; i < UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY; ++i) snapshot_init(&input[i], i);
    memset(output, 0x5a, (UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY + 1U) * sizeof(*output));
    CHECK(umi_ai_coding_history_create(&history) == UMI_STATUS_OK);
    if (strcmp(name, "null-record") == 0) {
        CHECK(UmiAiCodingAgentSnapshotValidateText(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_ai_coding_history_record(history, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "huge") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_ai_coding_history_count(history) == 0U);
    } else if (strcmp(name, "empty") == 0) {
        CHECK(UmiAiCodingHistoryCopyRange(history, 0U, output, 1U, &copied) == UMI_STATUS_OK && copied == 0U);
        CHECK(umi_ai_coding_history_at(history, 0U, output) == UMI_STATUS_NOT_FOUND);
        CHECK(((unsigned char *)output)[0] == 0x5a);
    } else if (strcmp(name, "zero") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, 3U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryRecordBatch(history, NULL, 0U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryCopyRange(history, 0U, NULL, 0U, &copied) == UMI_STATUS_OK && copied == 0U);
        CHECK(umi_ai_coding_history_count(history) == 3U);
    } else if (strcmp(name, "null-output") == 0 || strcmp(name, "null-count") == 0 || strcmp(name, "null-owner") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, 2U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryCopyRange(strcmp(name, "null-owner") == 0 ? NULL : history,
            0U, strcmp(name, "null-output") == 0 ? NULL : output, 2U,
            strcmp(name, "null-count") == 0 ? NULL : &copied) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(copied == 77U && umi_ai_coding_history_count(history) == 2U);
        CHECK(((unsigned char *)output)[0] == 0x5a);
    } else if (strcmp(name, "past-end") == 0 || strcmp(name, "maximum-offset") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, 2U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryCopyRange(history, strcmp(name, "past-end") == 0 ? 3U : SIZE_MAX,
            output, 2U, &copied) == UMI_STATUS_NOT_FOUND);
        CHECK(copied == 77U && ((unsigned char *)output)[0] == 0x5a);
    } else if (strcmp(name, "end") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, 2U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryCopyRange(history, 2U, output, 2U, &copied) == UMI_STATUS_OK && copied == 0U);
        CHECK(umi_ai_coding_history_at(history, 2U, output) == UMI_STATUS_NOT_FOUND);
        CHECK(((unsigned char *)output)[0] == 0x5a);
    } else if (strcmp(name, "copy") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, 2U) == UMI_STATUS_OK);
        memset(input, 0xa5, 2U * sizeof(*input));
        CHECK(UmiAiCodingHistoryCopyRange(history, 0U, output, 2U, &copied) == UMI_STATUS_OK && copied == 2U);
        CHECK(strcmp(output[1].task_id, "task-1") == 0);
    } else if (strcmp(name, "scalar") == 0) {
        input[0].state = (UmiAiCodingRuntimeState)99;
        input[0].last_status = (UmiStatus)-1;
        input[0].revision = UINT64_MAX;
        input[0].context_files = SIZE_MAX; input[0].patch_files = SIZE_MAX;
        input[0].validation_steps = SIZE_MAX; input[0].validation_failures = SIZE_MAX;
        input[0].cancelled = -7; input[0].iteration = UINT32_MAX;
        input[0].task_id[0] = '\0';
        CHECK(UmiAiCodingAgentSnapshotValidateText(input) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_history_record(history, input) == UMI_STATUS_OK);
        CHECK(umi_ai_coding_history_at(history, 0U, output) == UMI_STATUS_OK);
        CHECK(output[0].revision == UINT64_MAX && output[0].context_files == SIZE_MAX);
        CHECK(output[0].state == (UmiAiCodingRuntimeState)99 && output[0].last_status == (UmiStatus)-1);
        CHECK(output[0].cancelled == -7 && output[0].iteration == UINT32_MAX);
    } else if (strcmp(name, "range") == 0) {
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, 5U) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryCopyRange(history, 1U, output, 2U, &copied) == UMI_STATUS_OK && copied == 2U);
        CHECK(output[0].revision == 1U && output[1].revision == 2U);
        CHECK(((unsigned char *)&output[2])[0] == 0x5a);
        CHECK(umi_ai_coding_history_count(history) == 5U);
    } else if (strcmp(name, "retain") == 0 || strcmp(name, "replace") == 0 || strcmp(name, "single-eviction") == 0 || strcmp(name, "partial-retain") == 0) {
        unsigned initial = strcmp(name, "partial-retain") == 0 ? UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY - 3U : UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY;
        CHECK(UmiAiCodingHistoryRecordBatch(history, input, initial) == UMI_STATUS_OK);
        unsigned added = strcmp(name, "replace") == 0 ? UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY :
            strcmp(name, "single-eviction") == 0 ? 1U : 7U;
        for (unsigned i = 0U; i < added; ++i) snapshot_init(&input[i], 100U + i);
        CHECK((added == 1U ? umi_ai_coding_history_record(history, input) :
            UmiAiCodingHistoryRecordBatch(history, input, added)) == UMI_STATUS_OK);
        CHECK(UmiAiCodingHistoryCopyRange(history, 0U, output,
            UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY + 1U, &copied) == UMI_STATUS_OK);
        CHECK(copied == UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY);
        for (size_t i = 0U; i < copied; ++i) {
            size_t retained = UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY - added;
            size_t dropped = initial + added - UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY;
            uint64_t expected = i < retained ? i + dropped : 100U + i - retained;
            CHECK(output[i].revision == expected);
        }
        CHECK(((unsigned char *)&output[copied])[0] == 0x5a);
    } else goto cleanup;
    rc = 0;
cleanup:
    umi_ai_coding_history_destroy(history); free(input); free(output); return rc;
}

static int text_case(const char *mode, const char *name)
{
    UmiAiCodingHistory *history = NULL;
    UmiAiCodingAgentSnapshot *inputs = calloc(UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY, sizeof(*inputs));
    UmiAiCodingAgentSnapshot batch[2], output;
    int rc = 1;
    CHECK(inputs != NULL);
    for (unsigned i = 0U; i < UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY; ++i) snapshot_init(&inputs[i], i);
    snapshot_init(&batch[0], 100U); snapshot_init(&batch[1], 101U);
    char *text = NULL; size_t size = 0U;
#define SELECT_FIELD(label, member) if (strcmp(name, label) == 0) { text = batch[1].member; size = sizeof batch[1].member; }
    SELECT_FIELD("task-id", task_id)
    SELECT_FIELD("request-id", request_id)
    SELECT_FIELD("provider-id", provider_id)
    SELECT_FIELD("model-id", model_id)
    SELECT_FIELD("patch-id", patch_id)
#undef SELECT_FIELD
    CHECK(text != NULL);
    memset(text, 'x', size);
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "last-byte") == 0) { text[size - 1U] = '\0'; expected = UMI_STATUS_OK; }
    CHECK(UmiAiCodingAgentSnapshotValidateText(&batch[1]) == expected);
    CHECK(umi_ai_coding_history_create(&history) == UMI_STATUS_OK);
    CHECK(UmiAiCodingHistoryRecordBatch(history, inputs, UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY) == UMI_STATUS_OK);
    CHECK(UmiAiCodingHistoryRecordBatch(history, batch, 2U) == expected);
    CHECK(umi_ai_coding_history_count(history) == UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY);
    CHECK(umi_ai_coding_history_at(history, 0U, &output) == UMI_STATUS_OK);
    CHECK(output.revision == (expected == UMI_STATUS_OK ? 2U : 0U));
    rc = 0;
cleanup:
    umi_ai_coding_history_destroy(history); free(inputs); return rc;
}

static int private_case(const char *name)
{
    UmiAiCodingHistory *history = NULL, *before = malloc(sizeof(*before));
    UmiAiCodingAgentSnapshot inputs[2], output[2], saved[2];
    size_t copied = 77U;
    int rc = 1;
    CHECK(before != NULL);
    snapshot_init(&inputs[0], 0U); snapshot_init(&inputs[1], 1U);
    CHECK(fixture_history_create(&history) == UMI_STATUS_OK);
    CHECK(fixture_history_batch(history, inputs, 2U) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(name, "count") == 0) history->count = SIZE_MAX;
    else if (strcmp(name, "stored-text") == 0) {
        memset(history->items[1].model_id, 'x', sizeof history->items[1].model_id);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else goto cleanup;
    memcpy(before, history, sizeof(*before));
    memset(output, 0x5a, sizeof output); memcpy(saved, output, sizeof saved);
    CHECK(fixture_history_range(history, 0U, output, 2U, &copied) == expected);
    CHECK(copied == 77U && memcmp(output, saved, sizeof output) == 0);
    CHECK(memcmp(before, history, sizeof(*before)) == 0);
    if (expected == UMI_STATUS_INVALID_STATE) CHECK(fixture_history_batch(history, inputs, 1U) == expected);
    rc = 0;
cleanup:
    fixture_history_destroy(history); free(before); return rc;
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "public") == 0) return public_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "private") == 0) return private_case(argv[2]);
    if (argc == 4 && strcmp(argv[1], "text") == 0) return text_case(argv[2], argv[3]);
    return 2;
}
