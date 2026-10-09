/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_stream.c
 * PURPOSE: Check diagnostics across arbitrary process chunks and damaged records.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/parser.h"
#include "umicom/diagnostics/compiler_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

typedef struct Capture
{
    UmiBuildDiagnosticList list;
    size_t malformed;
} Capture;

/* Reuse the public build projection so these cases also protect the Problems
 * representation. The test does not substitute a second compiler grammar. */
static void Observe(UmiStatus status, const UmiCompilerDiagnosticFields *fields, void *context)
{
    Capture *capture = context;
    UmiBuildDiagnostic diagnostic;
    if (status == UMI_STATUS_INVALID_ARGUMENT)
        ++capture->malformed;
    if (status == UMI_STATUS_OK)
        status = UmiBuildDiagnosticFromCompilerFields(fields, &diagnostic);
    if (status == UMI_STATUS_OK)
        (void)umi_build_diagnostic_list_add(&capture->list, &diagnostic);
    else
        ++capture->list.dropped;
}

static int Same(const UmiBuildDiagnosticList *a, const UmiBuildDiagnosticList *b)
{
    if (a->count != b->count || a->dropped != b->dropped)
        return 0;
    /* Compare meaningful fields; struct padding is not diagnostic evidence. */
    for (size_t i = 0U; i < a->count; ++i)
    {
        const UmiBuildDiagnostic *x = &a->items[i], *y = &b->items[i];
        if (x->severity != y->severity || x->line != y->line || x->column != y->column ||
            strcmp(x->file, y->file) || strcmp(x->message, y->message) || strcmp(x->code, y->code))
            return 0;
    }
    return 1;
}

static int Fragmented(const char *mode)
{
    const char transcript[] =
        "[1/8] Compiling\n"
        "\033[31mC:/project/caf\xc3\xa9.c:12:4: error: first failure\033[0m\r\n"
        "CMake Warning at CMakeLists.txt:8 (message):\n"
        "  First explanation.\n\n  Another paragraph.\n"
        "Call Stack (most recent call first):\n"
        "  modules/notes.cmake:2 (include)\n"
        "  CMake Error at modules/child.cmake:5 (message):\n"
        "    nested explanation\n"
        "source.cpp(9,2): warning C4100: unused parameter\r\n"
        "last.c:3: note: no final newline";
    Capture *capture = calloc(1U, sizeof *capture);
    UmiBuildDiagnosticList *expected = calloc(1U, sizeof *expected);
    CHECK(capture != NULL && expected != NULL);
    CHECK(umi_build_parse_output(transcript, expected) == UMI_STATUS_OK);
    CHECK(expected->count == 5U && expected->dropped == 0U);
    const size_t length = sizeof transcript - 1U;
    /* Every possible two-part boundary includes UTF-8, escape and CRLF splits.
     * A separate byte-at-a-time pass exercises many boundaries in one record. */
    for (size_t boundary = 0U; boundary <= length; ++boundary)
    {
        UmiCompilerDiagnosticStream *stream = NULL;
        memset(capture, 0, sizeof *capture);
        CHECK(UmiCompilerDiagnosticStreamCreate(Observe, capture, &stream) == UMI_STATUS_OK);
        if (strcmp(mode, "bytes") == 0)
        {
            for (size_t i = 0U; i < length; ++i)
                CHECK(UmiCompilerDiagnosticStreamFeed(stream, transcript + i, 1U) == UMI_STATUS_OK);
        }
        else
        {
            CHECK(UmiCompilerDiagnosticStreamFeed(stream, transcript, boundary) == UMI_STATUS_OK);
            CHECK(UmiCompilerDiagnosticStreamFeed(stream, transcript + boundary,
                                                  length - boundary) == UMI_STATUS_OK);
        }
        CHECK(UmiCompilerDiagnosticStreamFinish(stream) == UMI_STATUS_OK);
        CHECK(Same(&capture->list, expected));
        CHECK(UmiCompilerDiagnosticStreamFinish(stream) == UMI_STATUS_OK);
        CHECK(Same(&capture->list, expected));
        CHECK(UmiCompilerDiagnosticStreamFeed(stream, NULL, 0U) == UMI_STATUS_INVALID_STATE);
        UmiCompilerDiagnosticStreamDestroy(stream);
        if (strcmp(mode, "bytes") == 0)
            break;
    }
    free(expected);
    free(capture);
    return 0;
}

