/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_review/test_cmake_records.c
 * PURPOSE: Check CMake explanation ownership, bounds and shared build-review search.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/parser.h"
#include "umicom/build/review.h"
#include "umicom/diagnostics/compiler_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

/* Public examples use small, readable transcripts. The capacity cases below
 * build exact boundary inputs so an off-by-one cannot hide behind a long log. */
static const char header[] = "CMake Error at framework/CMakeLists.txt:32 (add_executable):";
static const char collision[] =
    "CMake Error at framework/CMakeLists.txt:32 (add_executable):\n"
    "  add_executable cannot create target \"sample-test\" because another target\n"
    "  with the same name already exists.\n"
    "Call Stack (most recent call first):\n"
    "  CMakeLists.txt:40 (include)\n"
    "\n-- Configuring incomplete, errors occurred!\n";

static int Expected(const char *text, const char *message, const char *remainder)
{
    UmiCompilerDiagnosticFields fields;
    size_t consumed = 0U;
    CHECK(UmiCompilerDiagnosticParseBlock(text, &fields, &consumed) == UMI_STATUS_OK);
    CHECK(strcmp(fields.message, message) == 0);
    CHECK(strcmp(text + consumed, remainder) == 0);
    return 0;
}

static int Grammar(const char *name)
{
    char text[4096], message[4096];
    if (strcmp(name, "collision") == 0) {
        snprintf(message, sizeof message, "%s\n%s\n%s", header,
            "add_executable cannot create target \"sample-test\" because another target",
            "with the same name already exists.");
        return Expected(collision, message, strstr(collision, "Call Stack"));
    }
    if (strcmp(name, "paragraphs") == 0) {
        snprintf(text, sizeof text, "%s\n  First explanation.\n \n\tSecond paragraph.\n\n-- Done\n", header);
        snprintf(message, sizeof message, "%s\nFirst explanation.\n\nSecond paragraph.", header);
        return Expected(text, message, "\n-- Done\n");
    }
    if (strcmp(name, "crlf") == 0) {
        snprintf(text, sizeof text, "%s\r\n  Path contains caf\xc3\xa9.\r\n-- Stop\r\n", header);
        snprintf(message, sizeof message, "%s\nPath contains caf\xc3\xa9.", header);
        return Expected(text, message, "-- Stop\r\n");
    }
    if (strcmp(name, "colour") == 0) {
        snprintf(text, sizeof text, "\x1b[31m%s\x1b[0m\n\x1b[33m  Explanation.\x1b[0m\n", header);
        snprintf(message, sizeof message, "%s\nExplanation.", header);
        return Expected(text, message, "");
    }
    if (strcmp(name, "trailing-blank") == 0) {
        snprintf(text, sizeof text, "%s\n  Explanation.\n\n \n", header);
        snprintf(message, sizeof message, "%s\nExplanation.", header);
        return Expected(text, message, "\n \n");
    }
    if (strcmp(name, "next-header") == 0 || strcmp(name, "indented-header") == 0) {
        const char *next = strcmp(name, "next-header") == 0
            ? "CMake Warning at other.cmake:4 (message):\n  Warning explanation.\n"
            : "  CMake Warning at other.cmake:4 (message):\n  Warning explanation.\n";
        snprintf(text, sizeof text, "%s\n  First error.\n%s", header, next);
        snprintf(message, sizeof message, "%s\nFirst error.", header);
        return Expected(text, message, next);
    }
    if (strcmp(name, "next-compiler") == 0) {
        const char *next = "source.c:5:3: error: missing token\n";
        snprintf(text, sizeof text, "%s\n  First error.\n%s", header, next);
        snprintf(message, sizeof message, "%s\nFirst error.", header);
        return Expected(text, message, next);
    }
    if (strcmp(name, "header-only") == 0) return Expected(header, header, "");
    if (strcmp(name, "terminal-control") == 0) {
        snprintf(text, sizeof text, "%s\n  \x1b]unsupported\n-- Done\n", header);
        return Expected(text, header, "  \x1b]unsupported\n-- Done\n");
    }
    if (strcmp(name, "false-header") == 0)
        return Expected("source.c:4: error: CMake failed\n  source excerpt\n",
            "CMake failed", "  source excerpt\n");
    if (strcmp(name, "warning") == 0) {
        UmiCompilerDiagnosticFields fields;
        size_t consumed = 0U;
        const char *warning = "CMake Warning (dev) at C:/Project Files/rules.cmake:8 (message):\n  Check policy.\n";
        CHECK(UmiCompilerDiagnosticParseBlock(warning, &fields, &consumed) == UMI_STATUS_OK);
        CHECK(fields.severity == UMI_DIAGNOSTIC_WARNING && fields.line == 8U && fields.column == 0U);
        CHECK(strcmp(fields.path, "C:/Project Files/rules.cmake") == 0);
        CHECK(strstr(fields.message, "\nCheck policy.") != NULL && consumed == strlen(warning));
        return 0;
    }
    return 1;
}

