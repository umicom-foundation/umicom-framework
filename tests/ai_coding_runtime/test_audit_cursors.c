/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_audit_cursors.c
 *
 * PURPOSE:
 *   Verify sequence-based audit reads report retention gaps and refuse malformed or exhausted admission atomically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/audit.h"
#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
/* Public cases link the SDK implementation. Renamed private copies permit
 * impossible-owner and exhaustion injection without exposing mutable storage. */
#define umi_ai_coding_audit_log_create fixture_audit_create
#define umi_ai_coding_audit_log_destroy fixture_audit_destroy
#define umi_ai_coding_audit_log_append fixture_audit_append
#define umi_ai_coding_audit_log_at fixture_audit_at
#define umi_ai_coding_audit_log_count fixture_audit_count
#define UmiAiCodingAuditLogAppendBatch fixture_audit_batch
#define UmiAiCodingAuditLogCheckAppendBatch fixture_audit_check_batch
#define UmiAiCodingAuditLogReadSince fixture_audit_read
#define UmiAiCodingAuditRecordValidateText fixture_audit_validate
#include "../../src/ai_coding_runtime/audit.c"
#undef umi_ai_coding_audit_log_create
#undef umi_ai_coding_audit_log_destroy
#undef umi_ai_coding_audit_log_append
#undef umi_ai_coding_audit_log_at
#undef umi_ai_coding_audit_log_count
#undef UmiAiCodingAuditLogAppendBatch
#undef UmiAiCodingAuditLogCheckAppendBatch
#undef UmiAiCodingAuditLogReadSince
#undef UmiAiCodingAuditRecordValidateText

static void InitRecords(UmiAiCodingAuditRecord *records, size_t count)
{
    memset(records, 0, count * sizeof(records[0]));
    for (size_t i = 0U; i < count; ++i) {
        records[i].sequence = 777U;
        records[i].plan_hash = (uint64_t)i;
        strcpy(records[i].task_id, "task");
        strcpy(records[i].summary, "Stored evidence");
    }
}

