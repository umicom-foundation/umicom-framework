/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_build_preflight.c
 *
 * PURPOSE:
 *   Prove complete metadata refusal precedes cleanup, log creation and process observers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/build_execution.h"
#include "umicom/toolchain/cmake_cache.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define SOURCE "C:/fixture/source"
#define BINARY "C:/fixture/source/build"
#define LOG_PATH "C:/fixture/evidence/output.log"
#else
#define SOURCE "/fixture/source"
#define BINARY "/fixture/source/build"
#define LOG_PATH "/fixture/evidence/output.log"
#endif
static unsigned cwd_calls, existence_calls, cache_reads, removals, log_creates, commands, executions;
static UmiStatus FixtureCwd(char *out, size_t capacity)
{
    ++cwd_calls;
    return umi_path_copy(out, capacity, SOURCE);
}
static int FixtureExists(const char *name) { (void)name; ++existence_calls; return 1; }
static UmiStatus FixtureCache(const char *name, UmiCMakeCache *cache)
{
    (void)name; ++cache_reads; memset(cache, 0, sizeof *cache);
    cache->present = UMI_CMAKE_CACHE_SOURCE | UMI_CMAKE_CACHE_BUILD;
    (void)snprintf(cache->source_directory, sizeof cache->source_directory, "%s", SOURCE);
    (void)snprintf(cache->build_directory, sizeof cache->build_directory, "%s", BINARY);
    return UMI_STATUS_OK;
}
static UmiStatus FixtureRemove(const char *name)
{
    ++removals;
    return umi_path_equal(name, BINARY) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus FixtureLog(const char *name, UmiOutputFile **out)
{
    (void)name; ++log_creates; *out = NULL; return UMI_STATUS_IO_ERROR;
}
static void Observe(UmiBuildAction action, const UmiProcessRequest *request, void *context)
{
    (void)action; (void)request; (void)context; ++commands;
}
static UmiStatus Execute(const UmiProcessRequest *request, UmiProcessOutputObserver output,
    void *output_context, UmiProcessResult *result, void *context)
{
    (void)request; (void)output; (void)output_context; (void)context; ++executions;
    memset(result, 0, sizeof *result); result->launched = 1; result->exit_code = 0;
    return UMI_STATUS_OK;
}
/* Only observable side-effect boundaries are replaced. The real execution,
 * logging and linked shared preflight code must reject before reaching them.
 * This fixture cannot remove a directory, create a log or launch a child. */
#define umi_fs_current_directory FixtureCwd
#define umi_fs_exists FixtureExists
#define umi_fs_remove_tree FixtureRemove
#define UmiCMakeCacheRead FixtureCache
#define UmiOutputFileCreate FixtureLog
#include "../../src/toolchain/build.c"
#include "../../src/toolchain/build_logging.c"
#undef UmiOutputFileCreate
#undef UmiCMakeCacheRead
#undef umi_fs_remove_tree
#undef umi_fs_exists
#undef umi_fs_current_directory
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s line %d: %s\n", mode, __LINE__, #x); failed = 1; goto finish; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *cases[] = {"valid", "preview", "preview-missing", "preview-undeclared",
        "missing-cmake", "missing-ctest", "cmake-found", "ctest-failed", "empty-cmake",
        "empty-ctest", "kind-cmake", "kind-ctest", "state-cmake", "state-ctest",
        "unterminated-cmake", "unterminated-ctest", "utf8-cmake", "utf8-ctest",
        "unicode-cmake", "unicode-ctest",
        "profile-size", "profile-version", "profile-count", "null-profile", "window-mode",
        "environment-count", "environment-name", "environment-value", "environment-duplicate",
        "late-malformed", "clean-bad-environment", "run-no-tools", "command-no-tools",
        "test-only", "configure-only", "compile-only", "install-only", "package-only",
        "make", "build-test", "clean-only", "invalid-options", "invalid-request",
        "unrelated-slot", "null-environment"};
    int known = 0, failed = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(mode, cases[i])) known = 1;
    if (!known) return 2;
    static UmiToolchainProfile profile;
    static UmiEnvironmentPlan environment, before_environment;
    umi_toolchain_profile_init(&profile); umi_environment_plan_init(&environment);
    UmiToolInfo *cmake = &profile.tools[UMI_TOOL_CMAKE], *ctest = &profile.tools[UMI_TOOL_CTEST];
    cmake->state = ctest->state = UMI_TOOL_VALIDATED;
    strcpy(cmake->path, "fixture-cmake"); strcpy(ctest->path, "fixture-ctest");
    UmiBuildRequest request; umi_build_request_init(&request);
    request.source_root = SOURCE; request.build_directory = BINARY; request.clean = 1;
    request.target = "fixture"; request.test_expression = "^fixture$";
    UmiBuildAction action = UMI_BUILD_DELIVER;
    UmiBuildExecutionOptions options = {0};
    options.command = Observe; options.executor = Execute;
    const UmiToolchainProfile *selected_profile = &profile;
    UmiEnvironmentPlan *selected_environment = &environment;
    UmiStatus expected = UMI_STATUS_OK;
    unsigned phases = 5U;
    if (!strcmp(mode, "preview")) options.preview = 1;
    else if (!strcmp(mode, "preview-missing")) {
        options.preview = 1; cmake->state = ctest->state = UMI_TOOL_MISSING;
        cmake->path[0] = ctest->path[0] = '\0';
    } else if (!strcmp(mode, "preview-undeclared")) { options.preview = 1; profile.tool_count = 0U; }
    else if (!strcmp(mode, "missing-cmake")) { profile.tool_count = UMI_TOOL_CMAKE; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "missing-ctest")) { profile.tool_count = UMI_TOOL_CTEST; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "cmake-found")) { cmake->state = UMI_TOOL_FOUND; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "ctest-failed")) { ctest->state = UMI_TOOL_FAILED; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "empty-cmake")) { cmake->path[0] = '\0'; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "empty-ctest")) { ctest->path[0] = '\0'; expected = UMI_STATUS_NOT_FOUND; }
    else if (!strcmp(mode, "kind-cmake")) { cmake->kind = UMI_TOOL_GIT; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "kind-ctest")) { ctest->kind = UMI_TOOL_GIT; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "state-cmake")) { cmake->state = (UmiToolState)99; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "state-ctest")) { ctest->state = (UmiToolState)-1; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "unterminated-cmake")) {
        memset(cmake->path, 'x', sizeof cmake->path); options.preview = 1; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "unterminated-ctest")) {
        memset(ctest->path, 'x', sizeof ctest->path); expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(mode, "utf8-cmake")) { strcpy(cmake->path, "\xff"); expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "utf8-ctest")) { strcpy(ctest->path, "\xff"); options.preview = 1; expected = UMI_STATUS_INVALID_ARGUMENT; }
    /* Positive multibyte cases complement malformed-octet refusal. The fake
     * executor observes admission only; this does not claim the path exists. */
    else if (!strcmp(mode, "unicode-cmake")) strcpy(cmake->path, "caf\xc3\xa9/cmake");
    else if (!strcmp(mode, "unicode-ctest")) strcpy(ctest->path, "caf\xc3\xa9/ctest");
    else if (!strcmp(mode, "profile-size")) { --profile.structure_size; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "profile-version")) { ++profile.api_version; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "profile-count")) { profile.tool_count = UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY + 1U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "null-profile")) { selected_profile = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "window-mode")) { request.window_mode = (UmiProcessWindowMode)99; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "environment-count")) { environment.count = UMI_ENVIRONMENT_PLAN_MAX + 1U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(mode, "environment-name")) {
        environment.count = 1U; memset(environment.entries[0].name, 'x', sizeof environment.entries[0].name); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "environment-value")) {
        environment.count = 1U; strcpy(environment.entries[0].name, "A");
        memset(environment.entries[0].value, 'x', sizeof environment.entries[0].value); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "environment-duplicate") || !strcmp(mode, "late-malformed")) {
        environment.count = 2U; strcpy(environment.entries[0].name, "A"); strcpy(environment.entries[1].name, "A");
        expected = UMI_STATUS_ALREADY_EXISTS;
        if (!strcmp(mode, "late-malformed")) {
            environment.count = 3U; strcpy(environment.entries[2].name, "B"); strcpy(environment.entries[2].value, "\n");
            expected = UMI_STATUS_INVALID_ARGUMENT;
        }
    } else if (!strcmp(mode, "clean-bad-environment")) {
        action = UMI_BUILD_CLEAN; environment.count = 1U; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(mode, "run-no-tools") || !strcmp(mode, "command-no-tools")) {
        action = !strcmp(mode, "run-no-tools") ? UMI_BUILD_RUN : UMI_BUILD_COMMAND;
        request.clean = 0; request.executable = "fixture-program"; profile.tool_count = 0U; phases = 1U;
    } else if (!strcmp(mode, "test-only")) {
        action = UMI_BUILD_TEST; request.clean = 0; cmake->state = UMI_TOOL_MISSING; phases = 1U;
    } else if (!strcmp(mode, "configure-only") || !strcmp(mode, "compile-only") ||
        !strcmp(mode, "install-only") || !strcmp(mode, "package-only")) {
        action = !strcmp(mode, "configure-only") ? UMI_BUILD_CONFIGURE :
            !strcmp(mode, "compile-only") ? UMI_BUILD_COMPILE :
            !strcmp(mode, "install-only") ? UMI_BUILD_INSTALL : UMI_BUILD_PACKAGE;
        ctest->state = UMI_TOOL_MISSING; request.clean = 0; phases = 1U;
    } else if (!strcmp(mode, "make")) { action = UMI_BUILD_MAKE; phases = 3U; }
    else if (!strcmp(mode, "build-test")) { action = UMI_BUILD_COMPILE_TEST; request.clean = 0; phases = 2U; }
    else if (!strcmp(mode, "clean-only")) { action = UMI_BUILD_CLEAN; profile.tool_count = 0U; phases = 0U; }
    else if (!strcmp(mode, "invalid-options")) { options.configure_definition_count = 1U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "invalid-request")) { request.source_root = NULL; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (!strcmp(mode, "unrelated-slot")) {
        profile.tools[UMI_TOOL_GCC].kind = UMI_TOOL_GIT;
        memset(profile.tools[UMI_TOOL_GCC].path, 'x', sizeof profile.tools[UMI_TOOL_GCC].path);
    } else if (!strcmp(mode, "null-environment")) selected_environment = NULL;
    before_environment = environment;
    UmiBuildReport *report = calloc(1U, sizeof *report);
    CHECK(report != NULL);
    char message[256] = {0};
    CHECK(UmiBuildExecutionPreflight(selected_profile, selected_environment, action,
        &request, &options, message, sizeof message) == expected);
    CHECK(message[0] && !memcmp(&environment, &before_environment, sizeof environment));
    CHECK(cwd_calls == 0U && existence_calls == 0U && cache_reads == 0U &&
        removals == 0U && log_creates == 0U && commands == 0U && executions == 0U);
    CHECK(UmiBuildExecuteWithOptions(selected_profile, selected_environment, action,
        &request, &options, report) == expected);
    CHECK(report->last_status == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(cwd_calls == 0U && existence_calls == 0U && cache_reads == 0U &&
            removals == 0U && commands == 0U && executions == 0U);
        CHECK(report->configure_exit_code == -1 && report->build_exit_code == -1 &&
            report->test_exit_code == -1 && report->install_exit_code == -1 && report->package_exit_code == -1);
    } else {
        CHECK(commands == phases && executions == (options.preview ? 0U : phases));
        CHECK(removals == ((!options.preview && request.clean) ? 1U : 0U));
    }
    cwd_calls = existence_calls = cache_reads = removals = commands = executions = 0U;
    UmiBuildExecutionLog log;
    UmiStatus logged = UmiBuildExecuteLogged(selected_profile, selected_environment, action,
        &request, &options, LOG_PATH, &log, report);
    if (expected != UMI_STATUS_OK) {
        CHECK(logged == expected && !log.created && log.execution_status == expected);
        CHECK(!cwd_calls && !existence_calls && !cache_reads && !removals && !log_creates && !commands && !executions);
    } else if (options.preview) {
        CHECK(logged == UMI_STATUS_OK && !log.created && !log_creates && !removals && !executions);
        CHECK(commands == phases);
    } else {
        CHECK(logged == UMI_STATUS_IO_ERROR && !log.created && log_creates == 1U);
        CHECK(!removals && !cache_reads && !commands && !executions);
    }
finish:
    free(report);
    return failed;
}
