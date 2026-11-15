/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/codeguard/test_bounded_reader.c
 *
 * PURPOSE:
 *   Verify native-chunk adaptation, cancellation, text admission and candidate publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/source.h"
#include "umicom/codeguard/normalizer.h"
#include "umicom/platform/path.h"
#include "../../src/codeguard/source_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static const unsigned char *input;
static size_t input_size, chunk_size = 3U;
static unsigned visits;
static uint64_t observed_limit;
static UmiStatus native_result = UMI_STATUS_OK;
static UmiCancellationToken *token;
static int cancel_on_return, native_path_absolute;
static UmiStatus NativeVisit(const char *path, uint64_t maximum,
    UmiInputChunkVisitor visitor, void *context, uint64_t *out_size)
{
    ++visits;
    observed_limit = maximum;
    native_path_absolute = umi_path_is_absolute(path);
    *out_size = 0U;
    if ((uint64_t)input_size > maximum) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t offset = 0U; offset < input_size;) {
        size_t size = input_size - offset;
        if (size > chunk_size) size = chunk_size;
        UmiStatus status = visitor(input + offset, size, context);
        if (status != UMI_STATUS_OK) return status;
        offset += size;
    }
    if (cancel_on_return) umi_cancellation_token_request(token);
    if (native_result == UMI_STATUS_OK) *out_size = input_size;
    return native_result;
}

/* Replace only native I/O. The path adapter, segmenter, measurement and hash
 * algorithms below are production code. Separate filesystem cases exercise
 * the actual platform reader; this seam deterministically injects late errors. */
#define UmiInputFileVisit NativeVisit
#include "../../src/codeguard/source_reader.c"
#undef UmiInputFileVisit
#include "../../src/codeguard/source.c"
#include "../../src/codeguard/normalizer.c"

