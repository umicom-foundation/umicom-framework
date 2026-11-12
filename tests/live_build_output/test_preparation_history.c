/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_build_output/test_preparation_history.c
 *
 * PURPOSE:
 *   Verify admitted preparation failures share completed history without launching a child process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Rename public symbols before their declarations. The private fixture compiles
 * the actual runner while linking its real history, result and parser services.
 * Only external preparation boundaries and the process port are controlled. */
#define umi_build_runner_create FixtureRunnerCreate
#define umi_build_runner_destroy FixtureRunnerDestroy
#define umi_build_runner_set_profile FixtureRunnerSetProfile
#define umi_build_runner_run FixtureRunnerRun
#define umi_build_runner_next_operation_id FixtureRunnerNextId
#define UmiBuildRunnerRunObserved FixtureRunnerObserved
#define UmiBuildRunnerRunAutomaticTests FixtureRunnerAutomatic
#include "umicom/build/runner.h"
#include "umicom/build/live_output.h"
#include "umicom/build/launch_plan.h"
#include "umicom/build/provider.h"
#include "umicom/diagnostics/compiler_stream.h"
#include "umicom/platform/process_environment.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); exit(1); \
} } while (0)

typedef enum FailurePoint {
    FAIL_NONE, FAIL_SOURCE, FAIL_BUILD_PATH, FAIL_INSTALL_PATH, FAIL_RUN_PATH,
    FAIL_LAUNCH, FAIL_PROVIDER, FAIL_DIRECTORY, FAIL_ENV_CREATE, FAIL_ENV_READ,
    FAIL_PARSER
} FailurePoint;
static FailurePoint failure;
static unsigned path_calls, process_calls, observer_calls, environment_created;
static unsigned environment_destroyed, parser_created, parser_destroyed;
static UmiStatus process_status;
static int process_exit;
static UmiBuildResult *observed_result;

