/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_report_failures.c
 *
 * PURPOSE:
 *   Inject process/report failures at the existing Framework process boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_adapter.h"
#include "umicom/platform/process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

/* Deliberate test double, linked only in this executable. The separate real
 * execution tests link the production process implementation without mocks. */
static unsigned mode, calls, failures, checks;
static char reportPath[4096], previousPath[4096];
#define CHECK(condition) do { ++checks; if (!(condition)) { \
    ++failures; fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
} } while (0)

UmiStatus umi_process_execute(const UmiProcessRequest *request, UmiProcessResult *result)
{
    ++calls; memset(result, 0, sizeof *result); result->exit_code = -1;
    CHECK(request != NULL && request->argument_count == 10U);
    CHECK(strcmp(request->program, "ctest") == 0);
    CHECK(strcmp(request->arguments[0], "--test-dir") == 0);
    CHECK(strcmp(request->arguments[1], "Umicom build root") == 0);
    CHECK(strcmp(request->arguments[3], "^notes\\.save$") == 0);
    CHECK(strcmp(request->arguments[5], "--no-tests=error") == 0);
    CHECK(strcmp(request->arguments[6], "--output-junit") == 0);
    CHECK(strcmp(request->arguments[8], "-C") == 0 && strcmp(request->arguments[9], "Debug") == 0);
    CHECK(request->working_directory == NULL && request->window_mode == UMI_PROCESS_WINDOW_HIDDEN);
    CHECK(strlen(request->arguments[7]) < sizeof reportPath);
    snprintf(reportPath, sizeof reportPath, "%s", request->arguments[7]);
    CHECK(strcmp(reportPath, previousPath) != 0);
    FILE *old = fopen(reportPath, "rb"); CHECK(old == NULL); if (old != NULL) fclose(old);
    if (mode == 0U) return UMI_STATUS_IO_ERROR; /* Launch failed. */
    result->launched = 1; result->exit_code = 0;
    if (mode == 1U) return UMI_STATUS_OK; /* Successful process, no report. */
    FILE *file = fopen(reportPath, "wb");
    CHECK(file != NULL); if (file == NULL) return UMI_STATUS_IO_ERROR;
    const char *valid = "<testsuite tests=\"1\"><testcase name=\"notes.save\" status=\"run\" time=\"0\"/></testsuite>";
    if (mode == 2U) fputs("<testsuite>", file);
    else if (mode == 3U) fputs("<testsuite tests=\"0\"/>", file);
    else if (mode == 4U) fputs("<testsuite tests=\"2\"><testcase name=\"notes.save\" status=\"run\" time=\"0\"/><testcase name=\"notes.save\" status=\"run\" time=\"0\"/></testsuite>", file);
    else if (mode == 5U) {
        char chunk[4096]; memset(chunk, 'x', sizeof chunk);
        for (size_t size = 0U; size <= UMI_CTEST_REPORT_MAX_BYTES; size += sizeof chunk)
            CHECK(fwrite(chunk, 1U, sizeof chunk, file) == sizeof chunk);
    } else if (mode == 6U) { /* A NUL cannot hide a contradictory tail. */
        fputs(valid, file); fputc(0, file); fputs("<testcase/>", file);
    } else fputs(valid, file);
    CHECK(fclose(file) == 0);
    if (mode == 11U) {
        char extra[4104]; snprintf(extra, sizeof extra, "%s.extra", reportPath);
        FILE *unexpected = fopen(extra, "wb");
        CHECK(unexpected != NULL); if (unexpected != NULL) fclose(unexpected);
    }
    if (mode == 12U) return UMI_STATUS_IO_ERROR;
    if (mode == 7U) { result->exit_code = 8; return UMI_STATUS_INTERNAL_ERROR; }
    if (mode == 8U) { result->cancelled = 1; return UMI_STATUS_CANCELLED; }
    if (mode == 9U) { result->timed_out = 1; return UMI_STATUS_TIMEOUT; }
    return UMI_STATUS_OK;
}

/* This symbol is needed by the unchanged discovery entry point, but no test in
 * this executable calls discovery. It must not silently fake discovery data. */
UmiStatus umi_process_capture(const char *program, const char *const *arguments,
    size_t count, char *outText, size_t capacity, int *outExit)
{
    (void)program; (void)arguments; (void)count; (void)outText;
    (void)capacity; (void)outExit; return UMI_STATUS_NOT_IMPLEMENTED;
}

int main(void)
{
    static const UmiStatus statuses[] = { UMI_STATUS_IO_ERROR, UMI_STATUS_IO_ERROR,
        UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND, UMI_STATUS_PARSE_ERROR,
        UMI_STATUS_CAPACITY_EXCEEDED, UMI_STATUS_PARSE_ERROR,
        UMI_STATUS_INTERNAL_ERROR, UMI_STATUS_CANCELLED, UMI_STATUS_TIMEOUT,
        UMI_STATUS_OK, UMI_STATUS_IO_ERROR, UMI_STATUS_IO_ERROR };
    UmiCtestRunOptions options = {0}; options.configuration = "Debug";
    UmiTestResult result;
    for (mode = 0U; mode < sizeof statuses / sizeof statuses[0]; ++mode) {
        UmiStatus status = UmiCtestRunConfigured("Umicom build root", "notes.save", &options, &result);
        CHECK(status == statuses[mode] && result.status == status);
        CHECK((UmiCtestResultRequirePassed(&result) == UMI_STATUS_OK) == (mode == 10U));
        FILE *file = fopen(reportPath, "rb"); CHECK(file == NULL); if (file != NULL) fclose(file);
        if (mode == 11U) {
            /* The adapter must leave the unexpected file, not delete recursively.
             * This test created it, so only this test is entitled to remove it. */
            char extra[4104]; snprintf(extra, sizeof extra, "%s.extra", reportPath);
            file = fopen(extra, "rb"); CHECK(file != NULL); if (file != NULL) fclose(file);
            CHECK(result.state == UMI_TEST_STATE_FAILED);
            CHECK(remove(extra) == 0);
            char directory[4096]; snprintf(directory, sizeof directory, "%s", reportPath);
            char *end = strrchr(directory, '/');
#ifdef _WIN32
            char *backslash = strrchr(directory, '\\');
            if (end == NULL || (backslash != NULL && backslash > end)) end = backslash;
#endif
            CHECK(end != NULL);
            if (end != NULL) {
                *end = '\0';
#ifdef _WIN32
                CHECK(_rmdir(directory) == 0);
#else
                CHECK(rmdir(directory) == 0);
#endif
            }
        }
        snprintf(previousPath, sizeof previousPath, "%s", reportPath);
    }
    CHECK(calls == 13U);
    printf("Injected failures: %u checks, %u failures\n", checks, failures);
    return failures == 0U ? 0 : 1;
}
