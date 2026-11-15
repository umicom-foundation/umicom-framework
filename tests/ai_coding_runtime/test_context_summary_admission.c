/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_context_summary_admission.c
 *
 * PURPOSE:
 *   Verify bounded metadata summaries, compatible projections and failure-preserving output formatting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_coding_runtime/context_summary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); goto cleanup; \
} } while (0)

typedef struct SummaryFixture {
    UmiAiCodingMaterializedContext context;
    char output[65536], before[65536];
} SummaryFixture;
static void seed(SummaryFixture *f, size_t count)
{
    memset(f, 0, sizeof *f);
    f->context.file_count = count; f->context.estimated_tokens = count == 0U ? 0U : 12U;
    for (size_t i = 0U; i < count; ++i) {
        UmiAiCodingMaterializedFile *file = &f->context.files[i];
        (void)snprintf(file->path, sizeof file->path, "src/file%zu.c", i);
        strcpy(file->language_id, "c"); file->length = 10U; file->hash = 42U; file->active = i == 0U;
    }
    memset(f->output, 0x5a, sizeof f->output); memcpy(f->before, f->output, sizeof f->output);
}

static int invalid_case(const char *mode)
{
    SummaryFixture *f = calloc(1U, sizeof *f);
    const UmiAiCodingMaterializedContext *context;
    UmiStatus expected = UMI_STATUS_INVALID_STATE;
    int result = 1;
    REQUIRE(f != NULL); seed(f, 2U); context = &f->context;
    UmiAiCodingMaterializedFile *file = &f->context.files[1];
    if (strcmp(mode, "null") == 0) { context = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "count") == 0) f->context.file_count = SIZE_MAX;
    else if (strcmp(mode, "path") == 0) memset(file->path, 'x', sizeof file->path);
    else if (strcmp(mode, "empty-path") == 0) file->path[0] = '\0';
    else if (strcmp(mode, "unsafe-path") == 0) strcpy(file->path, "../outside.c");
    else if (strcmp(mode, "language") == 0) memset(file->language_id, 'x', sizeof file->language_id);
    else if (strcmp(mode, "language-newline") == 0) strcpy(file->language_id, "c\nforged row");
    else if (strcmp(mode, "path-del") == 0) strcpy(file->path, "src/\x7f.c");
    else if (strcmp(mode, "path-utf8") == 0) { strcpy(file->path, "src/\xff.c"); expected = UMI_STATUS_PARSE_ERROR; }
    else if (strcmp(mode, "language-utf8") == 0) { strcpy(file->language_id, "c\xff"); expected = UMI_STATUS_PARSE_ERROR; }
    else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_context_summary(context, f->output, sizeof f->output) == expected);
    REQUIRE(memcmp(f->output, f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int capacity_case(const char *mode)
{
    SummaryFixture *f = calloc(1U, sizeof *f);
    char expected[2048];
    size_t length, capacity;
    UmiStatus status = UMI_STATUS_CAPACITY_EXCEEDED;
    int result = 1;
    REQUIRE(f != NULL); seed(f, 1U);
    REQUIRE(umi_ai_coding_context_summary(&f->context, expected, sizeof expected) == UMI_STATUS_OK);
    length = strlen(expected); capacity = length;
    if (strcmp(mode, "exact") == 0) { capacity = length + 1U; status = UMI_STATUS_OK; }
    else if (strcmp(mode, "short") == 0) { }
    else if (strcmp(mode, "one") == 0) capacity = 1U;
    else if (strcmp(mode, "zero") == 0) { capacity = 0U; status = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "null-output") == 0) {
        REQUIRE(umi_ai_coding_context_summary(&f->context, NULL, sizeof f->output) == UMI_STATUS_INVALID_ARGUMENT);
        result = 0; goto cleanup;
    } else if (strcmp(mode, "alias") == 0) {
        REQUIRE(umi_ai_coding_context_summary(&f->context, f->context.files[0].path, sizeof f->context.files[0].path) == UMI_STATUS_OK);
        REQUIRE(strcmp(f->context.files[0].path, expected) == 0);
        result = 0; goto cleanup;
    } else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_context_summary(&f->context, f->output, capacity) == status);
    if (status == UMI_STATUS_OK) {
        REQUIRE(strcmp(f->output, expected) == 0);
        REQUIRE((unsigned char)f->output[length + 1U] == 0x5aU);
    } else REQUIRE(memcmp(f->output, f->before, sizeof f->output) == 0);
    result = 0;
cleanup:
    free(f); return result;
}

static int success_case(const char *mode)
{
    SummaryFixture *f = calloc(1U, sizeof *f);
    int result = 1;
    REQUIRE(f != NULL); seed(f, 1U);
    if (strcmp(mode, "metadata-only") == 0) {
        /* Summary deliberately does not read or certify source text. */
        memset(f->context.files[0].text, 0xff, sizeof f->context.files[0].text);
    } else if (strcmp(mode, "empty") == 0) seed(f, 0U);
    else if (strcmp(mode, "empty-language") == 0) f->context.files[0].language_id[0] = '\0';
    else if (strcmp(mode, "unicode") == 0) {
        strcpy(f->context.files[0].path, "src/\xe2\x82\xac.c");
        strcpy(f->context.files[0].language_id, "c \xc2\xa2");
    } else if (strcmp(mode, "flags") == 0) { f->context.truncated = -1; f->context.files[0].active = -2; }
    else if (strcmp(mode, "numeric-maxima") == 0) {
        f->context.skipped_count = SIZE_MAX; f->context.estimated_tokens = UINT32_MAX;
        f->context.files[0].length = SIZE_MAX; f->context.files[0].hash = UINT64_MAX;
    } else if (strcmp(mode, "maximum-labels") == 0) {
        seed(f, UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY);
        for (size_t i = 0U; i < f->context.file_count; ++i) {
            UmiAiCodingMaterializedFile *file = &f->context.files[i];
            memset(file->path, 'a', sizeof file->path - 1U); file->path[sizeof file->path - 1U] = '\0';
            memset(file->language_id, 'b', sizeof file->language_id - 1U); file->language_id[sizeof file->language_id - 1U] = '\0';
        }
    } else { result = 2; goto cleanup; }
    REQUIRE(umi_ai_coding_context_summary(&f->context, f->output, sizeof f->output) == UMI_STATUS_OK);
    REQUIRE(strncmp(f->output, "Context files: ", 15U) == 0);
    if (strcmp(mode, "metadata-only") == 0)
        REQUIRE(strcmp(f->output, "Context files: 1, skipped: 0, estimated tokens: 12\n* src/file0.c [c] 10 bytes hash=000000000000002a\n") == 0);
    if (strcmp(mode, "flags") == 0) REQUIRE(strstr(f->output, ", truncated\n* ") != NULL);
    if (strcmp(mode, "numeric-maxima") == 0) REQUIRE(strstr(f->output, "hash=ffffffffffffffff") != NULL);
    if (strcmp(mode, "maximum-labels") == 0) {
        size_t lines = 0U;
        for (const char *p = f->output; *p != '\0'; ++p) if (*p == '\n') ++lines;
        REQUIRE(lines == UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY + 1U);
    }
    result = 0;
cleanup:
    free(f); return result;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "invalid") == 0) return invalid_case(argv[2]);
    if (strcmp(argv[1], "capacity") == 0) return capacity_case(argv[2]);
    if (strcmp(argv[1], "success") == 0) return success_case(argv[2]);
    return 2;
}
