/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/benchmark_suite.c
 *
 * PURPOSE:
 *   Parse a reviewed, bounded, non-executable benchmark text format.
 *   Isolated from C23 model mathematics and the Umicom LLM application.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/benchmark_suite.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUITE_HEADER "UMICOM-AI-BENCHMARK-1"

typedef struct BenchmarkEntry {
    char title[UMI_AI_BENCHMARK_MAX_TITLE_BYTES + 1U];
    uint8_t prompt[UMI_AI_GENERATION_MAX_PROMPT_TOKENS];
    uint8_t expected[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t promptCount;
    size_t expectedCount;
} BenchmarkEntry;

struct UmiAiTrainingBenchmarkSuite {
    size_t count;
    BenchmarkEntry items[UMI_AI_BENCHMARK_MAX_CASES];
};

static bool NameValid(const char *name)
{
    size_t length = 0U;
    if (name == NULL || name[0] == '\0') return false;
    for (const char *p = name; *p != '\0'; ++p) {
        const char c = *p;
        if (!((c >= '0' && c <= '9') ||
              (c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              c == '-' || c == '_' || c == '.')) return false;
        ++length;
        if (length > UMI_AI_BENCHMARK_MAX_TITLE_BYTES) return false;
    }
    return true;
}

static int HexDigit(char digit)
{
    if (digit >= '0' && digit <= '9') return digit - '0';
    if (digit >= 'a' && digit <= 'f') return digit - 'a' + 10;
    if (digit >= 'A' && digit <= 'F') return digit - 'A' + 10;
    return -1;
}

static bool DecodeHex(const char *text, uint8_t *dest,
                      size_t maxBytes, size_t *outCount, bool allowEmpty)
{
    size_t n = 0U;
    if (text == NULL || dest == NULL || outCount == NULL) return false;
    if (allowEmpty && strcmp(text, "-") == 0) {
        *outCount = 0U;
        return true;
    }
    size_t digits = strlen(text);
    if (digits == 0U || (digits & 1U) != 0U || digits / 2U > maxBytes)
        return false;
    for (size_t index = 0U; index < digits; index += 2U) {
        const int left = HexDigit(text[index]);
        const int right = HexDigit(text[index + 1U]);
        if (left < 0 || right < 0) return false;
        dest[n++] = (uint8_t)((unsigned)(left * 16 + right));
    }
    *outCount = n;
    return true;
}

/* Bounded local tokeniser: only SP/HTAB separate fields. Reject malformed
 * or surplus columns. The function operates only on already copied input. */
static char *TakeField(char **cursor)
{
    char *begin;
    char *p = *cursor;
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '\0') { *cursor = p; return NULL; }
    begin = p;
    while (*p != '\0' && *p != ' ' && *p != '\t') ++p;
    if (*p != '\0') {
        *p = '\0'; ++p;
    }
    *cursor = p;
    return begin;
}

UmiStatus UmiAiTrainingBenchmarkSuiteParse(
    const char *text, size_t textLength,
    UmiAiTrainingBenchmarkSuite **outSuite)
{
    UmiAiTrainingBenchmarkSuite *suite = NULL;
    char *buffer = NULL;
    size_t cursor = 0U;
    bool seenHeader = false;
    UmiStatus status = UMI_STATUS_PARSE_ERROR;
    if (text == NULL || outSuite == NULL || textLength == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (textLength > UMI_AI_BENCHMARK_MAX_SUITE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', textLength) != NULL) return UMI_STATUS_PARSE_ERROR;
    buffer = malloc(textLength + 1U);
    suite = calloc(1U, sizeof(*suite));
    if (buffer == NULL || suite == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    memcpy(buffer, text, textLength);
    buffer[textLength] = '\0';
    while (cursor < textLength) {
        size_t start = cursor;
        size_t count;
        char *row, *fields[5] = {0};
        while (cursor < textLength && buffer[cursor] != '\n') ++cursor;
        count = cursor - start;
        if (cursor < textLength) ++cursor;
        if (count > 0U && buffer[start + count - 1U] == '\r') --count;
        buffer[start + count] = '\0';
        row = buffer + start;
        if (count == 0U || row[0] == '#') continue;
        if (!seenHeader) {
            if (strcmp(row, SUITE_HEADER) != 0) goto done;
            seenHeader = true;
            continue;
        }
        for (size_t i = 0U; i < count; ++i) {
            const unsigned char c = (unsigned char)row[i];
            if (c > 127U || c == '\r' || (c < 32U && c != '\t')) goto done;
        }
        char *next = row;
        for (size_t i = 0U; i < 5U; ++i) fields[i] = TakeField(&next);
        if (fields[0] == NULL || strcmp(fields[0], "case") != 0 ||
            fields[1] == NULL || fields[2] == NULL || fields[3] == NULL ||
            fields[4] != NULL || !NameValid(fields[1])) goto done;
        if (suite->count == UMI_AI_BENCHMARK_MAX_CASES) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto done;
        }
        for (size_t i = 0U; i < suite->count; ++i)
            if (strcmp(fields[1], suite->items[i].title) == 0) goto done;
        BenchmarkEntry *candidate = &suite->items[suite->count];
        if (!DecodeHex(fields[2], candidate->prompt, sizeof(candidate->prompt),
                       &candidate->promptCount, true) ||
            !DecodeHex(fields[3], candidate->expected, sizeof(candidate->expected),
                       &candidate->expectedCount, false)) goto done;
        memcpy(candidate->title, fields[1], strlen(fields[1]) + 1U);
        ++suite->count;
    }
    if (!seenHeader || suite->count == 0U) goto done;
    *outSuite = suite;
    suite = NULL;
    status = UMI_STATUS_OK;
done:
    free(buffer);
    free(suite);
    return status;
}

UmiStatus UmiAiTrainingBenchmarkSuiteLoadFile(
    const char *filePath, UmiAiTrainingBenchmarkSuite **outSuite)
{
    FILE *file;
    char *buffer = NULL;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    long length;
    if (filePath == NULL || filePath[0] == '\0' || outSuite == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(filePath, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    if (fseek(file, 0L, SEEK_END) != 0 || (length = ftell(file)) < 0L) goto done;
    if ((unsigned long)length > UMI_AI_BENCHMARK_MAX_SUITE_BYTES) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    if (fseek(file, 0L, SEEK_SET) != 0) goto done;
    buffer = malloc((size_t)length + 1U);
    if (buffer == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    if (fread(buffer, 1U, (size_t)length, file) != (size_t)length ||
        fgetc(file) != EOF || ferror(file) != 0) goto done;
    /* Finish the read transaction BEFORE publishing a suite. A close failure
     * cannot leave the caller holding output from a rejected load. */
    if (fclose(file) != 0) {
        file = NULL;
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    file = NULL;
    status = UmiAiTrainingBenchmarkSuiteParse(buffer, (size_t)length, outSuite);
done:
    free(buffer);
    if (file != NULL) (void)fclose(file);
    return status;
}

UmiStatus UmiAiTrainingBenchmarkSuiteCase(
    const UmiAiTrainingBenchmarkSuite *suite, size_t index,
    UmiAiTrainingBenchmarkCase *outCase)
{
    const BenchmarkEntry *entry;
    UmiAiTrainingBenchmarkCase result;
    if (suite == NULL || outCase == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= suite->count) return UMI_STATUS_INVALID_ARGUMENT;
    entry = &suite->items[index];
    result.title = entry->title;
    result.promptBytes = entry->promptCount == 0U ? NULL : entry->prompt;
    result.promptLength = entry->promptCount;
    result.expectedBytes = entry->expected;
    result.expectedLength = entry->expectedCount;
    *outCase = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingBenchmarkSuiteCount(
    const UmiAiTrainingBenchmarkSuite *suite, size_t *outCount)
{
    if (suite == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCount = suite->count;
    return UMI_STATUS_OK;
}

void UmiAiTrainingBenchmarkSuiteDestroy(UmiAiTrainingBenchmarkSuite *suite)
{
    free(suite);
}
