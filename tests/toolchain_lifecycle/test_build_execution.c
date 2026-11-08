/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_execution.c
 *
 * PURPOSE:
 *   Verify literal commands, previews and phase failure boundaries through a fake process transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define TEST_LOG "C:/evidence/test output.log"
#define TEST_JUNIT "C:/evidence/results.xml"
#else
#define TEST_LOG "/evidence/test output.log"
#define TEST_JUNIT "/evidence/results.xml"
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

/* The probe now also distinguishes optional artifact/native-argument checks; retain its earlier layout. */
#if 0
typedef struct Fixture { unsigned calls, commands, fail_at; int bad; UmiBuildAction phases[8]; } Fixture;
#endif
typedef struct Fixture { unsigned calls, commands, fail_at; int bad; UmiBuildAction phases[8]; int extensions; } Fixture;
static int argument(const UmiProcessRequest *request, const char *key, const char *value)
{
    for (size_t i = 0U; i < request->argument_count; ++i)
        if (!strcmp(request->arguments[i], key) && (!value ||
            (i + 1U < request->argument_count && !strcmp(request->arguments[i + 1U], value)))) return 1;
    return 0;
}
static void observe(UmiBuildAction action, const UmiProcessRequest *request, void *context)
{
    Fixture *fixture = context;
    if (fixture->commands >= 8U) { fixture->bad = 1; return; }
    fixture->phases[fixture->commands++] = action;
    if (strcmp(request->working_directory, "project with spaces") != 0) fixture->bad = 1;
    if (action == UMI_BUILD_CONFIGURE && (!argument(request, "--preset", "preset") ||
        !argument(request, "-B", "binary folder") || !argument(request, "-D", "NAME=two words"))) fixture->bad = 1;
    if ((action == UMI_BUILD_COMPILE || action == UMI_BUILD_PACKAGE) &&
        (!argument(request, "--build", "binary folder") || !argument(request, "--config", "Release") ||
         !argument(request, "--parallel", "2"))) fixture->bad = 1;
    if (action == UMI_BUILD_TEST && (!argument(request, "--test-dir", "binary folder") ||
        !argument(request, "--no-tests=error", NULL) || !argument(request, "--output-on-failure", NULL) ||
        !argument(request, "--parallel", "2") || !argument(request, "-R", "^suite[.]"))) fixture->bad = 1;
    if (fixture->extensions) {
        int buildPhase = action == UMI_BUILD_COMPILE || action == UMI_BUILD_PACKAGE;
        if (buildPhase) {
            if (request->argument_count < 4U ||
                strcmp(request->arguments[request->argument_count - 4U], "--") ||
                strcmp(request->arguments[request->argument_count - 3U], "-k") ||
                strcmp(request->arguments[request->argument_count - 2U], "0") ||
                strcmp(request->arguments[request->argument_count - 1U], "NAME=two words;$(literal)")) fixture->bad = 1;
        } else if (argument(request, "--", NULL)) fixture->bad = 1;
        if (action == UMI_BUILD_TEST) {
            if (!argument(request, "--output-log", TEST_LOG) ||
                !argument(request, "--output-junit", TEST_JUNIT)) fixture->bad = 1;
        } else if (argument(request, "--output-log", NULL) ||
            argument(request, "--output-junit", NULL)) fixture->bad = 1;
    }
    if (action == UMI_BUILD_INSTALL && (!argument(request, "--prefix", "install folder") ||
        !argument(request, "--component", "Framework"))) fixture->bad = 1;
}
static UmiStatus execute(const UmiProcessRequest *request, UmiProcessOutputObserver output,
    void *output_context, UmiProcessResult *result, void *context)
{
    (void)request;
    Fixture *fixture = context;
    ++fixture->calls;
    memset(result, 0, sizeof *result);
    result->launched = 1;
    result->exit_code = fixture->fail_at == fixture->calls ? 17 : 0;
    if (output) output("output\n", 7U, output_context);
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiToolchainProfile profile;
    umi_toolchain_profile_init(&profile);
    UmiToolInfo *cmake = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_CMAKE);
    UmiToolInfo *ctest = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_CTEST);
    CHECK(cmake && ctest);
    cmake->state = ctest->state = UMI_TOOL_VALIDATED;
    (void)snprintf(cmake->path, sizeof cmake->path, "cmake");
    (void)snprintf(ctest->path, sizeof ctest->path, "ctest");
    UmiBuildRequest request;
    umi_build_request_init(&request);
    request.source_root = "project with spaces";
    request.build_directory = "binary folder";
    request.preset = "preset";
    request.configuration = "Release";
    request.jobs = 2;
    request.test_expression = "^suite[.]";
    request.install_prefix = "install folder";
    request.install_component = "Framework";
    const char *definitions[] = {"NAME=two words"};
    Fixture fixture = {0};
    UmiBuildExecutionOptions options = {0};
    options.explicit_build_directory = 1;
    options.configure_definitions = definitions;
    options.configure_definition_count = 1U;
    options.command = observe;
    options.observer_context = &fixture;
    options.executor = execute;
    options.executor_context = &fixture;
    UmiBuildReport *report = calloc(1U, sizeof *report);
    CHECK(report);
    /* One Framework validator serves preview, direct execution and logging.
     * Reject malformed definitions before command observers see partial work. */
    char message[256];
    const char *invalid_definitions[] = {NULL, "NO_EQUALS", "=missing-name", "-FLAG=value"};
    for (size_t i = 0U; i < sizeof invalid_definitions / sizeof invalid_definitions[0]; ++i) {
        options.configure_definitions = &invalid_definitions[i];
        CHECK(UmiBuildExecutionValidate(UMI_BUILD_DELIVER, &request, &options, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(message[0] != '\0');
        for (int preview = 0; preview <= 1; ++preview) {
            options.preview = preview;
            CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(fixture.calls == 0U && fixture.commands == 0U);
            CHECK(report->last_status == UMI_STATUS_INVALID_ARGUMENT);
        }
    }
    options.preview = 0; options.configure_definitions = definitions;
    CHECK(UmiBuildExecutionValidate(UMI_BUILD_COMPILE, &request, &options, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE, &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture.calls == 0U && fixture.commands == 0U);
    CHECK(UmiBuildExecutionValidate(UMI_BUILD_DELIVER, &request, &options, message, sizeof message) == UMI_STATUS_OK);
    const UmiBuildAction order[] = {UMI_BUILD_CONFIGURE, UMI_BUILD_COMPILE, UMI_BUILD_TEST, UMI_BUILD_INSTALL, UMI_BUILD_PACKAGE};
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 5U && fixture.commands == 5U && !fixture.bad);
    CHECK(memcmp(fixture.phases, order, sizeof order) == 0);
    /* A failure at every boundary prevents all later phases, including install. */
    for (unsigned failure = 1U; failure <= 5U; ++failure) {
        memset(&fixture, 0, sizeof fixture); fixture.fail_at = failure;
        CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) != UMI_STATUS_OK);
        CHECK(fixture.calls == failure && fixture.commands == failure && !fixture.bad);
        CHECK(report->last_action == order[failure - 1U]);
    }
    /* The same optional values must be scoped to their own phases, preserve
     * literal argument boundaries, and keep previews free of execution. */
    const char *native[] = {"-k", "0", "NAME=two words;$(literal)"};
    options.build_arguments = native; options.build_argument_count = 3U;
    options.test_output_log = TEST_LOG; options.test_output_junit = TEST_JUNIT;
    memset(&fixture, 0, sizeof fixture); fixture.extensions = 1;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 5U && fixture.commands == 5U && !fixture.bad);
    memset(&fixture, 0, sizeof fixture); fixture.extensions = 1; options.preview = 1;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 0U && fixture.commands == 5U && !fixture.bad);
    options.preview = 0; memset(&fixture, 0, sizeof fixture);
    options.build_argument_count = 17U;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture.calls == 0U && fixture.commands == 0U);
    options.build_argument_count = 1U; options.build_arguments = NULL;
    CHECK(UmiBuildExecutionValidate(UMI_BUILD_DELIVER, &request, &options, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    const char *bad_native[] = {NULL, ""};
    for (size_t i = 0U; i < 2U; ++i) {
        options.build_arguments = &bad_native[i];
        CHECK(UmiBuildExecutionValidate(UMI_BUILD_DELIVER, &request, &options, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    }
    options.build_argument_count = 0U; options.build_arguments = NULL;
    options.test_output_junit = TEST_LOG;
    CHECK(UmiBuildExecutionValidate(UMI_BUILD_DELIVER, &request, &options, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    options.test_output_junit = "relative.xml";
    CHECK(UmiBuildExecutionValidate(UMI_BUILD_DELIVER, &request, &options, NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    options.test_output_log = NULL; options.test_output_junit = NULL;
    memset(&fixture, 0, sizeof fixture);
    options.preview = 1;
    cmake->state = ctest->state = UMI_TOOL_MISSING;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 0U && fixture.commands == 5U && !fixture.bad);
    CHECK(report->configure_exit_code == -1 && report->package_exit_code == -1);
    options.preview = 0;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_DELIVER, &request, &options, report) == UMI_STATUS_NOT_FOUND);
    CHECK(fixture.calls == 0U);
    request.clean = 1; request.build_directory = NULL; options.preview = 1;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_CONFIGURE, &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture.calls == 0U);
    options.explicit_build_directory = 0;
    request.clean = 0; request.executable = "fake"; request.run_argument_count = 1U; request.run_arguments = NULL;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_RUN, &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    free(report);
    return 0;
}