static size_t lines, bytes, first_size, last_size;
static char collected[20000];
static UmiStatus callback_result = UMI_STATUS_OK;
static int cancel_in_callback;
static UmiStatus Line(const char *text, size_t size, size_t number, void *context)
{
    (void)context;
    if (number != lines + 1U || strlen(text) != size ||
        bytes > sizeof collected - size) return UMI_STATUS_INVALID_STATE;
    if (lines == 0U) first_size = size;
    last_size = size;
    memcpy(collected + bytes, text, size);
    bytes += size;
    ++lines;
    if (cancel_in_callback) umi_cancellation_token_request(token);
    return callback_result;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    static const char *cases[] = {
        "arguments", "relative", "empty", "exact", "over", "zero-over",
        "segments", "long", "newline-boundary", "crlf", "nul", "late-nul",
        "late-read", "late-change", "late-close", "cancel-before", "cancel-line",
        "cancel-empty", "failure-cancel", "callback-failure", "line-overflow",
        "measure", "measure-failure", "measure-alias", "hash", "hash-binary",
        "hash-failure", "hash-overflow", "missing", "trailing-slash"
    };
    size_t selected = 0U;
    while (selected < sizeof cases / sizeof cases[0] && strcmp(cases[selected], name)) ++selected;
    if (selected == sizeof cases / sizeof cases[0]) return 2;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    input = (const unsigned char *)"ab\ncd\nx";
    input_size = 7U;
    unsigned char long_input[16390];
    uint64_t maximum = UINT64_MAX;
    if (!strcmp(name, "arguments")) {
        CHECK(umi_codeguard_source_visit_lines(NULL, maximum, Line, NULL, token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_source_visit_lines("", maximum, Line, NULL, token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_source_visit_lines("sample.c", maximum, NULL, NULL, token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_codeguard_source_visit_bytes("sample.c", maximum, NULL, NULL, token) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(visits == 0U);
    } else if (!strcmp(name, "line-overflow")) {
        UmiCodeGuardLineVisit visit = {.used = 1U, .number = SIZE_MAX,
            .visitor = Line, .cancellation = token};
        visit.text[0] = 'x';
        CHECK(umi_codeguard_reader_line(&visit) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(lines == 0U);
    } else if (!strncmp(name, "measure", 7U)) {
        input = (const unsigned char *)"#include <a.h>\nvoid f(void) {}\n";
        input_size = strlen((const char *)input);
        UmiCodeGuardSourceInfo result;
        memset(&result, 0xa5, sizeof result);
        unsigned char prior[sizeof result];
        memcpy(prior, &result, sizeof result);
        const char *path = "sample.c";
        if (!strcmp(name, "measure-alias")) { strcpy(result.path, path); path = result.path; }
        if (!strcmp(name, "measure-failure")) native_result = UMI_STATUS_BUSY;
        UmiStatus status = umi_codeguard_source_measure_bounded(path, input_size, &result, token);
        if (native_result != UMI_STATUS_OK)
            CHECK(status == native_result && !memcmp(prior, &result, sizeof result));
        else {
            CHECK(status == UMI_STATUS_OK && !strcmp(result.path, "sample.c"));
            CHECK(result.bytes == input_size && result.lines == 2U);
            CHECK(result.include_count == 1U && result.function_hint_count == 1U);
        }
        CHECK(observed_limit == input_size && native_path_absolute);
    } else if (!strncmp(name, "hash", 4U) || !strcmp(name, "trailing-slash")) {
        input = (const unsigned char *)" a /* note */ + b // end\n";
        input_size = strlen((const char *)input);
        const char *normalized = "a+b";
        if (!strcmp(name, "hash-binary")) {
            input = (const unsigned char *)"a\0b";
            input_size = 3U;
        }
        if (!strcmp(name, "trailing-slash")) {
            input = (const unsigned char *)"a /";
            input_size = 3U;
            normalized = "a/";
            chunk_size = 1U;
        }
        uint64_t raw = 11U, normal = 22U;
        size_t size = 33U;
        if (!strcmp(name, "hash-failure")) native_result = UMI_STATUS_IO_ERROR;
        if (!strcmp(name, "hash-overflow")) {
            UmiCodeGuardHashVisit candidate = {.raw = 11U, .normalized = 22U, .size = SIZE_MAX};
            CHECK(umi_codeguard_hash_chunk(input, 1U, &candidate) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(candidate.raw == 11U && candidate.normalized == 22U && candidate.size == SIZE_MAX);
        } else {
            UmiStatus status = umi_codeguard_hash_file_bounded("sample.c", input_size,
                &raw, &normal, &size, token);
            if (native_result != UMI_STATUS_OK)
                CHECK(status == native_result && raw == 11U && normal == 22U && size == 33U);
            else {
                CHECK(status == UMI_STATUS_OK && size == input_size);
                CHECK(raw == umi_codeguard_hash_bytes(input, input_size));
                CHECK(normal == (!strcmp(name, "hash-binary") ? raw :
                    umi_codeguard_hash_bytes(normalized, strlen(normalized))));
            }
        }
    } else {
        if (!strcmp(name, "empty") || !strcmp(name, "cancel-empty")) { input_size = 0U; maximum = 0U; }
        if (!strcmp(name, "exact")) maximum = input_size;
        if (!strcmp(name, "over")) maximum = input_size - 1U;
        if (!strcmp(name, "zero-over")) maximum = 0U;
        if (!strcmp(name, "long") || !strcmp(name, "newline-boundary")) {
            memset(long_input, 'x', sizeof long_input);
            input = long_input;
            input_size = 8194U;
            chunk_size = 7U;
            if (!strcmp(name, "newline-boundary")) long_input[8190] = '\n';
        }
        if (!strcmp(name, "crlf")) { input = (const unsigned char *)"a\r\nb\r"; input_size = 5U; chunk_size = 1U; }
        if (!strcmp(name, "nul")) { input = (const unsigned char *)"a\0b"; input_size = 3U; }
        if (!strcmp(name, "late-nul")) { input = (const unsigned char *)"a\nb\0c"; input_size = 5U; }
        if (!strcmp(name, "late-read") || !strcmp(name, "late-close")) native_result = UMI_STATUS_IO_ERROR;
        if (!strcmp(name, "late-change")) native_result = UMI_STATUS_BUSY;
        if (!strcmp(name, "missing")) { native_result = UMI_STATUS_NOT_FOUND; input_size = 0U; }
        if (!strcmp(name, "cancel-before")) umi_cancellation_token_request(token);
        if (!strcmp(name, "cancel-line") || !strcmp(name, "callback-failure")) cancel_in_callback = 1;
        if (!strcmp(name, "callback-failure")) callback_result = UMI_STATUS_OUT_OF_MEMORY;
        if (!strcmp(name, "cancel-empty")) cancel_on_return = 1;
        if (!strcmp(name, "failure-cancel")) { cancel_on_return = 1; native_result = UMI_STATUS_BUSY; }
        UmiStatus status = umi_codeguard_source_visit_lines("sample.c", maximum, Line, NULL, token);
        if (!strcmp(name, "cancel-before")) CHECK(status == UMI_STATUS_CANCELLED && visits == 0U && lines == 0U);
        else if (!strcmp(name, "cancel-line")) CHECK(status == UMI_STATUS_CANCELLED && lines == 1U && bytes == 3U);
        else if (!strcmp(name, "cancel-empty")) CHECK(status == UMI_STATUS_CANCELLED && lines == 0U);
        else if (!strcmp(name, "callback-failure")) CHECK(status == UMI_STATUS_OUT_OF_MEMORY && lines == 1U);
        else if (!strcmp(name, "missing")) CHECK(status == UMI_STATUS_IO_ERROR && lines == 0U);
        else if (maximum < input_size) CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && lines == 0U);
        else if (!strcmp(name, "nul") || !strcmp(name, "late-nul"))
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && lines == (!strcmp(name, "nul") ? 0U : 1U));
        else if (native_result != UMI_STATUS_OK)
            CHECK(status == native_result && lines == 2U && bytes == 6U);
        else {
            CHECK(status == UMI_STATUS_OK && bytes == input_size && !memcmp(input, collected, bytes));
            if (!strcmp(name, "long") || !strcmp(name, "newline-boundary"))
                CHECK(lines == 2U && first_size == 8191U && last_size == 3U);
            else if (!strcmp(name, "crlf")) CHECK(lines == 2U && first_size == 3U && last_size == 2U);
            else CHECK(lines == (input_size == 0U ? 0U : 3U));
        }
        if (visits) CHECK(native_path_absolute && observed_limit == maximum);
    }
    umi_cancellation_token_destroy(token);
    return 0;
}