static int Capacity(const char *name)
{
    UmiCompilerDiagnosticFields fields, before;
    memset(&fields, 0x51, sizeof fields);
    memcpy(&before, &fields, sizeof before);
    char *text = calloc(16000U, 1U);
    CHECK(text != NULL);
    size_t body = UMI_DIAGNOSTIC_MESSAGE_CAPACITY - strlen(header) - 2U;
    int exact = strcmp(name, "exact-fit") == 0;
    if (strcmp(name, "long-continuation") == 0) body = 9000U;
    else if (!exact) ++body;
    size_t used = (size_t)snprintf(text, 16000U, "%s\n  ", header);
    memset(text + used, 'x', body);
    used += body;
    strcpy(text + used, "\nsource.c:9: error: separate error\n");
    size_t consumed = 0U;
    UmiStatus status = UmiCompilerDiagnosticParseBlock(text, &fields, &consumed);
    CHECK(strcmp(text + consumed, "source.c:9: error: separate error\n") == 0);
    if (exact) {
        CHECK(status == UMI_STATUS_OK);
        CHECK(strlen(fields.message) == UMI_DIAGNOSTIC_MESSAGE_CAPACITY - 1U);
    } else {
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&fields, &before, sizeof fields) == 0);
        /* The build projection must skip the entire oversized explanation but
         * still retain the next compiler error and account for the lost record. */
        UmiBuildDiagnosticList *list = calloc(1U, sizeof *list);
        CHECK(list != NULL);
        CHECK(umi_build_parse_output(text, list) == UMI_STATUS_OK);
        CHECK(list->count == 1U && list->dropped == 1U);
        CHECK(strcmp(list->items[0].file, "source.c") == 0);
        free(list);
    }
    free(text);
    return 0;
}

static int Contract(const char *name)
{
    UmiCompilerDiagnosticFields fields, before;
    memset(&fields, 0x51, sizeof fields);
    memcpy(&before, &fields, sizeof before);
    size_t consumed = 99U;
    if (strcmp(name, "ordinary") == 0) {
        CHECK(UmiCompilerDiagnosticParseBlock("-- Configuring\nnext", &fields, &consumed) == UMI_STATUS_NOT_FOUND);
        CHECK(consumed == strlen("-- Configuring\n"));
    } else if (strcmp(name, "empty") == 0) {
        CHECK(UmiCompilerDiagnosticParseBlock("", &fields, &consumed) == UMI_STATUS_NOT_FOUND);
        CHECK(consumed == 0U);
    } else if (strcmp(name, "invalid") == 0) {
        CHECK(UmiCompilerDiagnosticParseBlock(NULL, &fields, &consumed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(consumed == 99U);
        CHECK(UmiCompilerDiagnosticParseBlock(header, NULL, &consumed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(consumed == 99U);
        CHECK(UmiCompilerDiagnosticParseBlock(header, &fields, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 1;
    CHECK(memcmp(&fields, &before, sizeof fields) == 0);
    return 0;
}

static int Review(const char *name)
{
    UmiBuildReview *review = NULL;
    UmiBuildReviewSummary summary = {0};
    UmiBuildReviewFilter filter = {UMI_BUILD_DIAGNOSTIC_NOTE, ""};
    UmiBuildDiagnostic diagnostic;
    CHECK(UmiBuildReviewImportLog(collision, strlen(collision), &review) == UMI_STATUS_OK);
    strcpy(filter.contains, "same name already exists");
    CHECK(UmiBuildReviewSummarise(review, 0U, &filter, &summary) == UMI_STATUS_OK);
    CHECK(summary.totalDiagnostics == 1U && summary.visibleDiagnostics == 1U);
    CHECK(summary.outcome == UMI_BUILD_REVIEW_UNRECORDED);
    CHECK(summary.droppedDiagnostics == 0U);
    CHECK(UmiBuildReviewDiagnostic(review, 0U, &filter, 0U, &diagnostic) == UMI_STATUS_OK);
    CHECK(strcmp(diagnostic.file, "framework/CMakeLists.txt") == 0 && diagnostic.line == 32U);
    CHECK(strstr(diagnostic.message, "same name already exists") != NULL);
    CHECK(strstr(diagnostic.message, "Call Stack") == NULL);
    if (strcmp(name, "report") == 0) {
        char *report = NULL;
        size_t length = 0U;
        CHECK(UmiBuildReviewRender(review, 0U, &filter, &report, &length) == UMI_STATUS_OK);
        CHECK(length == strlen(report));
        CHECK(strstr(report, "Call Stack (most recent call first):") != NULL);
        CHECK(strstr(report, "-- Configuring incomplete") != NULL);
        UmiBuildReviewTextFree(report);
    } else CHECK(strcmp(name, "search") == 0);
    UmiBuildReviewDestroy(review);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (strcmp(argv[1], "grammar") == 0) return Grammar(argv[2]);
    if (strcmp(argv[1], "capacity") == 0) return Capacity(argv[2]);
    if (strcmp(argv[1], "contract") == 0) return Contract(argv[2]);
    if (strcmp(argv[1], "review") == 0) return Review(argv[2]);
    return 2;
}
