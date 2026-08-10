/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/build.c
 *
 * PURPOSE:
 *   Implement native CMake, Ninja, CTest, application launch, stale-cache
 *   recovery, local user-preset generation, and prepared interactive shells.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/build.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"

static UmiStatus umi_build_run_process(const UmiToolInfo *tool,
                                       const char *const *arguments,
                                       size_t argument_count,
                                       const char *working_directory,
                                       UmiEnvironmentPlan *environment,
                                       int *out_exit_code,
                                       char *out_text,
                                       size_t capacity)
{
    UmiProcessRequest process_request;
    UmiProcessResult result;
    UmiStatus status;

    if (tool == NULL || tool->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(&process_request, 0, sizeof(process_request));
    process_request.program = tool->path;
    process_request.arguments = arguments;
    process_request.argument_count = argument_count;
    process_request.working_directory = working_directory;
    process_request.environment = environment != NULL
        ? umi_environment_plan_variables(environment)
        : NULL;
    process_request.environment_count = environment != NULL
        ? environment->count
        : 0U;
    process_request.capture_stdout = 1;
    process_request.capture_stderr = 1;
    status = umi_process_execute(&process_request, &result);
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }
    if (out_text != NULL && capacity > 0U) {
        (void)snprintf(out_text, capacity, "%s", result.output);
    }
    return status;
}

static UmiStatus umi_build_configure_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *cmake = umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE);
    const char *arguments[12];
    size_t count = 0U;

    if (request->preset != NULL && request->preset[0] != '\0') {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } else {
        arguments[count++] = "-S";
        arguments[count++] = request->source_root;
        arguments[count++] = "-B";
        arguments[count++] = request->build_directory;
        arguments[count++] = "-G";
        arguments[count++] = "Ninja";
        arguments[count++] = "-DCMAKE_BUILD_TYPE=Debug";
        arguments[count++] = "-DBUILD_TESTING=ON";
    }
    return umi_build_run_process(cmake,
                                 arguments,
                                 count,
                                 request->source_root,
                                 environment,
                                 &report->configure_exit_code,
                                 report->last_output,
                                 sizeof(report->last_output));
}

static UmiStatus umi_build_compile_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *cmake = umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE);
    const char *arguments[10];
    char jobs[32];
    size_t count = 0U;

    arguments[count++] = "--build";
    if (request->preset != NULL && request->preset[0] != '\0') {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } else {
        arguments[count++] = request->build_directory;
    }
    if (request->target != NULL && request->target[0] != '\0') {
        arguments[count++] = "--target";
        arguments[count++] = request->target;
    }
    if (request->jobs > 0) {
        (void)snprintf(jobs, sizeof(jobs), "%d", request->jobs);
        arguments[count++] = "--parallel";
        arguments[count++] = jobs;
    }
    return umi_build_run_process(cmake,
                                 arguments,
                                 count,
                                 request->source_root,
                                 environment,
                                 &report->build_exit_code,
                                 report->last_output,
                                 sizeof(report->last_output));
}

static UmiStatus umi_build_test_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *ctest = umi_toolchain_profile_tool(profile, UMI_TOOL_CTEST);
    const char *arguments[6];
    size_t count = 0U;

    if (request->preset != NULL && request->preset[0] != '\0') {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } else {
        arguments[count++] = "--test-dir";
        arguments[count++] = request->build_directory;
        arguments[count++] = "--output-on-failure";
    }
    return umi_build_run_process(ctest,
                                 arguments,
                                 count,
                                 request->source_root,
                                 environment,
                                 &report->test_exit_code,
                                 report->last_output,
                                 sizeof(report->last_output));
}