static UmiStatus ResolveSource(const UmiBuildProfile *profile, char *out, size_t capacity)
{
    if (failure == FAIL_SOURCE) return UMI_STATUS_IO_ERROR;
    return UmiBuildProfileSourceDirectory(profile, out, capacity);
}
static UmiStatus ResolvePath(const char *input, const char *base, char *out, size_t capacity)
{
    ++path_calls;
    if ((failure == FAIL_BUILD_PATH && path_calls == 1U) ||
        (failure == FAIL_INSTALL_PATH && path_calls == 2U) ||
        (failure == FAIL_RUN_PATH && path_calls == 3U)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_path_absolute(input, base, out, capacity);
}
static UmiStatus PrepareLaunch(const UmiBuildProfile *profile, const char *base,
    UmiBuildLaunchPlan *out)
{
    if (failure == FAIL_LAUNCH) return UMI_STATUS_INVALID_STATE;
    return UmiBuildLaunchPlanCreate(profile, base, out);
}
static UmiStatus PrepareCommand(const UmiBuildProvider *provider,
    const UmiBuildProfile *profile, UmiBuildPhase phase, UmiBuildCommand *out)
{
    CHECK(provider != NULL && profile != NULL);
    (void)phase;
    if (failure == FAIL_PROVIDER) return UMI_STATUS_NOT_IMPLEMENTED;
    /* Command generation is deliberately controlled: automatic selection must
     * not read a project's test index, and no fixture may reach a real tool. */
    umi_build_command_init(out, "controlled-child");
    return UMI_STATUS_OK;
}
static int PrepareDirectory(UmiBuildCommand *command, const char *directory)
{
    if (failure == FAIL_DIRECTORY) return 0;
    return umi_build_command_set_working_directory(command, directory);
}
static UmiStatus PrepareEnvironment(const char *definitions, const char *directory,
    UmiProcessEnvironmentPlan **out)
{
    if (failure == FAIL_ENV_CREATE) { *out = NULL; return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = UmiProcessEnvironmentPlanCreate(definitions, directory, out);
    if (status == UMI_STATUS_OK) ++environment_created;
    return status;
}
static UmiStatus ReadEnvironment(const UmiProcessEnvironmentPlan *plan,
    const UmiEnvironmentVariable **variables, size_t *count)
{
    if (failure == FAIL_ENV_READ) return UMI_STATUS_INVALID_STATE;
    return UmiProcessEnvironmentPlanRead(plan, variables, count);
}
static void DestroyEnvironment(UmiProcessEnvironmentPlan *plan)
{
    if (plan != NULL) ++environment_destroyed;
    UmiProcessEnvironmentPlanDestroy(plan);
}
static UmiStatus PrepareParser(UmiCompilerDiagnosticObserver observer, void *context,
    UmiCompilerDiagnosticStream **out)
{
    if (failure == FAIL_PARSER) { *out = NULL; return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = UmiCompilerDiagnosticStreamCreate(observer, context, out);
    if (status == UMI_STATUS_OK) ++parser_created;
    return status;
}
static void DestroyParser(UmiCompilerDiagnosticStream *parser)
{
    if (parser != NULL) ++parser_destroyed;
    UmiCompilerDiagnosticStreamDestroy(parser);
}
static UmiStatus ExecuteControlled(const UmiProcessRequest *request, UmiProcessLifetime lifetime,
    UmiProcessResultObserver observer, UmiProcessOutputObserver raw, void *context,
    UmiProcessResult *out)
{
    static const char bytes[] = "controlled child observation\n";
    ++process_calls;
    CHECK(request != NULL && lifetime == UMI_PROCESS_LIFETIME_TREE);
    CHECK(strcmp(request->program, "controlled-child") == 0);
    CHECK(request->working_directory != NULL && request->capture_stdout && request->capture_stderr);
    CHECK(observer == NULL && raw != NULL);
    memset(out, 0, sizeof(*out));
    out->exit_code = process_exit;
    out->launched = process_exit >= 0;
    out->duration_ms = 17U;
    if (out->launched) {
        strcpy(out->output, bytes);
        raw(bytes, sizeof(bytes) - 1U, context);
    }
    return process_status;
}

#define UmiBuildProfileSourceDirectory ResolveSource
#define umi_path_absolute ResolvePath
#define UmiBuildLaunchPlanCreate PrepareLaunch
#define umi_build_provider_create_command PrepareCommand
#define umi_build_command_set_working_directory PrepareDirectory
#define UmiProcessEnvironmentPlanCreate PrepareEnvironment
#define UmiProcessEnvironmentPlanRead ReadEnvironment
#define UmiProcessEnvironmentPlanDestroy DestroyEnvironment
#define UmiCompilerDiagnosticStreamCreate PrepareParser
#define UmiCompilerDiagnosticStreamDestroy DestroyParser
#define UmiProcessExecuteWithLifetime ExecuteControlled
#include "../../src/build/runner.c"
#undef UmiBuildProfileSourceDirectory
#undef umi_path_absolute
#undef UmiBuildLaunchPlanCreate
#undef umi_build_provider_create_command
#undef umi_build_command_set_working_directory
#undef UmiProcessEnvironmentPlanCreate
#undef UmiProcessEnvironmentPlanRead
#undef UmiProcessEnvironmentPlanDestroy
#undef UmiCompilerDiagnosticStreamCreate
#undef UmiCompilerDiagnosticStreamDestroy
#undef UmiProcessExecuteWithLifetime

static void Observe(const char *bytes, size_t length, void *context)
{
    CHECK(context == observed_result && observed_result->state == UMI_BUILD_STATE_RUNNING);
    CHECK(bytes != NULL && length != 0U);
    ++observer_calls;
}
static UmiStatus Invoke(UmiBuildRunner *runner, const char *mode, UmiBuildPhase phase,
    UmiBuildResult *result)
{
    observed_result = result;
    if (strcmp(mode, "legacy") == 0) return FixtureRunnerRun(runner, phase, result);
    if (strcmp(mode, "automatic") == 0)
        return FixtureRunnerAutomatic(runner, Observe, result, result);
    return FixtureRunnerObserved(runner, phase, Observe, result, result);
}
static void ResetProbe(void)
{
    path_calls = process_calls = observer_calls = 0U;
    environment_created = environment_destroyed = parser_created = parser_destroyed = 0U;
    failure = FAIL_NONE; process_status = UMI_STATUS_OK; process_exit = 0;
}
static void CheckReleased(void)
{
    CHECK(environment_created == environment_destroyed);
    CHECK(parser_created == parser_destroyed);
}
static UmiStatus SelectFailure(const char *name)
{
    if (strcmp(name, "source") == 0) { failure = FAIL_SOURCE; return UMI_STATUS_IO_ERROR; }
    if (strcmp(name, "build-path") == 0) { failure = FAIL_BUILD_PATH; return UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(name, "install-path") == 0) { failure = FAIL_INSTALL_PATH; return UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(name, "run-path") == 0) { failure = FAIL_RUN_PATH; return UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(name, "launch") == 0) { failure = FAIL_LAUNCH; return UMI_STATUS_INVALID_STATE; }
    if (strcmp(name, "provider") == 0) { failure = FAIL_PROVIDER; return UMI_STATUS_NOT_IMPLEMENTED; }
    if (strcmp(name, "directory") == 0) { failure = FAIL_DIRECTORY; return UMI_STATUS_CAPACITY_EXCEEDED; }
    if (strcmp(name, "environment-create") == 0) { failure = FAIL_ENV_CREATE; return UMI_STATUS_OUT_OF_MEMORY; }
    if (strcmp(name, "environment-read") == 0) { failure = FAIL_ENV_READ; return UMI_STATUS_INVALID_STATE; }
    if (strcmp(name, "parser") == 0) { failure = FAIL_PARSER; return UMI_STATUS_OUT_OF_MEMORY; }
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *mode = argv[1], *name = argv[2];
    CHECK(strcmp(mode, "legacy") == 0 || strcmp(mode, "observed") == 0 || strcmp(mode, "automatic") == 0);
    static const char *const known[] = {
        "source",
        "build-path",
        "install-path",
        "run-path",
        "launch",
        "provider",
        "directory",
        "environment-create",
        "environment-read",
        "parser",
        "success",
        "nonzero",
        "spawn-error",
        "cancel",
        "timeout",
        "no-history-success",
        "no-history-failure",
        "invalid",
        "exhausted",
        "rolling",
        "shared"
    };
    int recognised = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(name, known[i]) == 0) recognised = 1;
    if (!recognised || (strcmp(mode, "automatic") == 0 && strcmp(name, "launch") == 0)) return 2;
    UmiBuildRunnerConfig config = {0};
    UmiBuildRunner *runner = NULL, *second = NULL;
    UmiBuildHistory *history = NULL;
    UmiBuildResult *result = NULL, *retained = NULL, *seed = NULL;
    CHECK(umi_build_result_create(&result) == UMI_STATUS_OK);
    CHECK(umi_build_result_create(&retained) == UMI_STATUS_OK);
    CHECK(umi_build_result_create(&seed) == UMI_STATUS_OK);
    CHECK(umi_build_history_create(2U, &history) == UMI_STATUS_OK);
    umi_build_profile_init(&config.profile);
#ifdef _WIN32
    strcpy(config.profile.source_directory, "C:/controlled-project");
#else
    strcpy(config.profile.source_directory, "/controlled-project");
#endif
    strcpy(config.profile.run_program, "./controlled-child");
    config.history = strstr(name, "no-history") != NULL ? NULL : history;
    CHECK(FixtureRunnerCreate(&config, &runner) == UMI_STATUS_OK);
    umi_build_result_init(seed, 40U, UMI_BUILD_PHASE_BUILD, "previous");
    umi_build_result_finish(seed, UMI_STATUS_OK, 0, 5U);
    CHECK(umi_build_history_append(history, seed) == UMI_STATUS_OK);
    ResetProbe();
    UmiBuildPhase phase = strcmp(name, "launch") == 0 ? UMI_BUILD_PHASE_RUN : UMI_BUILD_PHASE_TEST;
    UmiStatus expected = SelectFailure(name);
    if (strcmp(name, "no-history-failure") == 0) { failure = FAIL_PARSER; expected = UMI_STATUS_OUT_OF_MEMORY; }
    if (strcmp(name, "nonzero") == 0) { process_exit = 9; process_status = UMI_STATUS_IO_ERROR; expected = process_status; }
    if (strcmp(name, "spawn-error") == 0) { process_exit = -1; process_status = UMI_STATUS_IO_ERROR; expected = process_status; }
    if (strcmp(name, "cancel") == 0) { process_exit = -1; process_status = UMI_STATUS_CANCELLED; expected = process_status; }
    if (strcmp(name, "timeout") == 0) { process_exit = -1; process_status = UMI_STATUS_TIMEOUT; expected = process_status; }
    if (strcmp(name, "invalid") == 0) {
        memcpy(result, seed, sizeof(*result));
        CHECK(Invoke(NULL, mode, phase, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(result, seed, sizeof(*result)) == 0);
        CHECK(Invoke(runner, mode, phase, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_history_count(history) == 1U && process_calls == 0U);
    } else if (strcmp(name, "exhausted") == 0) {
        seed->operation_id = UINT64_MAX;
        CHECK(umi_build_history_append(history, seed) == UMI_STATUS_OK);
        CHECK(Invoke(runner, mode, phase, result) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(result->operation_id == 0U && result->state == UMI_BUILD_STATE_FAILED);
        CHECK(umi_build_history_count(history) == 2U && process_calls == 0U);
        CHECK(umi_build_history_latest(history, retained) == UMI_STATUS_OK);
        CHECK(memcmp(seed, retained, sizeof(*seed)) == 0);
    } else {
        CHECK(Invoke(runner, mode, phase, result) == expected);
        CHECK(result->status == expected);
        CHECK(result->operation_id == (config.history != NULL ? 41U : 1U));
        CHECK(strcmp(result->profile_id, config.profile.profile_id) == 0);
        CHECK(result->phase == phase);
        if (failure != FAIL_NONE) {
            CHECK(process_calls == 0U && observer_calls == 0U);
            CHECK(result->exit_code == -1 && result->duration_ms == 0U);
            CHECK(result->output[0] == '\0' && result->diagnostics.count == 0U);
            CHECK(result->state == UMI_BUILD_STATE_FAILED);
        } else {
            CHECK(process_calls == 1U && result->duration_ms == 17U);
            CHECK(result->exit_code == process_exit);
            CHECK(observer_calls == ((strcmp(mode, "legacy") != 0 && process_exit >= 0) ? 1U : 0U));
            CHECK(result->state == (expected == UMI_STATUS_OK ? UMI_BUILD_STATE_SUCCEEDED :
                expected == UMI_STATUS_CANCELLED ? UMI_BUILD_STATE_CANCELLED :
                expected == UMI_STATUS_TIMEOUT ? UMI_BUILD_STATE_TIMED_OUT : UMI_BUILD_STATE_FAILED));
        }
        CHECK(umi_build_history_latest(history, retained) == UMI_STATUS_OK);
        CHECK(memcmp(retained, config.history != NULL ? result : seed, sizeof(*result)) == 0);
        CHECK(umi_build_history_count(history) == (config.history != NULL ? 2U : 1U));
        CheckReleased();
        /* A full ring evicts only the oldest entry. Independent runners reserve
         * from the same history, so a preparation failure must not reuse an ID. */
        if (strcmp(name, "rolling") == 0 || strcmp(name, "shared") == 0) {
            memcpy(seed, result, sizeof(*seed));
            ResetProbe(); failure = FAIL_PROVIDER;
            if (strcmp(name, "shared") == 0) CHECK(FixtureRunnerCreate(&config, &second) == UMI_STATUS_OK);
            CHECK(Invoke(second != NULL ? second : runner, mode, phase, result) == UMI_STATUS_NOT_IMPLEMENTED);
            CHECK(result->operation_id == 42U && process_calls == 0U);
            CHECK(umi_build_history_count(history) == 2U);
            CHECK(umi_build_history_at(history, 0U, retained) == UMI_STATUS_OK);
            CHECK(memcmp(retained, seed, sizeof(*seed)) == 0);
            CHECK(umi_build_history_latest(history, retained) == UMI_STATUS_OK);
            CHECK(memcmp(retained, result, sizeof(*result)) == 0);
        }
    }
    CheckReleased();
    FixtureRunnerDestroy(second); FixtureRunnerDestroy(runner);
    umi_build_history_destroy(history);
    umi_build_result_destroy(result); umi_build_result_destroy(retained); umi_build_result_destroy(seed);
    return 0;
}
