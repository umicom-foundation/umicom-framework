/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_incremental.c
 *
 * PURPOSE:
 *   Verify explicit incremental build/test selection, argument ownership and
 *   failure boundaries without launching a compiler or another process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/build_invocation.h"
#include "umicom/toolchain/build_execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
#ifdef _WIN32
#define WORK "C:/work/project"
#else
#define WORK "/work/project"
#endif

typedef struct Fixture {
    unsigned commands, calls, fail_at;
    int invalid, cancelled, timed_out;
    UmiBuildAction actions[3];
} Fixture;

static int Has(const UmiProcessRequest *command, const char *key, const char *value)
{
    for (size_t i = 0U; i < command->argument_count; ++i) {
        if (strcmp(command->arguments[i], key) == 0)
            return value == NULL || (i + 1U < command->argument_count &&
                strcmp(command->arguments[i + 1U], value) == 0);
    }
    return 0;
}

/* The observer checks the real Framework command builder. The transport below
 * only substitutes the child outcome; it does not reconstruct command arguments. */
static void Command(UmiBuildAction action, const UmiProcessRequest *command, void *context)
{
    Fixture *fixture = context;
    if (fixture->commands >= 3U) { fixture->invalid = 1; return; }
    fixture->actions[fixture->commands++] = action;
    if (strcmp(command->working_directory, WORK) != 0 ||
        !Has(command, "--parallel", "2") ||
        Has(command, "--clean-first", NULL) || Has(command, "--preset", NULL) ||
        Has(command, "-S", NULL) || Has(command, "-B", NULL))
        fixture->invalid = 1;
    if (action == UMI_BUILD_COMPILE) {
        if (!Has(command, "--build", WORK "/output") ||
            !Has(command, "--target", "selected-tests") ||
            !Has(command, "--config", "Debug")) fixture->invalid = 1;
    } else if (action == UMI_BUILD_TEST) {
        if (!Has(command, "--test-dir", WORK "/output") ||
            !Has(command, "-R", "^selected[.]") ||
            !Has(command, "--no-tests=error", NULL) ||
            !Has(command, "--output-on-failure", NULL) ||
            !Has(command, "-C", "Debug")) fixture->invalid = 1;
    } else fixture->invalid = 1;
}

static UmiStatus Execute(const UmiProcessRequest *command, UmiProcessOutputObserver output,
    void *output_context, UmiProcessResult *result, void *context)
{
    (void)command;
    Fixture *fixture = context;
    ++fixture->calls;
    memset(result, 0, sizeof *result);
    result->launched = 1;
    result->exit_code = fixture->fail_at == fixture->calls ? 17 : 0;
    result->cancelled = fixture->cancelled;
    result->timed_out = fixture->timed_out;
    if (output) output("incremental phase\n", 18U, output_context);
    return UMI_STATUS_OK;
}

static int Parsing(void)
{
    UmiBuildInvocation *inv = calloc(1U, sizeof *inv);
    CHECK(inv != NULL);
    char message[256];
    const char *valid[] = {"--build", "output", "--target", "selected-tests",
        "--tests", "^selected[.]", "--jobs", "2", "--dry-run"};
    CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE_TEST, 9, valid, WORK,
        inv, message, sizeof message) == UMI_STATUS_OK);
    CHECK(inv->options.preview && inv->options.explicit_build_directory);
    CHECK(inv->options.configure_definition_count == 0U && !inv->request.clean);
    CHECK(inv->request.preset == NULL && inv->request.jobs == 2);
    CHECK(strcmp(inv->request.target, "selected-tests") == 0);
    CHECK(strcmp(inv->request.test_expression, "^selected[.]") == 0);
    /* Missing selection and configure/install options must fail before discovery.
     * Existing make and check/doctor interfaces remain separate operations. */
    const char *invalid[][9] = {
        {"--target", "selected-tests", "--tests", "^selected[.]"},
        {"--build", "output", "--tests", "^selected[.]"},
        {"--build", "output", "--target", "selected-tests"},
        {"--build", "output", "--target", "selected-tests", "--tests", ""},
        {"--build", "output", "--target", "selected-tests", "--tests", "^selected[.]", "--clean"},
        {"--build", "output", "--target", "selected-tests", "--tests", "^selected[.]", "--preset", "debug"},
        {"--build", "output", "--target", "selected-tests", "--tests", "^selected[.]", "--define", "X=Y"},
        {"--build", "output", "--target", "selected-tests", "--tests", "^selected[.]", "--prefix", "install"},
        {"--build", "output", "--target", "selected-tests", "--tests", "^selected[.]", "--jobs", "0"},
        {"--build", "output", "--target", "selected-tests", "--tests", "^selected[.]", "--target", "other"}
    };
    for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
        int count = 0;
        while (count < 9 && invalid[i][count] != NULL) ++count;
        CHECK(UmiBuildInvocationParse(UMI_BUILD_COMPILE_TEST, count, invalid[i],
            WORK, inv, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
    }
    free(inv);
    return 0;
}