static int Damaged(const char *mode)
{
    Capture *capture = calloc(1U, sizeof *capture);
    UmiCompilerDiagnosticStream *stream = NULL;
    CHECK(capture != NULL);
    CHECK(UmiCompilerDiagnosticStreamCreate(Observe, capture, &stream) == UMI_STATUS_OK);
    if (strcmp(mode, "nul") == 0)
    {
        const char damaged[] = "bad.c:1: error: hidden\0suffix.c:2: error: forged\n";
        CHECK(UmiCompilerDiagnosticStreamFeed(stream, damaged, sizeof damaged - 1U) ==
              UMI_STATUS_OK);
    }
    else if (strcmp(mode, "record-bound") == 0)
    {
        const char *header = "CMake Error at CMakeLists.txt:4 (message):\n";
        CHECK(UmiCompilerDiagnosticStreamFeed(stream, header, strlen(header)) == UMI_STATUS_OK);
        /* Blank paragraphs consume bounded raw storage without causing an
         * unbounded allocation. Only the lost logical record is reported. */
        for (size_t i = 0U; i < 40000U; ++i)
            CHECK(UmiCompilerDiagnosticStreamFeed(stream, "\n", 1U) == UMI_STATUS_OK);
        CHECK(UmiCompilerDiagnosticStreamFeed(stream, "  continuation\n", 15U) == UMI_STATUS_OK);
    }
    else
    {
        int continuation = strcmp(mode, "long-continuation") == 0;
        if (continuation)
        {
            const char *header = "CMake Error at CMakeLists.txt:4 (message):\n";
            CHECK(UmiCompilerDiagnosticStreamFeed(stream, header, strlen(header)) == UMI_STATUS_OK);
            CHECK(UmiCompilerDiagnosticStreamFeed(stream, "  ", 2U) == UMI_STATUS_OK);
        }
        char line[8192];
        memset(line, 'x', sizeof line);
        CHECK(UmiCompilerDiagnosticStreamFeed(stream, line, sizeof line) == UMI_STATUS_OK);
        const char *tail = "suffix.c:7: error: must not become a record\n";
        CHECK(UmiCompilerDiagnosticStreamFeed(stream, tail, strlen(tail)) == UMI_STATUS_OK);
    }
    const char *valid = "good.c:5: warning: retained after damaged record\n";
    CHECK(UmiCompilerDiagnosticStreamFeed(stream, valid, strlen(valid)) == UMI_STATUS_OK);
    CHECK(UmiCompilerDiagnosticStreamFinish(stream) == UMI_STATUS_OK);
    CHECK(capture->list.count == 1U && capture->list.dropped == 1U);
    CHECK(strcmp(capture->list.items[0].file, "good.c") == 0);
    CHECK(capture->malformed == (strcmp(mode, "nul") == 0 ? 1U : 0U));
    UmiCompilerDiagnosticStreamDestroy(stream);
    free(capture);
    return 0;
}

static int Lifecycle(void)
{
    Capture *capture = calloc(1U, sizeof *capture);
    UmiCompilerDiagnosticStream *stream = NULL;
    CHECK(capture != NULL);
    CHECK(UmiCompilerDiagnosticStreamCreate(NULL, capture, &stream) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          stream == NULL);
    CHECK(UmiCompilerDiagnosticStreamCreate(Observe, capture, &stream) == UMI_STATUS_OK);
    CHECK(UmiCompilerDiagnosticStreamFeed(stream, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCompilerDiagnosticStreamFeed(stream, NULL, 0U) == UMI_STATUS_OK);
    const char *partial = "last.c:8: error: cancelled process ended here";
    CHECK(UmiCompilerDiagnosticStreamFeed(stream, partial, strlen(partial)) == UMI_STATUS_OK);
    CHECK(capture->list.count == 0U);
    CHECK(UmiCompilerDiagnosticStreamFinish(stream) == UMI_STATUS_OK);
    CHECK(capture->list.count == 1U && capture->list.items[0].line == 8U);
    UmiCompilerDiagnosticStreamDestroy(stream);
    stream = NULL;
    CHECK(UmiCompilerDiagnosticStreamCreate(Observe, capture, &stream) == UMI_STATUS_OK);
    CHECK(UmiCompilerDiagnosticStreamFeed(stream, partial, strlen(partial)) == UMI_STATUS_OK);
    UmiCompilerDiagnosticStreamDestroy(stream); /* Abandoning does not publish. */
    CHECK(capture->list.count == 1U);
    UmiCompilerDiagnosticStreamDestroy(NULL);
    free(capture);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (strcmp(argv[1], "split") == 0 || strcmp(argv[1], "bytes") == 0)
        return Fragmented(argv[1]);
    if (strcmp(argv[1], "lifecycle") == 0)
        return Lifecycle();
    CHECK(strcmp(argv[1], "nul") == 0 || strcmp(argv[1], "long-line") == 0 ||
          strcmp(argv[1], "long-continuation") == 0 || strcmp(argv[1], "record-bound") == 0);
    return Damaged(argv[1]);
}
