/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_logging.c
 * PURPOSE: Verify complete raw logs and non-overwrite behavior with a fake child process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); goto finish; } } while (0)

typedef struct Fixture { size_t calls, seen; int exit_code; unsigned char *bytes; } Fixture;
static void observe(const char *bytes, size_t size, void *context)
{
    Fixture *fixture = context;
    if (bytes && size) fixture->seen += size;
}
static UmiStatus execute(const UmiProcessRequest *request, UmiProcessOutputObserver output,
    void *output_context, UmiProcessResult *result, void *context)
{
    Fixture *fixture = context;
    ++fixture->calls;
    if (request->argument_count != 2U || strcmp(request->arguments[0], "two words") ||
        strcmp(request->arguments[1], "$(literal);*")) return UMI_STATUS_INVALID_ARGUMENT;
    memset(result, 0, sizeof *result);
    result->launched = 1; result->exit_code = fixture->exit_code;
    result->output_truncated = 1;
    /* The diagnostic tail is deliberately incomplete; raw capture must not be. */
    memcpy(result->output, "tail", 5U);
    if (output) {
        output((const char *)fixture->bytes, 40000U, output_context);
        output((const char *)fixture->bytes + 40000U, 60000U, output_context);
    }
    return UMI_STATUS_OK;
}
int main(void)
{
    int passed = 0, owns = 0;
    char temp[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY] = {0}, path[UMI_PATH_CAPACITY], second[UMI_PATH_CAPACITY];
    unsigned char *readback = NULL;
    UmiBuildReport *report = calloc(1U, sizeof *report);
    Fixture fixture = {0}; fixture.bytes = malloc(100000U);
    CHECK(report && fixture.bytes);
    for (size_t i = 0U; i < 100000U; ++i) fixture.bytes[i] = (unsigned char)(i % 256U);
    UmiClock clock = umi_clock_system();
    char leaf[128];
    (void)snprintf(leaf, sizeof leaf, "umicom-build-log-%llu", (unsigned long long)clock.monotonic_nanoseconds(&clock));
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(umi_path_join(temp, leaf, directory, sizeof directory) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(directory));
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK); owns = 1;
    CHECK(umi_path_join(directory, "output.log", path, sizeof path) == UMI_STATUS_OK);
    CHECK(umi_path_join(directory, "failure.log", second, sizeof second) == UMI_STATUS_OK);
    UmiToolchainProfile profile; umi_toolchain_profile_init(&profile);
    UmiBuildRequest request; umi_build_request_init(&request);
    request.source_root = directory; request.executable = "fake-program";
    const char *args[] = {"two words", "$(literal);*"};
    request.run_arguments = args; request.run_argument_count = 2U;
    UmiBuildExecutionOptions options = {0};
    options.executor = execute; options.executor_context = &fixture;
    options.output = observe; options.observer_context = &fixture;
    UmiBuildExecutionLog log;
    CHECK(UmiBuildExecuteLogged(&profile, NULL, UMI_BUILD_RUN, &request, &options, path, &log, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 1U && fixture.seen == 100000U && log.created && log.capture_complete);
    CHECK(log.file.bytes_written == 100000U && log.log_status == UMI_STATUS_OK);
    size_t size = 0U;
    CHECK(umi_fs_read_bytes(path, &readback, &size) == UMI_STATUS_OK);
    CHECK(size == 100000U && !memcmp(readback, fixture.bytes, size));
    umi_fs_free_bytes(readback); readback = NULL;
    CHECK(UmiBuildExecuteLogged(&profile, NULL, UMI_BUILD_RUN, &request, &options, path, &log, report) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(fixture.calls == 1U && !log.created);
    options.preview = 1;
    CHECK(UmiBuildExecuteLogged(&profile, NULL, UMI_BUILD_RUN, &request, &options, second, &log, report) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(second) && fixture.calls == 1U && report->run_exit_code == -1);
    options.preview = 0; fixture.exit_code = 23;
    CHECK(UmiBuildExecuteLogged(&profile, NULL, UMI_BUILD_RUN, &request, &options, second, &log, report) != UMI_STATUS_OK);
    CHECK(report->run_exit_code == 23 && log.capture_complete && log.execution_status != UMI_STATUS_OK);
    CHECK(umi_fs_exists(second));
    CHECK(UmiBuildExecuteLogged(&profile, NULL, UMI_BUILD_RUN, &request, &options, "relative.log", &log, report) != UMI_STATUS_OK);
    CHECK(fixture.calls == 2U);
    passed = 1;
finish:
    umi_fs_free_bytes(readback); free(fixture.bytes); free(report);
    /* Only this unique test-owned directory may be removed. */
    if (owns && umi_fs_remove_tree(directory) != UMI_STATUS_OK) passed = 0;
    return passed ? 0 : 1;
}
