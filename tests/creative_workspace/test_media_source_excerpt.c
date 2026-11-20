/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_source_excerpt.c
 *
 * PURPOSE:
 *   Verify explicit source lines, exact original bytes and unchanged results after refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/media_source_excerpt.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/native_arguments.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

typedef struct ExcerptCase {
    const char *name;
    const char *source;
    size_t source_size;
    size_t first;
    size_t last;
    size_t offset;
    size_t selected;
    UmiStatus status;
} ExcerptCase;

#define FIXTURE(name, bytes, first, last, offset, selected, status) \
    {name, bytes, sizeof(bytes) - 1U, first, last, offset, selected, status}

/* Literal bytes make encoding and original offsets independent from the API. */
static const ExcerptCase cases[] = {
    FIXTURE("lf", "first\nsecond\nthird", 2U, 3U, 6U, 12U, UMI_STATUS_OK),
    FIXTURE("mixed", "ignore\r\none\rtwo\r\nthree\nend", 2U, 4U, 8U, 14U, UMI_STATUS_OK),
    FIXTURE("bom", "\xEF\xBB\xBF" "caf\xC3\xA9\nnext", 1U, 1U, 3U, 5U, UMI_STATUS_OK),
    FIXTURE("blank", "one\r\n\r\nthree", 2U, 2U, 5U, 0U, UMI_STATUS_OK),
    FIXTURE("empty", "", 1U, 1U, 0U, 0U, UMI_STATUS_NOT_FOUND),
    FIXTURE("bom-only", "\xEF\xBB\xBF", 1U, 1U, 0U, 0U, UMI_STATUS_NOT_FOUND),
    FIXTURE("no-extra-line", "one\n", 2U, 2U, 0U, 0U, UMI_STATUS_NOT_FOUND),
    FIXTURE("missing-range", "one\r\ntwo", 2U, 3U, 0U, 0U, UMI_STATUS_NOT_FOUND),
    FIXTURE("malformed-tail", "one\n\xF0\x80\x80\xAF", 1U, 1U, 0U, 0U, UMI_STATUS_PARSE_ERROR),
    FIXTURE("embedded-nul", "one\n\x00two", 1U, 1U, 0U, 0U, UMI_STATUS_PARSE_ERROR),
    FIXTURE("control", "one\n\x7F" "two", 2U, 2U, 0U, 0U, UMI_STATUS_PARSE_ERROR),
    {"exact-selection", NULL, 2048U, 1U, 1U, 0U, 2048U, UMI_STATUS_OK},
    {"large-selection", NULL, 2049U, 1U, 1U, 0U, 0U, UMI_STATUS_CAPACITY_EXCEEDED},
    {"exact-file", NULL, UMI_MEDIA_SOURCE_FILE_MAX_BYTES, 1U, 1U, 0U, 1U, UMI_STATUS_OK},
    {"large-file", NULL, UMI_MEDIA_SOURCE_FILE_MAX_BYTES + 1U, 1U, 1U, 0U, 0U, UMI_STATUS_CAPACITY_EXCEEDED},
    {"missing-file", NULL, 0U, 1U, 1U, 0U, 0U, UMI_STATUS_NOT_FOUND},
    FIXTURE("relative", "one", 1U, 1U, 0U, 0U, UMI_STATUS_INVALID_ARGUMENT),
    FIXTURE("zero-line", "one", 0U, 1U, 0U, 0U, UMI_STATUS_INVALID_ARGUMENT),
    FIXTURE("reversed", "one", 2U, 1U, 0U, 0U, UMI_STATUS_INVALID_ARGUMENT),
    FIXTURE("cancel", "one", 1U, 1U, 0U, 0U, UMI_STATUS_CANCELLED),
    FIXTURE("null-output", "one", 1U, 1U, 0U, 0U, UMI_STATUS_INVALID_ARGUMENT),
};

static int WriteSource(const char *path, const char *bytes, size_t size)
{
    /* Every regression receives a separate absolute file in its build area. */
    FILE *file = fopen(path, "wb");
    if (file == NULL) return 0;
    size_t written = fwrite(bytes, 1U, size, file);
    return fclose(file) == 0 && written == size;
}

static int Run(const ExcerptCase *test, const char *path)
{
    const char *bytes = test->source;
    char *generated = NULL;
    if (bytes == NULL && test->source_size > 0U) {
        /* Large input cases otherwise exercise the same physical read path. */
        generated = malloc(test->source_size);
        CHECK(generated != NULL);
        memset(generated, 'x', test->source_size);
        if (test->source_size >= UMI_MEDIA_SOURCE_FILE_MAX_BYTES) generated[1] = '\n';
        bytes = generated;
    }
    if (bytes != NULL) CHECK(WriteSource(path, bytes, test->source_size));

    /* Check cancellation before reading and use a sentinel to detect partial
     * publication on every failure, including failures caused by other lines. */
    UmiCancellationToken *token = NULL;
    if (strcmp(test->name, "cancel") == 0) {
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
    }
    UmiMediaSourceExcerpt result;
    UmiMediaSourceExcerpt original;
    memset(&result, 0xA5, sizeof(result));
    memcpy(&original, &result, sizeof(original));
    const char *selected_path = strcmp(test->name, "relative") == 0 ? "relative-source.txt" : path;
    UmiMediaSourceExcerpt *output = strcmp(test->name, "null-output") == 0 ? NULL : &result;
    UmiStatus status = UmiMediaSourceExcerptReadUtf8(selected_path, test->first, test->last, token, output);
    umi_cancellation_token_destroy(token);
    CHECK(status == test->status);
    if (status == UMI_STATUS_OK) {
        CHECK(result.first_line == test->first && result.last_line == test->last);
        CHECK(result.byte_offset == test->offset && result.byte_count == test->selected);
        CHECK(result.text[test->selected] == '\0');
        CHECK(memcmp(result.text, bytes + test->offset, test->selected) == 0);
    } else {
        CHECK(memcmp(&result, &original, sizeof(result)) == 0);
    }

    /* Reopen every physical source and independently inspect the stored bytes.
     * Missing source input must remain missing. */
    unsigned char *readback = NULL;
    size_t readback_size = 0U;
    status = UmiInputFileRead(path, test->source_size, &readback, &readback_size);
    if (bytes != NULL) {
        CHECK(status == UMI_STATUS_OK && readback_size == test->source_size);
        CHECK(memcmp(readback, bytes, readback_size) == 0);
    } else {
        CHECK(status == UMI_STATUS_NOT_FOUND && readback == NULL && readback_size == 0U);
    }
    UmiInputFileFree(readback);
    free(generated);
    return 0;
}

