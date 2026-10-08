/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_codec.c
 * PURPOSE: Validate portable archive records before publishing copied values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "archive_internal.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Fields are decimal numbers and hex-encoded text separated by '|'. A storage
 * marker permits future migrations without depending on compiler struct layout.
 * Wire builders are private scratch; public decoded objects publish only after
 * every field and cross-field invariant has been checked. */
typedef struct ArchiveWriter
{
    char *data;
    size_t capacity;
    size_t used;
    bool valid;
} ArchiveWriter;
static void number(ArchiveWriter *writer, uint64_t value)
{
    if (!writer->valid)
        return;
    int length =
        snprintf(writer->data + writer->used, writer->capacity - writer->used, "%" PRIu64 "|", value);
    if (length < 0 || (size_t)length >= writer->capacity - writer->used)
    {
        writer->valid = false;
        return;
    }
    writer->used += (size_t)length;
}
static void text(ArchiveWriter *writer, const char *value)
{
    static const char digits[] = "0123456789abcdef";
    if (!writer->valid)
        return;
    size_t length = strlen(value);
    if (length > (SIZE_MAX - 2U) / 2U || length * 2U + 2U > writer->capacity - writer->used)
    {
        writer->valid = false;
        return;
    }
    for (size_t i = 0; i < length; ++i)
    {
        unsigned char byte = (unsigned char)value[i];
        writer->data[writer->used++] = digits[byte >> 4U];
        writer->data[writer->used++] = digits[byte & 15U];
    }
    writer->data[writer->used++] = '|';
    writer->data[writer->used] = '\0';
}
static bool read_number(const char **cursor, uint64_t *out)
{
    const char *cursor_text = *cursor;
    uint64_t value = 0;
    if (*cursor_text < '0' || *cursor_text > '9')
        return false;
    do
    {
        unsigned digit = (unsigned)(*cursor_text - '0');
        if (value > (UINT64_MAX - digit) / 10U)
            return false;
        value = value * 10U + digit;
        ++cursor_text;
    } while (*cursor_text >= '0' && *cursor_text <= '9');
    if (*cursor_text != '|')
        return false;
    *out = value;
    *cursor = cursor_text + 1;
    return true;
}
static int digit(char character)
{
    if (character >= '0' && character <= '9')
        return character - '0';
    if (character >= 'a' && character <= 'f')
        return character - 'a' + 10;
    return -1;
}
static bool read_text(const char **cursor, char *out, size_t capacity)
{
    const char *cursor_text = *cursor;
    size_t count = 0;
    while (*cursor_text != '|')
    {
        int high = digit(*cursor_text);
        if (high < 0)
            return false;
        ++cursor_text;
        int low = digit(*cursor_text);
        if (low < 0 || count + 1U >= capacity)
            return false;
        ++cursor_text;
        unsigned byte = (unsigned)(high * 16 + low);
        if (byte == 0U)
            return false;
        out[count++] = (char)(unsigned char)byte;
    }
    out[count] = '\0';
    *cursor = cursor_text + 1;
    return true;
}
static bool string_field(const char *value, size_t capacity, bool required)
{
    return value != NULL && (!required || value[0] != '\0') && memchr(value, '\0', capacity) != NULL;
}
bool UmiTestArchiveOriginValid(const UmiTestArchiveOrigin *origin)
{
    return origin != NULL && string_field(origin->source_root, sizeof(origin->source_root), true) &&
           string_field(origin->source_revision, sizeof(origin->source_revision), false);
}
bool UmiTestArchiveRequestValid(const UmiCtestJobRequest *request)
{
    return request != NULL && string_field(request->test_id, sizeof(request->test_id), true) &&
           string_field(request->name, sizeof(request->name), true) &&
           string_field(request->build_directory, sizeof(request->build_directory), true) &&
           string_field(request->configuration, sizeof(request->configuration), false) &&
           (request->enabled == 0 || request->enabled == 1);
}
bool UmiTestArchiveResultValid(const UmiTestResult *result)
{
    if (result == NULL || !string_field(result->test_id, sizeof(result->test_id), true) ||
        !string_field(result->name, sizeof(result->name), true) ||
        !string_field(result->output, sizeof(result->output), false) ||
        (unsigned)result->status > (unsigned)UMI_STATUS_BUSY)
        return false;
    if ((unsigned)result->state > (unsigned)UMI_TEST_STATE_TIMED_OUT ||
        result->state == UMI_TEST_STATE_RUNNING)
        return false;
    /* A process/report error must never be recorded as a passing testcase. */
    return result->state != UMI_TEST_STATE_PASSED || result->status == UMI_STATUS_OK;
}
bool UmiTestArchiveEntryValid(const UmiTestArchiveEntry *entry)
{
    if (entry == NULL || entry->id == 0 || !UmiTestArchiveOriginValid(&entry->origin))
        return false;
    const UmiCtestJobPlanSnapshot *plan = &entry->plan;
    const UmiCtestJobSnapshot *run = &entry->run;
    if (plan->request_count == 0 || plan->request_count > UMI_CTEST_JOB_MAX_ATTEMPTS ||
        plan->repeat_count == 0 || plan->repeat_count > UMI_CTEST_JOB_MAX_ATTEMPTS / plan->request_count ||
        run->planned != plan->request_count * plan->repeat_count || run->completed > run->planned ||
        run->task_id == 0 || run->active_attempt != 0 || run->active_test_id[0] != '\0' ||
        run->active_name[0] != '\0' || (unsigned)run->status > (unsigned)UMI_STATUS_BUSY ||
        (unsigned)run->first_error > (unsigned)UMI_STATUS_BUSY)
        return false;
    if (run->state != UMI_TASK_SUCCEEDED && run->state != UMI_TASK_FAILED && run->state != UMI_TASK_CANCELLED)
        return false;
    if ((run->state == UMI_TASK_SUCCEEDED && run->status != UMI_STATUS_OK) ||
        (run->state == UMI_TASK_FAILED && run->status == UMI_STATUS_OK) ||
        (run->state == UMI_TASK_CANCELLED && run->status != UMI_STATUS_CANCELLED))
        return false;
    /* Subtract bounded counts rather than summing untrusted values that could
     * wrap. Every completed invocation has exactly one observed outcome. */
    const size_t counts[] = {run->passed,    run->failed,    run->skipped,
                             run->cancelled, run->timed_out, run->not_run};
    size_t left = run->completed;
    for (size_t i = 0; i < sizeof(counts) / sizeof(counts[0]); ++i)
    {
        if (counts[i] > left)
            return false;
        left -= counts[i];
    }
    return left == 0;
}
UmiStatus UmiTestArchiveEncodeEntry(const UmiTestArchiveEntry *entry, char *wire, size_t capacity)
{
    if (wire == NULL || capacity == 0 || !UmiTestArchiveEntryValid(entry))
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveWriter writer = {wire, capacity, 0, true};
    const UmiCtestJobSnapshot *run = &entry->run;
    const uint64_t fields[] = {1U,
                               entry->id,
                               entry->origin.workspace_generation,
                               entry->origin.retain_output,
                               entry->plan.request_count,
                               entry->plan.repeat_count,
                               entry->plan.stop_on_failure,
                               run->task_id,
                               (unsigned)run->state,
                               (unsigned)run->status,
                               (unsigned)run->first_error,
                               run->planned,
                               run->completed,
                               run->passed,
                               run->failed,
                               run->skipped,
                               run->cancelled,
                               run->timed_out,
                               run->not_run,
                               run->duration_ms};
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i)
        number(&writer, fields[i]);
    text(&writer, entry->origin.source_root);
    text(&writer, entry->origin.source_revision);
    return writer.valid ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiStatus UmiTestArchiveDecodeEntry(const char *wire, UmiTestArchiveEntry *output)
{
    if (wire == NULL || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestArchiveEntry entry = {0};
    uint64_t fields[20];
    const char *cursor = wire;
    for (size_t i = 0; i < 20U; ++i)
        if (!read_number(&cursor, &fields[i]))
            return UMI_STATUS_PARSE_ERROR;
    if (fields[0] != 1U || fields[3] > 1U || fields[4] > UMI_CTEST_JOB_MAX_ATTEMPTS ||
        fields[5] > UMI_CTEST_JOB_MAX_ATTEMPTS || fields[6] > 1U || fields[8] > UMI_TASK_CANCELLED ||
        fields[9] > UMI_STATUS_BUSY || fields[10] > UMI_STATUS_BUSY)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 11U; i < 19U; ++i)
        if (fields[i] > UMI_CTEST_JOB_MAX_ATTEMPTS)
            return UMI_STATUS_PARSE_ERROR;
    entry.id = fields[1];
    entry.origin.workspace_generation = fields[2];
    entry.origin.retain_output = fields[3] != 0;
    entry.plan.request_count = (size_t)fields[4];
    entry.plan.repeat_count = (uint32_t)fields[5];
    entry.plan.stop_on_failure = fields[6] != 0;
    entry.run.task_id = fields[7];
    entry.run.state = (UmiTaskState)fields[8];
    entry.run.status = (UmiStatus)fields[9];
    entry.run.first_error = (UmiStatus)fields[10];
    entry.run.planned = (size_t)fields[11];
    entry.run.completed = (size_t)fields[12];
    entry.run.passed = (size_t)fields[13];
    entry.run.failed = (size_t)fields[14];
    entry.run.skipped = (size_t)fields[15];
    entry.run.cancelled = (size_t)fields[16];
    entry.run.timed_out = (size_t)fields[17];
    entry.run.not_run = (size_t)fields[18];
    entry.run.duration_ms = fields[19];
    if (!read_text(&cursor, entry.origin.source_root, sizeof(entry.origin.source_root)) ||
        !read_text(&cursor, entry.origin.source_revision, sizeof(entry.origin.source_revision)) ||
        *cursor != '\0' || !UmiTestArchiveEntryValid(&entry))
        return UMI_STATUS_PARSE_ERROR;
    *output = entry;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveEncodeRequest(const UmiCtestJobRequest *request, char *wire, size_t capacity)
{
    if (wire == NULL || capacity == 0 || !UmiTestArchiveRequestValid(request))
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveWriter writer = {wire, capacity, 0, true};
    number(&writer, 1U);
    number(&writer, request->timeout_ms);
    number(&writer, (unsigned)request->enabled);
    text(&writer, request->test_id);
    text(&writer, request->name);
    text(&writer, request->build_directory);
    text(&writer, request->configuration);
    return writer.valid ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiStatus UmiTestArchiveDecodeRequest(const char *wire, UmiCtestJobRequest *output)
{
    if (wire == NULL || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCtestJobRequest request = {0};
    const char *cursor = wire;
    uint64_t fields[3];
    for (size_t i = 0; i < 3U; ++i)
        if (!read_number(&cursor, &fields[i]))
            return UMI_STATUS_PARSE_ERROR;
    if (fields[0] != 1U || fields[1] > UINT32_MAX || fields[2] > 1U)
        return UMI_STATUS_PARSE_ERROR;
    request.timeout_ms = (uint32_t)fields[1];
    request.enabled = (int)fields[2];
    if (!read_text(&cursor, request.test_id, sizeof(request.test_id)) ||
        !read_text(&cursor, request.name, sizeof(request.name)) ||
        !read_text(&cursor, request.build_directory, sizeof(request.build_directory)) ||
        !read_text(&cursor, request.configuration, sizeof(request.configuration)) || *cursor != '\0' ||
        !UmiTestArchiveRequestValid(&request))
        return UMI_STATUS_PARSE_ERROR;
    *output = request;
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveEncodeResult(const UmiTestResult *result, char *wire, size_t capacity)
{
    if (wire == NULL || capacity == 0 || !UmiTestArchiveResultValid(result))
        return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveWriter writer = {wire, capacity, 0, true};
    /* Widen before negating so INT_MIN has a representable positive magnitude. */
    /* Offset the negative side before negation, including a widest-width int. */
    int64_t signed_code = result->exit_code;
    uint64_t magnitude = signed_code < 0 ? (uint64_t)(-(signed_code + 1)) + 1U : (uint64_t)signed_code;
    number(&writer, 1U);
    number(&writer, (unsigned)result->state);
    number(&writer, (unsigned)result->status);
    number(&writer, signed_code < 0);
    number(&writer, magnitude);
    number(&writer, result->duration_ms);
    text(&writer, result->test_id);
    text(&writer, result->name);
    text(&writer, result->output);
    return writer.valid ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
UmiStatus UmiTestArchiveDecodeResult(const char *wire, UmiTestResult *output)
{
    if (wire == NULL || output == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A diagnostic tail is large; keep staging off a GUI thread's small stack. */
    UmiTestResult *result = calloc(1U, sizeof(*result));
    if (result == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    const char *cursor = wire;
    uint64_t fields[6];
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0; i < 6U; ++i)
        if (!read_number(&cursor, &fields[i]))
            goto done;
    if (fields[0] != 1U || fields[1] > UMI_TEST_STATE_TIMED_OUT || fields[2] > UMI_STATUS_BUSY ||
        fields[3] > 1U || fields[4] > (fields[3] ? ((uint64_t)(-(INT_MIN + 1)) + 1U) : (uint64_t)INT_MAX))
        goto done;
    result->state = (UmiTestState)fields[1];
    result->status = (UmiStatus)fields[2];
    result->exit_code = fields[3] ? (fields[4] == 0 ? 0 : -(int)(fields[4] - 1U) - 1) : (int)fields[4];
    result->duration_ms = fields[5];
    if (!read_text(&cursor, result->test_id, sizeof(result->test_id)) ||
        !read_text(&cursor, result->name, sizeof(result->name)) ||
        !read_text(&cursor, result->output, sizeof(result->output)) || *cursor != '\0' ||
        !UmiTestArchiveResultValid(result))
        goto done;
    *output = *result;
    status = UMI_STATUS_OK;
done:
    free(result);
    return status;
}
