/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_report_workspace.c
 * PURPOSE: Inspect result-file placement without launching CTest.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_adapter.h"
#include "umicom/platform/process.h"
#include "umicom/platform/path.h"
#include "report_file_helpers.h"
#include <stdio.h>
#include <string.h>

static unsigned calls, errors;
static const char *root, *mode;
static char previous[4096], captured[4096];
#define CHECK(condition) do { if (!(condition)) { ++errors; \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #condition); } } while (0)

UmiStatus umi_process_execute(const UmiProcessRequest *request, UmiProcessResult *result)
{
    ++calls;
    memset(result, 0, sizeof *result);
    result->exit_code = -1;
    CHECK(request->argument_count == 8U);
    const char *report = request->arguments[7];
    CHECK(umi_path_is_absolute(report));
    CHECK(strlen(report) < sizeof captured);
    (void)snprintf(captured, sizeof captured, "%s", report);
    char normalised[4096];
    (void)snprintf(normalised, sizeof normalised, "%s", report);
    for (char *cursor = normalised; *cursor != '\0'; ++cursor)
        if (*cursor == '\\') *cursor = '/';
    char build[4096];
    (void)snprintf(build, sizeof build, "%s", request->arguments[1]);
    for (char *cursor = build; *cursor != '\0'; ++cursor)
        if (*cursor == '\\') *cursor = '/';
    CHECK(umi_path_is_absolute(build));
    CHECK(strncmp(normalised, build, strlen(build)) == 0);
    CHECK(normalised[strlen(build)] == '/');
    CHECK(strstr(normalised, "/test-work/umicom-ctest-") != NULL);
    CHECK(strstr(normalised, "report caf\xC3\xA9") != NULL);
    CHECK(strcmp(captured, previous) != 0);
    FILE *existing = ReportFixtureOpen(captured, "rb");
    CHECK(existing == NULL);
    if (existing != NULL) fclose(existing);
    if (strcmp(mode, "launch-failure") == 0) return UMI_STATUS_UNAVAILABLE;
    FILE *output = ReportFixtureOpen(captured, "wb");
    CHECK(output != NULL);
    if (output == NULL) return UMI_STATUS_IO_ERROR;
    CHECK(fputs("<testsuite tests='1'><testcase name='notes' status='run' time='0'/></testsuite>", output) >= 0);
    CHECK(fclose(output) == 0);
    result->launched = 1;
    result->exit_code = 0;
    return UMI_STATUS_OK;
}

UmiStatus UmiProcessExecuteWithLifetime(const UmiProcessRequest *request,
    UmiProcessLifetime lifetime, UmiProcessResultObserver observer,
    UmiProcessOutputObserver rawObserver, void *context, UmiProcessResult *result)
{
    (void)context;
    if (lifetime != UMI_PROCESS_LIFETIME_TREE || observer != NULL || rawObserver != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_process_execute(request, result);
}

UmiStatus umi_process_capture(const char *program, const char *const *arguments,
    size_t count, char *outText, size_t capacity, int *outExit)
{
    (void)program; (void)arguments; (void)count; (void)outText;
    (void)capacity; (void)outExit;
    return UMI_STATUS_NOT_IMPLEMENTED;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    root = argv[1]; mode = argv[2];
    if (strcmp(mode, "unicode") != 0 && strcmp(mode, "relative") != 0 &&
        strcmp(mode, "launch-failure") != 0 && strcmp(mode, "blocked") != 0 &&
        strcmp(mode, "missing") != 0) return 2;
    UmiCtestRunOptions options = {0};
    UmiTestResult result;
    UmiStatus expected = UMI_STATUS_OK;
    char obstruction[4096] = "";
    if (strcmp(mode, "blocked") == 0) {
        int used = snprintf(obstruction, sizeof obstruction, "%s/test-work", root);
        CHECK(used > 0 && (size_t)used < sizeof obstruction);
        FILE *file = ReportFixtureOpen(obstruction, "wb");
        CHECK(file != NULL);
        if (file != NULL) {
            CHECK(fputs("owned fixture; retain until checked", file) >= 0);
            CHECK(fclose(file) == 0);
        }
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(mode, "missing") == 0) expected = UMI_STATUS_NOT_FOUND;
    else if (strcmp(mode, "launch-failure") == 0) expected = UMI_STATUS_UNAVAILABLE;
    if (errors != 0U) return 1;
    unsigned attempts = expected == UMI_STATUS_OK ? 2U : 1U;
    for (unsigned index = 0U; index < attempts; ++index) {
        CHECK(UmiCtestRunConfigured(root, "notes", &options, &result) == expected);
        CHECK(result.status == expected);
        CHECK((UmiCtestResultRequirePassed(&result) == UMI_STATUS_OK) == (expected == UMI_STATUS_OK));
        if (captured[0] != '\0') {
            FILE *file = ReportFixtureOpen(captured, "rb");
            CHECK(file == NULL);
            if (file != NULL) fclose(file);
            (void)snprintf(previous, sizeof previous, "%s", captured);
        }
    }
    CHECK(calls == ((expected == UMI_STATUS_OK || expected == UMI_STATUS_UNAVAILABLE) ? attempts : 0U));
    if (obstruction[0] != '\0') {
        FILE *file = ReportFixtureOpen(obstruction, "rb");
        CHECK(file != NULL);
        if (file != NULL) {
            char text[80] = "";
            CHECK(fgets(text, sizeof text, file) != NULL);
            CHECK(strcmp(text, "owned fixture; retain until checked") == 0);
            CHECK(fclose(file) == 0);
        }
        CHECK(ReportFixtureRemove(obstruction) == 0);
    }
    return errors == 0U ? 0 : 1;
}
