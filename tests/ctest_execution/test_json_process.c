/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_json_process.c
 * PURPOSE: Inject protocol capture and child failure states without starting CTest.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "json_test_support.h"
#include "umicom/platform/process.h"
static const char *scenario;
static size_t invocations;
UmiStatus UmiProcessExecuteStreamed(const UmiProcessRequest *request,
    UmiProcessOutputObserver observer, void *context, UmiProcessResult *result)
{
    const char *json = "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[{\"name\":\"notes.save\"}]}";
    ++invocations;
    REQUIRE(strcmp(request->program, "ctest") == 0 && request->argument_count == 5U);
    REQUIRE(strcmp(request->arguments[0], "--test-dir") == 0);
    REQUIRE(strcmp(request->arguments[2], "--show-only=json-v1") == 0);
    REQUIRE(strcmp(request->arguments[3], "-C") == 0 && strcmp(request->arguments[4], "Debug") == 0);
    REQUIRE(request->timeout_ms > 0U && request->capture_stdout && request->capture_stderr);
    memset(result, 0, sizeof(*result)); result->launched = 1;
    if (strcmp(scenario, "stderr-prefix") == 0) observer("warning\n", 8U, context);
    if (strcmp(scenario, "empty-output") != 0) {
        size_t length = strlen(json);
        if (strcmp(scenario, "partial-output") == 0) --length;
        for (size_t i = 0U; i < length; ++i) observer(json + i, 1U, context);
    }
    if (strcmp(scenario, "stderr-suffix") == 0) observer("\nwarning", 8U, context);
    strcpy(result->output, "Child diagnostics retained.");
    if (strcmp(scenario, "exit-failure") == 0) { result->exit_code = 2; return UMI_STATUS_INTERNAL_ERROR; }
    if (strcmp(scenario, "missing-launch") == 0) result->launched = 0;
    if (strcmp(scenario, "cancelled") == 0) result->cancelled = 1;
    if (strcmp(scenario, "timed-out") == 0) result->timed_out = 1;
    if (strcmp(scenario, "process-error") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(scenario, "diagnostic-truncation") == 0) result->output_truncated = 1;
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 3); scenario = argv[1];
    JsonTestState s; UmiTestPlatformCtestImportSummary summary;
    UmiTestPlatformCtestDiscoveryOptions control = {0}; char diagnostics[256];
    JsonTestCreate(&s); JsonTestSeed(&s);
    REQUIRE(strlen(argv[2]) < sizeof(s.options.build_directory)); strcpy(s.options.build_directory, argv[2]);
    s.options.configuration[0] = '\0'; /* Existing wrapper's Debug default remains. */
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(scenario, "stderr-prefix") == 0 || strcmp(scenario, "stderr-suffix") == 0 ||
        strcmp(scenario, "empty-output") == 0 || strcmp(scenario, "partial-output") == 0) expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(scenario, "exit-failure") == 0) expected = UMI_STATUS_INTERNAL_ERROR;
    if (strcmp(scenario, "missing-launch") == 0) expected = UMI_STATUS_UNAVAILABLE;
    if (strcmp(scenario, "cancelled") == 0) expected = UMI_STATUS_CANCELLED;
    if (strcmp(scenario, "timed-out") == 0) expected = UMI_STATUS_TIMEOUT;
    if (strcmp(scenario, "process-error") == 0) expected = UMI_STATUS_IO_ERROR;
    if (strcmp(scenario, "byte-limit") == 0) { expected = UMI_STATUS_CAPACITY_EXCEEDED; control.maximum_output_bytes = 16U; }
    uint64_t before = JsonTestFingerprint(&s);
    UmiStatus status = UmiTestPlatformCtestDiscoverConfigured(&s.options, &control,
        s.items, s.suites, s.discoveries, &summary, diagnostics, sizeof(diagnostics));
    REQUIRE(invocations == 1U && status == expected);
    if (status != UMI_STATUS_OK) {
        REQUIRE(JsonTestFingerprint(&s) == before && summary.discovered_count == 0U);
        REQUIRE(strstr(diagnostics, "catalogue unchanged") != NULL);
    } else REQUIRE(summary.discovered_count == 1U && diagnostics[0] == '\0');
    printf("%s: status=%d; chunked capture and publication checked.\n", scenario, (int)status);
    JsonTestDestroy(&s); return 0;
}