static void PublicCase(const char *name)
{
    UmiAiCodingAuditLog *log = NULL;
    UmiAiCodingAuditRecord *input = calloc(UMI_AI_CODING_AUDIT_CAPACITY, sizeof(*input));
    UmiAiCodingAuditRecord *output = malloc(UMI_AI_CODING_AUDIT_CAPACITY * sizeof(*output));
    UmiAiCodingAuditRecord *sentinel = malloc(UMI_AI_CODING_AUDIT_CAPACITY * sizeof(*sentinel));
    assert(input != NULL && output != NULL && sentinel != NULL);
    InitRecords(input, UMI_AI_CODING_AUDIT_CAPACITY);
    memset(output, 0x5a, UMI_AI_CODING_AUDIT_CAPACITY * sizeof(*output));
    memcpy(sentinel, output, UMI_AI_CODING_AUDIT_CAPACITY * sizeof(*output));
    UmiAiCodingAuditRead report, reportBefore;
    memset(&report, 0x5a, sizeof(report)); memcpy(&reportBefore, &report, sizeof(report));
    assert(umi_ai_coding_audit_log_create(&log) == UMI_STATUS_OK);
    if (strcmp(name, "empty") == 0) {
        assert(UmiAiCodingAuditLogReadSince(log, 0U, output, 1U, &report) == UMI_STATUS_OK);
        assert(report.count == 0U && report.oldest_sequence == 0U && report.newest_sequence == 0U);
        assert(report.next_sequence == 0U && report.missed_records == 0U && !report.has_more);
        assert(memcmp(output, sentinel, sizeof(*output)) == 0);
        assert(umi_ai_coding_audit_log_at(log, 0U, output) == UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "null-record") == 0 || strcmp(name, "huge") == 0 || strcmp(name, "zero") == 0) {
        if (strcmp(name, "null-record") == 0) {
            assert(UmiAiCodingAuditRecordValidateText(NULL) == UMI_STATUS_INVALID_ARGUMENT);
            assert(umi_ai_coding_audit_log_append(log, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        } else if (strcmp(name, "huge") == 0)
            assert(UmiAiCodingAuditLogAppendBatch(log, input, SIZE_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        else assert(UmiAiCodingAuditLogAppendBatch(log, NULL, 0U) == UMI_STATUS_OK);
        assert(umi_ai_coding_audit_log_count(log) == 0U);
    } else {
        assert(UmiAiCodingAuditLogAppendBatch(log, input, 3U) == UMI_STATUS_OK);
        if (strcmp(name, "null-owner") == 0 || strcmp(name, "null-output") == 0 || strcmp(name, "null-report") == 0 ||
            strcmp(name, "future") == 0 || strcmp(name, "maximum-cursor") == 0) {
            uint64_t cursor = strcmp(name, "future") == 0 ? 4U : strcmp(name, "maximum-cursor") == 0 ? UINT64_MAX : 0U;
            UmiStatus expected = cursor != 0U ? UMI_STATUS_INVALID_STATE : UMI_STATUS_INVALID_ARGUMENT;
            assert(UmiAiCodingAuditLogReadSince(strcmp(name, "null-owner") == 0 ? NULL : log, cursor,
                strcmp(name, "null-output") == 0 ? NULL : output, 2U,
                strcmp(name, "null-report") == 0 ? NULL : &report) == expected);
            assert(memcmp(&report, &reportBefore, sizeof(report)) == 0);
            assert(memcmp(output, sentinel, UMI_AI_CODING_AUDIT_CAPACITY * sizeof(*output)) == 0);
        } else if (strcmp(name, "ordered") == 0 || strcmp(name, "copy") == 0 || strcmp(name, "page") == 0) {
            size_t capacity = strcmp(name, "page") == 0 ? 2U : UMI_AI_CODING_AUDIT_CAPACITY;
            assert(UmiAiCodingAuditLogReadSince(log, 0U, output, capacity, &report) == UMI_STATUS_OK);
            assert(report.count == (capacity < 3U ? capacity : 3U) && report.missed_records == 0U);
            assert(report.oldest_sequence == 1U && report.newest_sequence == 3U);
            assert(report.next_sequence == report.count && report.has_more == (capacity < 3U));
            for (size_t i = 0U; i < report.count; ++i) assert(output[i].sequence == i + 1U && output[i].plan_hash == i);
            if (strcmp(name, "copy") == 0) {
                strcpy(input[0].summary, "changed input"); strcpy(output[0].summary, "changed output");
                assert(umi_ai_coding_audit_log_at(log, 0U, output) == UMI_STATUS_OK);
                assert(strcmp(output[0].summary, "Stored evidence") == 0);
            }
        } else if (strcmp(name, "at-end") == 0 || strcmp(name, "metadata") == 0) {
            uint64_t cursor = strcmp(name, "at-end") == 0 ? 3U : 0U;
            assert(UmiAiCodingAuditLogReadSince(log, cursor, NULL, 0U, &report) == UMI_STATUS_OK);
            assert(report.count == 0U && report.next_sequence == cursor && report.has_more == (cursor == 0U));
            assert(report.oldest_sequence == 1U && report.newest_sequence == 3U);
        } else if (strcmp(name, "scalar") == 0) {
            input[0].state = (UmiAiCodingRuntimeState)1234; input[0].kind = (UmiAiCodingEventKind)4321;
            input[0].status = (UmiStatus)2468; input[0].file_count = SIZE_MAX; input[0].changed_lines = UINT32_MAX;
            assert(umi_ai_coding_audit_log_append(log, input) == UMI_STATUS_OK);
            assert(umi_ai_coding_audit_log_at(log, 3U, output) == UMI_STATUS_OK);
            assert(output[0].state == input[0].state && output[0].kind == input[0].kind && output[0].status == input[0].status);
            assert(output[0].file_count == SIZE_MAX && output[0].changed_lines == UINT32_MAX && output[0].sequence == 4U);
        } else {
            assert(strcmp(name, "gap") == 0 || strcmp(name, "gap-boundary") == 0 || strcmp(name, "continue") == 0 || strcmp(name, "replace") == 0);
            assert(UmiAiCodingAuditLogAppendBatch(log, input, UMI_AI_CODING_AUDIT_CAPACITY) == UMI_STATUS_OK);
            assert(umi_ai_coding_audit_log_count(log) == UMI_AI_CODING_AUDIT_CAPACITY);
            uint64_t cursor = strcmp(name, "gap-boundary") == 0 ? 3U : 0U;
            assert(UmiAiCodingAuditLogReadSince(log, cursor, output, 2U, &report) == UMI_STATUS_OK);
            assert(report.oldest_sequence == 4U && report.newest_sequence == 131U);
            assert(report.count == 2U && report.next_sequence == 5U && report.missed_records == (cursor == 0U ? 3U : 0U));
            assert(output[0].sequence == 4U && output[1].sequence == 5U && report.has_more);
            if (strcmp(name, "continue") == 0) {
                assert(UmiAiCodingAuditLogReadSince(log, report.next_sequence, output, UMI_AI_CODING_AUDIT_CAPACITY, &report) == UMI_STATUS_OK);
                assert(report.count == 126U && report.next_sequence == 131U && report.missed_records == 0U && !report.has_more);
                assert(output[0].sequence == 6U && output[125].sequence == 131U);
            } else if (strcmp(name, "replace") == 0) {
                assert(umi_ai_coding_audit_log_at(log, 0U, output) == UMI_STATUS_OK);
                assert(output[0].plan_hash == 0U && output[0].sequence == 4U);
            }
        }
    }
    umi_ai_coding_audit_log_destroy(log); free(input); free(output); free(sentinel);
}

static void PrivateCase(const char *name)
{
    UmiAiCodingAuditLog *log = NULL;
    assert(fixture_audit_create(&log) == UMI_STATUS_OK);
    UmiAiCodingAuditRecord records[2]; InitRecords(records, 2U);
    assert(fixture_audit_batch(log, records, 2U) == UMI_STATUS_OK);
    UmiAiCodingAuditRecord output[2], before[2];
    memset(output, 0x5a, sizeof(output)); memcpy(before, output, sizeof(output));
    UmiAiCodingAuditRead report, reportBefore;
    memset(&report, 0x5a, sizeof(report)); memcpy(&reportBefore, &report, sizeof(report));
    UmiAiCodingAuditLog *ownerBefore = malloc(sizeof(*ownerBefore)); assert(ownerBefore != NULL);
    if (strcmp(name, "last-identity") == 0 || strcmp(name, "exhaustion") == 0 || strcmp(name, "batch-exhaustion") == 0) {
        log->sequence = strcmp(name, "exhaustion") == 0 ? UINT64_MAX : UINT64_MAX - 1U;
        log->items[0].sequence = log->sequence-1U; log->items[1].sequence = log->sequence;
        memcpy(ownerBefore, log, sizeof(*log));
        if (strcmp(name, "last-identity") == 0) {
            assert(fixture_audit_append(log, records) == UMI_STATUS_OK);
            assert(log->sequence == UINT64_MAX && log->count == 3U);
            assert(fixture_audit_read(log, UINT64_MAX-1U, output, 2U, &report) == UMI_STATUS_OK);
            assert(report.count == 1U && report.next_sequence == UINT64_MAX && output[0].sequence == UINT64_MAX);
            memcpy(ownerBefore, log, sizeof(*log));
        }
        assert(fixture_audit_batch(log, records, strcmp(name, "batch-exhaustion") == 0 ? 2U : 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(memcmp(ownerBefore, log, sizeof(*log)) == 0);
    } else {
        if (strcmp(name, "count") == 0) log->count = UMI_AI_CODING_AUDIT_CAPACITY + 1U;
        else if (strcmp(name, "sequence-low") == 0) log->sequence = 1U;
        else if (strcmp(name, "sequence-gap") == 0) log->items[0].sequence = 0U;
        else if (strcmp(name, "empty-sequence") == 0) log->count = 0U;
        else { assert(strcmp(name, "stored-text") == 0); memset(log->items[1].summary, 'x', sizeof(log->items[1].summary)); }
        memcpy(ownerBefore, log, sizeof(*log));
        assert(fixture_audit_read(log, 0U, output, 2U, &report) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(output, before, sizeof(output)) == 0 && memcmp(&report, &reportBefore, sizeof(report)) == 0);
        assert(fixture_audit_at(log, 1U, output) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(output, before, sizeof(output)) == 0);
        if (strcmp(name, "stored-text") != 0) assert(fixture_audit_append(log, records) == UMI_STATUS_INVALID_STATE);
        assert(memcmp(ownerBefore, log, sizeof(*log)) == 0);
    }
    free(ownerBefore); fixture_audit_destroy(log);
}

static void TextCase(const char *mode, const char *field)
{
    UmiAiCodingAuditLog *log = NULL;
    assert(umi_ai_coding_audit_log_create(&log) == UMI_STATUS_OK);
    UmiAiCodingAuditRecord *records = calloc(UMI_AI_CODING_AUDIT_CAPACITY, sizeof(*records)); assert(records != NULL);
    InitRecords(records, UMI_AI_CODING_AUDIT_CAPACITY);
    assert(UmiAiCodingAuditLogAppendBatch(log, records, UMI_AI_CODING_AUDIT_CAPACITY) == UMI_STATUS_OK);
    char *text = strcmp(field, "task-id") == 0 ? records[1].task_id : records[1].summary;
    size_t size = strcmp(field, "task-id") == 0 ? sizeof(records[1].task_id) : sizeof(records[1].summary);
    memset(text, 'x', size);
    if (strcmp(mode, "last-byte") == 0) text[size-1U] = 0;
    else if (strcmp(mode, "empty") == 0) text[0] = 0;
    else assert(strcmp(mode, "unterminated") == 0);
    UmiStatus expected = strcmp(mode, "unterminated") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
    assert(UmiAiCodingAuditRecordValidateText(&records[1]) == expected);
    assert(UmiAiCodingAuditLogAppendBatch(log, records, 2U) == expected);
    UmiAiCodingAuditRecord first;
    assert(umi_ai_coding_audit_log_at(log, 0U, &first) == UMI_STATUS_OK);
    assert(first.sequence == (expected == UMI_STATUS_OK ? 3U : 1U));
    UmiAiCodingAuditRead report;
    assert(UmiAiCodingAuditLogReadSince(log, 0U, NULL, 0U, &report) == UMI_STATUS_OK);
    assert(report.newest_sequence == (expected == UMI_STATUS_OK ? 130U : 128U));
    assert(report.missed_records == (expected == UMI_STATUS_OK ? 2U : 0U));
    umi_ai_coding_audit_log_destroy(log); free(records);
}
int main(int argc, char **argv)
{
    assert(argc >= 3);
    if (strcmp(argv[1], "public") == 0) PublicCase(argv[2]);
    else if (strcmp(argv[1], "private") == 0) PrivateCase(argv[2]);
    else { assert(argc == 4 && strcmp(argv[1], "text") == 0); TextCase(argv[2], argv[3]); }
    return 0;
}
