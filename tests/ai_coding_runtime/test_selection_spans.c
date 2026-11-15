/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_selection_spans.c
 *
 * PURPOSE:
 *   Verify bounded line spans, defined character-buffer overlap and preserved selection results on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/selection.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)

typedef struct RangeCase {
    const char *name, *source, *expected;
    uint32_t first, last;
    UmiStatus status;
} RangeCase;
static int range_case(const char *name)
{
    static const RangeCase cases[] = {
        {"first", "one\ntwo\nthree", "one\n", 1U, 1U, UMI_STATUS_OK},
        {"middle", "one\ntwo\nthree", "two\n", 2U, 2U, UMI_STATUS_OK},
        {"multiple", "one\ntwo\nthree\nfour\n", "two\nthree\n", 2U, 3U, UMI_STATUS_OK},
        {"to-eof", "one\ntwo\nthree", "two\nthree", 2U, 50U, UMI_STATUS_OK},
        {"beyond-eof", "one\ntwo", NULL, 3U, 3U, UMI_STATUS_NOT_FOUND},
        {"empty", "", "", 1U, 1U, UMI_STATUS_OK},
        {"empty-later", "", NULL, 2U, 2U, UMI_STATUS_NOT_FOUND},
        {"trailing-empty", "one\n", "", 2U, 2U, UMI_STATUS_OK},
        {"crlf", "one\r\ntwo\r\nthree", "two\r\n", 2U, 2U, UMI_STATUS_OK},
        {"cr-only", "one\rtwo", "one\rtwo", 1U, 1U, UMI_STATUS_OK},
        {"maximum-end", "one\ntwo\n", "one\ntwo\n", 1U, UINT32_MAX, UMI_STATUS_OK},
        {"unicode", "one\n\xe2\x82\xac\nthree", "\xe2\x82\xac\n", 2U, 2U, UMI_STATUS_OK}
    };
    char output[128], before[128];
    if (strcmp(name, "unterminated-span") == 0) {
        const char bytes[] = {'o','n','e','\n','t','w','o'};
        size_t length = 77U;
        CHECK(UmiAiCodingExtractSelectionSpan(bytes, sizeof bytes, 2U, 2U, output, sizeof output, &length) == UMI_STATUS_OK);
        CHECK(length == 3U && strcmp(output, "two") == 0); return 0;
    }
    if (strcmp(name, "null-empty-span") == 0) {
        size_t length = 77U;
        CHECK(UmiAiCodingExtractSelectionSpan(NULL, 0U, 1U, 1U, output, sizeof output, &length) == UMI_STATUS_OK);
        CHECK(length == 0U && output[0] == '\0'); return 0;
    }
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
        const RangeCase *c = &cases[i];
        if (strcmp(c->name, name) != 0) continue;
        for (int span = 0; span < 2; ++span) {
            size_t length = 77U;
            memset(output, 0x5a, sizeof output); memcpy(before, output, sizeof before);
            UmiStatus status = span ? UmiAiCodingExtractSelectionSpan(c->source, strlen(c->source), c->first, c->last, output, sizeof output, &length) :
                umi_ai_coding_extract_selection(c->source, c->first, c->last, output, sizeof output, &length);
            CHECK(status == c->status);
            if (status == UMI_STATUS_OK) CHECK(strcmp(output, c->expected) == 0 && length == strlen(c->expected));
            else CHECK(length == 77U && memcmp(output, before, sizeof output) == 0);
        }
        return 0;
    }
    return 2;
}

static int invalid_case(const char *mode)
{
    char bytes[] = {'o','n','e','\0','x'};
    const char *source = "one\ntwo";
    size_t source_length = strlen(source), length = 77U, capacity = 128U;
    size_t *reported = &length;
    char output[128], before[128], *destination = output;
    uint32_t first = 1U, last = 1U;
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    int legacy = 0;
    memset(output, 0x5a, sizeof output); memcpy(before, output, sizeof before);
    if (strcmp(mode, "null-span") == 0) source = NULL;
    else if (strcmp(mode, "size-max") == 0) { source_length = SIZE_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "embedded-nul") == 0) { source = bytes; source_length = sizeof bytes; expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "zero-line") == 0) first = 0U;
    else if (strcmp(mode, "reverse") == 0) { first = 2U; last = 1U; }
    else if (strcmp(mode, "zero-capacity") == 0) capacity = 0U;
    else if (strcmp(mode, "null-output") == 0) destination = NULL;
    else if (strcmp(mode, "null-length") == 0) reported = NULL;
    else if (strcmp(mode, "legacy-null") == 0) { source = NULL; legacy = 1; }
    else return 2;
    UmiStatus status = legacy ? umi_ai_coding_extract_selection(source, first, last, destination, capacity, reported) :
        UmiAiCodingExtractSelectionSpan(source, source_length, first, last, destination, capacity, reported);
    CHECK(status == expected && length == 77U && memcmp(output, before, sizeof output) == 0);
    return 0;
}

static int alias_case(const char *api, const char *direction)
{
    char storage[128];
    char *source = storage + 32U, *destination = source;
    size_t length = 77U;
    memset(storage, 0x5a, sizeof storage); strcpy(source, "one\ntwo\nthree\n");
    if (strcmp(direction, "forward") == 0) destination += 2U;
    else if (strcmp(direction, "backward") == 0) destination -= 3U;
    else if (strcmp(direction, "same") != 0) return 2;
    UmiStatus status;
    if (strcmp(api, "span") == 0) status = UmiAiCodingExtractSelectionSpan(source, strlen(source), 1U, 2U, destination, 32U, &length);
    else if (strcmp(api, "legacy") == 0) status = umi_ai_coding_extract_selection(source, 1U, 2U, destination, 32U, &length);
    else return 2;
    CHECK(status == UMI_STATUS_OK && length == 8U && strcmp(destination, "one\ntwo\n") == 0);
    return 0;
}

static int capacity_case(const char *mode)
{
    char output[128], before[128];
    size_t capacity;
    UmiStatus expected;
    if (strcmp(mode, "exact") == 0) { capacity = 5U; expected = UMI_STATUS_OK; }
    else if (strcmp(mode, "short") == 0) { capacity = 4U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(mode, "one") == 0) { capacity = 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else return 2;
    for (int span = 0; span < 2; ++span) {
        size_t length = 77U;
        memset(output, 0x5a, sizeof output); memcpy(before, output, sizeof before);
        UmiStatus status = span ? UmiAiCodingExtractSelectionSpan("one\ntwo", 7U, 1U, 1U, output, capacity, &length) :
            umi_ai_coding_extract_selection("one\ntwo", 1U, 1U, output, capacity, &length);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK) CHECK(length == 4U && strcmp(output, "one\n") == 0 && (unsigned char)output[5] == 0x5aU);
        else CHECK(length == 77U && memcmp(output, before, sizeof output) == 0);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 3 && strcmp(argv[1], "range") == 0) return range_case(argv[2]);
    if (argc == 3 && strcmp(argv[1], "invalid") == 0) return invalid_case(argv[2]);
    if (argc == 4 && strcmp(argv[1], "alias") == 0) return alias_case(argv[2], argv[3]);
    if (argc == 3 && strcmp(argv[1], "capacity") == 0) return capacity_case(argv[2]);
    return 2;
}