/* Verify the hash against independently known bytes, then repeat the same
 * selected range after modifying only unselected text. */
static int Inspect(const char *name, const char *path)
{
    static const char sample[] = "\xEF\xBB\xBF" "first\r\ncaf\xC3\xA9\nLast\r";
    static const char digest[] = "83a93656f54c8e1dc144c3020768c11dbb853940c904d9c2a6d5a4418f4a0051";
    const char *expected = strcmp(name, "inspect-identity") == 0 ? NULL : digest;
    size_t last = 3U;
    UmiStatus wanted = UMI_STATUS_OK;
    UmiCancellationToken *cancel = NULL;
    CHECK(WriteSource(path, sample, sizeof(sample) - 1U));
    if (strcmp(name, "inspect-changed-source") == 0) {
        /* The requested range stays byte-identical; only the first line differs. */
        static const char changed[] = "\xEF\xBB\xBF" "First\r\ncaf\xC3\xA9\nLast\r";
        CHECK(WriteSource(path, changed, sizeof(changed) - 1U));
        wanted = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(name, "inspect-bad-digest") == 0) {
        expected = "83a93";
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(name, "inspect-cancelled") == 0) {
        CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    } else if (strcmp(name, "inspect-outside-lines") == 0) {
        last = 4U;
        wanted = UMI_STATUS_NOT_FOUND;
    } else {
        CHECK(strcmp(name, "inspect-identity") == 0 || strcmp(name, "inspect-repeat") == 0);
    }
    UmiMediaSourceExcerptObservation result;
    UmiMediaSourceExcerptObservation before;
    memset(&result, 0xA5, sizeof(result));
    memcpy(&before, &result, sizeof(before));
    const UmiStatus status = UmiMediaSourceExcerptInspectUtf8(path, 2U, last, expected, cancel, &result);
    umi_cancellation_token_destroy(cancel);
    CHECK(status == wanted);
    if (status == UMI_STATUS_OK) {
        CHECK(strcmp(result.source_sha256, digest) == 0 && result.file_bytes == sizeof(sample) - 1U);
        CHECK(result.excerpt.first_line == 2U && result.excerpt.last_line == 3U);
        CHECK(result.excerpt.byte_offset == 10U && result.excerpt.byte_count == 10U);
        CHECK(strcmp(result.excerpt.text, "caf\xC3\xA9\nLast") == 0);
    } else {
        CHECK(memcmp(&result, &before, sizeof(result)) == 0);
    }
    return 0;
}

/* Native output creates binary fixture bytes with a UTF-8 path. CMake's
 * file(WRITE) translates line endings on Windows and cannot seed this case. */
static int SeedCommandSource(const char *path, const char *version)
{
    static const char original[] = "head\ncaf\xC3\xA9\r\nlast\r";
    static const char revised[] = "HEAD\ncaf\xC3\xA9\r\nlast\r";
    CHECK(strcmp(version, "original") == 0 || strcmp(version, "revised") == 0);
    const char *bytes = strcmp(version, "original") == 0 ? original : revised;
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    UmiStatus status = UmiOutputFileWrite(file, bytes, strlen(bytes));
    if (status == UMI_STATUS_OK) status = UmiOutputFileClose(file);
    UmiOutputFileDestroy(file);
    CHECK(status == UMI_STATUS_OK);
    return 0;
}

/* The fixture now uses the same native UTF-8 argument owner as Media.
 * Retain the original entry-point signature for engineering review. */
#if 0
int main(int argc, char **argv)
#endif
static int RunFixture(int argc, char **argv)
{
    /* A subprocess must run the real command and write to native handles
     * so CTest can compare output bytes independently. */
    if (argc >= 2 && strcmp(argv[1], "command") == 0)
        return UmiMediaSourceExcerptCommand(argc - 2, argv + 2);
    if (argc == 4 && strcmp(argv[1], "seed-command-source") == 0)
        return SeedCommandSource(argv[2], argv[3]);

    /* CTest selects one named case so a failure identifies its exact contract. */
    CHECK(argc == 3);
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        if (strcmp(cases[i].name, argv[1]) == 0) return Run(&cases[i], argv[2]);
    }
    /* Identity cases exercise the same reader and keep separate files. */
    if (strncmp(argv[1], "inspect-", 8U) == 0)
        return Inspect(argv[1], argv[2]);
    fprintf(stderr, "Unknown excerpt case: %s\n", argv[1]);
    return 1;
}

int main(int argc, char **argv)
{
    /* Preserve a single entry point for direct API and CLI contract cases. */
    int exit_code = 1;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, RunFixture, &exit_code);
    return status == UMI_STATUS_OK ? exit_code : 1;
}