static UmiStatus umi_build_run_internal(
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    UmiProcessRequest process_request;
    UmiProcessResult result;
    UmiStatus status;

    if (request->executable == NULL || request->executable[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&process_request, 0, sizeof(process_request));
    process_request.program = request->executable;
    process_request.arguments = request->run_arguments;
    process_request.argument_count = request->run_argument_count;
    process_request.working_directory = request->source_root;
    process_request.environment = environment != NULL
        ? umi_environment_plan_variables(environment)
        : NULL;
    process_request.environment_count = environment != NULL
        ? environment->count
        : 0U;
    process_request.capture_stdout = 1;
    process_request.capture_stderr = 1;
    status = umi_process_execute(&process_request, &result);
    report->run_exit_code = result.exit_code;
    (void)snprintf(report->last_output,
                   sizeof(report->last_output),
                   "%s",
                   result.output);
    return status;
}

UmiStatus umi_build_execute(const UmiToolchainProfile *profile,
                            UmiEnvironmentPlan *environment,
                            UmiBuildAction action,
                            const UmiBuildRequest *request,
                            UmiBuildReport *out_report)
{
    UmiBuildReport local_report;
    UmiBuildReport *report = out_report != NULL ? out_report : &local_report;
    UmiStatus status;

    if (profile == NULL || request == NULL || request->source_root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(report, 0, sizeof(*report));
    report->configure_exit_code = -1;
    report->build_exit_code = -1;
    report->test_exit_code = -1;
    report->run_exit_code = -1;

    if (request->clean && request->build_directory != NULL) {
        (void)umi_fs_remove_tree(request->build_directory);
    }

    if (action == UMI_BUILD_CONFIGURE || action == UMI_BUILD_MAKE) {
        status = umi_build_configure_internal(profile,
                                              environment,
                                              request,
                                              report);
        if (status != UMI_STATUS_OK) return status;
    }
    if (action == UMI_BUILD_COMPILE || action == UMI_BUILD_MAKE) {
        status = umi_build_compile_internal(profile,
                                            environment,
                                            request,
                                            report);
        if (status != UMI_STATUS_OK) return status;
    }
    if (action == UMI_BUILD_TEST || action == UMI_BUILD_MAKE) {
        status = umi_build_test_internal(profile,
                                         environment,
                                         request,
                                         report);
        if (status != UMI_STATUS_OK) return status;
    }
    if (action == UMI_BUILD_RUN) {
        return umi_build_run_internal(environment, request, report);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_build_repair_cache(const UmiToolchainProfile *profile,
                                 const char *build_directory,
                                 char *out_recovery_path,
                                 size_t capacity,
                                 int dry_run)
{
    char cache_path[UMI_PATH_CAPACITY];
    char *cache = NULL;
    const UmiToolInfo *compiler;
    char marker[UMI_TOOL_PATH_CAPACITY + 64U];
    char recovery[UMI_PATH_CAPACITY];
    char timestamp[64];
    time_t now;
    struct tm *time_info;
    UmiStatus status;

    if (profile == NULL || build_directory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_join(cache_path,
                         sizeof(cache_path),
                         build_directory,
                         "CMakeCache.txt");
    if (status != UMI_STATUS_OK || !umi_fs_is_file(cache_path)) {
        return UMI_STATUS_OK;
    }
    compiler = umi_toolchain_profile_tool(profile, UMI_TOOL_CLANG);
    if (compiler == NULL || compiler->path[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_fs_read_text(cache_path, &cache, NULL);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(marker,
                   sizeof(marker),
                   "CMAKE_C_COMPILER:FILEPATH=%s",
                   compiler->path);
    if (strstr(cache, marker) != NULL) {
        umi_fs_free_text(cache);
        return UMI_STATUS_OK;
    }
    umi_fs_free_text(cache);

    now = time(NULL);
    time_info = localtime(&now);
    if (time_info == NULL ||
        strftime(timestamp,
                 sizeof(timestamp),
                 "%Y%m%d-%H%M%S",
                 time_info) == 0U) {
        (void)snprintf(timestamp, sizeof(timestamp), "unknown-time");
    }
    (void)snprintf(recovery,
                   sizeof(recovery),
                   "%s.recovery-%s",
                   build_directory,
                   timestamp);
    if (out_recovery_path != NULL && capacity > 0U) {
        (void)snprintf(out_recovery_path, capacity, "%s", recovery);
    }
    return dry_run
        ? UMI_STATUS_OK
        : umi_fs_rename(build_directory, recovery);
}

UmiStatus umi_build_write_user_presets(const UmiToolchainProfile *profile,
                                       const char *project_root,
                                       const char *path)
{
    const UmiToolInfo *compiler;
    const UmiToolInfo *ninja;
    const UmiToolInfo *pkg_config;
    char text[16384];
    int written;

    if (profile == NULL || project_root == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    compiler = umi_toolchain_profile_tool(profile, UMI_TOOL_CLANG);
    ninja = umi_toolchain_profile_tool(profile, UMI_TOOL_NINJA);
    pkg_config = umi_toolchain_profile_tool(profile, UMI_TOOL_PKG_CONFIG);
    if (compiler == NULL || ninja == NULL || pkg_config == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    written = snprintf(
        text,
        sizeof(text),
        "{\n"
        "  \"version\": 6,\n"
        "  \"configurePresets\": [\n"
        "    {\n"
        "      \"name\": \"umicom-auto-debug\",\n"
        "      \"displayName\": \"Umicom auto-discovered debug\",\n"
        "      \"generator\": \"Ninja\",\n"
        "      \"binaryDir\": \"${sourceDir}/build/umicom-auto-debug\",\n"
        "      \"cacheVariables\": {\n"
        "        \"CMAKE_BUILD_TYPE\": \"Debug\",\n"
        "        \"CMAKE_C_COMPILER\": \"%s\",\n"
        "        \"CMAKE_MAKE_PROGRAM\": \"%s\",\n"
        "        \"PKG_CONFIG_EXECUTABLE\": \"%s\",\n"
        "        \"CMAKE_PREFIX_PATH\": \"%s\",\n"
        "        \"BUILD_TESTING\": \"ON\"\n"
        "      }\n"
        "    }\n"
        "  ],\n"
        "  \"buildPresets\": [\n"
        "    { \"name\": \"umicom-auto-debug\", "
        "\"configurePreset\": \"umicom-auto-debug\" }\n"
        "  ],\n"
        "  \"testPresets\": [\n"
        "    { \"name\": \"umicom-auto-debug\", "
        "\"configurePreset\": \"umicom-auto-debug\", "
        "\"output\": { \"outputOnFailure\": true } }\n"
        "  ]\n"
        "}\n",
        compiler->path,
        ninja->path,
        pkg_config->path,
        profile->prefix_directory
    );
    (void)project_root;
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_fs_write_text(path, text);
}

UmiStatus umi_build_open_shell(const UmiToolchainProfile *profile,
                               UmiEnvironmentPlan *environment,
                               const char *working_directory,
                               const char *shell_program)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    const char *program = shell_program;
    (void)profile;

    if (environment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    if (program == NULL || program[0] == '\0') {
        program = "powershell.exe";
    }
#else
    if (program == NULL || program[0] == '\0') {
        program = getenv("SHELL");
        if (program == NULL || program[0] == '\0') {
            program = "/bin/sh";
        }
    }
#endif
    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.working_directory = working_directory;
    request.environment = umi_environment_plan_variables(environment);
    request.environment_count = environment->count;
    return umi_process_execute(&request, &result);
}