int main(void)
{
    CHECK(UMI_BUILD_COMMAND == 10 && UMI_BUILD_COMPILE_TEST == 11);
    CHECK(strcmp(umi_build_action_text(UMI_BUILD_COMPILE_TEST), "build-test") == 0);
    CHECK(Parsing() == 0);
    UmiToolchainProfile profile;
    umi_toolchain_profile_init(&profile);
    UmiToolInfo *cmake = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_CMAKE);
    UmiToolInfo *ctest = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_CTEST);
    CHECK(cmake != NULL && ctest != NULL);
    cmake->state = ctest->state = UMI_TOOL_VALIDATED;
    (void)snprintf(cmake->path, sizeof cmake->path, "cmake");
    (void)snprintf(ctest->path, sizeof ctest->path, "ctest");
    UmiBuildRequest request;
    umi_build_request_init(&request);
    request.source_root = WORK;
    request.build_directory = WORK "/output";
    request.target = "selected-tests";
    request.test_expression = "^selected[.]";
    request.jobs = 2;
    request.configuration = "Debug";
    Fixture fixture = {0};
    UmiBuildExecutionOptions options = {0};
    options.command = Command; options.observer_context = &fixture;
    options.executor = Execute; options.executor_context = &fixture;
    UmiBuildReport *report = calloc(1U, sizeof *report);
    CHECK(report != NULL);
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
        &request, &options, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 2U && fixture.commands == 2U && !fixture.invalid);
    CHECK(fixture.actions[0] == UMI_BUILD_COMPILE && fixture.actions[1] == UMI_BUILD_TEST);
    CHECK(report->configure_exit_code == -1 && report->install_exit_code == -1 &&
        report->package_exit_code == -1 && report->run_exit_code == -1);
    for (unsigned failed = 1U; failed <= 2U; ++failed) {
        memset(&fixture, 0, sizeof fixture); fixture.fail_at = failed;
        CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
            &request, &options, report) != UMI_STATUS_OK);
        CHECK(fixture.calls == failed && fixture.commands == failed && !fixture.invalid);
        CHECK(report->last_action == (failed == 1U ? UMI_BUILD_COMPILE : UMI_BUILD_TEST));
        CHECK(failed != 1U || report->test_exit_code == -1);
    }
    /* Cancellation and timeout at the build boundary must prevent all tests. */
    for (unsigned mode = 0U; mode < 2U; ++mode) {
        memset(&fixture, 0, sizeof fixture);
        fixture.cancelled = mode == 0U; fixture.timed_out = mode == 1U;
        CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
            &request, &options, report) != UMI_STATUS_OK);
        CHECK(fixture.calls == 1U && report->test_exit_code == -1);
    }
    memset(&fixture, 0, sizeof fixture); options.preview = 1;
    cmake->state = ctest->state = UMI_TOOL_MISSING;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
        &request, &options, report) == UMI_STATUS_OK);
    CHECK(fixture.calls == 0U && fixture.commands == 2U && !fixture.invalid);
    CHECK(report->build_exit_code == -1 && report->test_exit_code == -1);
    /* Validate direct C callers too, not only CLI argument parsing. */
    memset(&fixture, 0, sizeof fixture);
    request.clean = 1;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
        &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture.commands == 0U);
    request.clean = 0;
    const char *definitions[] = {"X=Y"};
    options.configure_definitions = definitions; options.configure_definition_count = 1U;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
        &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture.commands == 0U);
    options.configure_definition_count = 0U;
    request.test_expression = NULL;
    CHECK(UmiBuildExecuteWithOptions(&profile, NULL, UMI_BUILD_COMPILE_TEST,
        &request, &options, report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(fixture.commands == 0U);
    free(report);
    return 0;
}
