/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/build.c
 *
 * PURPOSE:
 *   Implement native CMake/CTest build execution, installation, packaging,
 *   launch, cancellation, timeout propagation, stale-cache recovery, local
 *   preset generation and prepared interactive shells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/build.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "umicom/platform/filesystem.h"

/* Provide the build message operation used by this module and its client applications. */
static void umi_build_message(char *out_message,
                              size_t capacity,
                              const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message == NULL || capacity == 0U) {
        return;
    }
    (void)snprintf(out_message, capacity, "%s", text != NULL ? text : "");
}

/*
 * Initialise build request from caller-provided values so later operations receive a known
 * state.
 */
void umi_build_request_init(UmiBuildRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) {
        return;
    }

    (void)memset(request, 0, sizeof(*request));
    request->jobs = 0;
    request->window_mode = UMI_PROCESS_WINDOW_HIDDEN;
}

/*
 * Initialise build report from caller-provided values so later operations receive a known
 * state.
 */
void umi_build_report_init(UmiBuildReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) {
        return;
    }

    (void)memset(report, 0, sizeof(*report));
    report->configure_exit_code = -1;
    report->build_exit_code = -1;
    report->test_exit_code = -1;
    report->run_exit_code = -1;
    report->install_exit_code = -1;
    report->package_exit_code = -1;
    report->last_action = (UmiBuildAction)0;
    report->last_status = UMI_STATUS_OK;
}

/* Provide the build action text operation used by this module and its client applications. */
const char *umi_build_action_text(UmiBuildAction action)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_BUILD_CONFIGURE: return "configure";
        case UMI_BUILD_COMPILE: return "build";
        case UMI_BUILD_TEST: return "test";
        case UMI_BUILD_RUN: return "run";
        case UMI_BUILD_MAKE: return "make";
        case UMI_BUILD_CLEAN: return "clean";
        case UMI_BUILD_INSTALL: return "install";
        case UMI_BUILD_PACKAGE: return "package";
        case UMI_BUILD_DELIVER: return "deliver";
        case UMI_BUILD_COMMAND: return "command";
        default: return "unknown";
    }
}

/* Provide the build has text operation used by this module and its client applications. */
static int umi_build_has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

/* Check that build request satisfies its contract before another service relies on it. */
UmiStatus umi_build_request_validate(UmiBuildAction action,
                                     const UmiBuildRequest *request,
                                     char *out_message,
                                     size_t message_capacity)
{
    const int uses_preset =
        request != NULL && umi_build_has_text(request->preset);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) {
        umi_build_message(out_message, message_capacity,
                          "Build request is required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_build_has_text(request->source_root)) {
        umi_build_message(out_message, message_capacity,
                          "Source root is required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->jobs < 0) {
        umi_build_message(out_message, message_capacity,
                          "Parallel job count cannot be negative.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->run_argument_count > UMI_PROCESS_MAX_ARGUMENTS) {
        umi_build_message(out_message, message_capacity,
                          "Run argument count exceeds the process limit.");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (action) {
        case UMI_BUILD_CONFIGURE:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!uses_preset &&
                !umi_build_has_text(request->build_directory)) {
                umi_build_message(
                    out_message,
                    message_capacity,
                    "Configure requires a build directory when no preset is used.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;

        case UMI_BUILD_COMPILE:
        case UMI_BUILD_TEST:
        case UMI_BUILD_PACKAGE:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!uses_preset &&
                !umi_build_has_text(request->build_directory)) {
                umi_build_message(
                    out_message,
                    message_capacity,
                    "Build, test and package require a build directory when no preset is used.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;

        case UMI_BUILD_RUN:
        case UMI_BUILD_COMMAND:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!umi_build_has_text(request->executable)) {
                umi_build_message(out_message, message_capacity,
                                  "Run requires an executable.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;

        case UMI_BUILD_CLEAN:
        case UMI_BUILD_INSTALL:
            /* Create this optional product surface only when its build option is enabled. */
            if (!umi_build_has_text(request->build_directory)) {
                umi_build_message(
                    out_message,
                    message_capacity,
                    "Clean and install require an explicit build directory.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;

        case UMI_BUILD_MAKE:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!uses_preset &&
                !umi_build_has_text(request->build_directory)) {
                umi_build_message(
                    out_message,
                    message_capacity,
                    "Make requires a build directory when no preset is used.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;

        case UMI_BUILD_DELIVER:
            /*
             * Delivery includes an install phase. CMake --install operates on
             * an explicit binary directory even when configure/build use a
             * named preset.
             */
            if (!umi_build_has_text(request->build_directory)) {
                umi_build_message(
                    out_message,
                    message_capacity,
                    "Deliver requires an explicit build directory for installation.");
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;

        default:
            umi_build_message(out_message, message_capacity,
                              "Unknown build action.");
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_build_message(out_message, message_capacity, "Build request is valid.");
    return UMI_STATUS_OK;
}

/*
 * Provide the build accumulate result operation used by this module and its client
 * applications.
 */
static void umi_build_accumulate_result(UmiBuildReport *report,
                                        UmiBuildAction action,
                                        const UmiProcessResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || result == NULL) {
        return;
    }

    report->last_action = action;
    report->cancelled = report->cancelled || result->cancelled;
    report->timed_out = report->timed_out || result->timed_out;
    report->output_truncated =
        report->output_truncated || result->output_truncated;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (UINT64_MAX - report->duration_ms < result->duration_ms) {
        report->duration_ms = UINT64_MAX;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        report->duration_ms += result->duration_ms;
    }

    (void)snprintf(report->last_output,
                   sizeof(report->last_output),
                   "%s",
                   result->output);
}

/*
 * Provide the build process status operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_process_status(const UmiProcessResult *result,
                                          UmiStatus transport_status)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (transport_status != UMI_STATUS_OK) {
        return transport_status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result->cancelled) {
        return UMI_STATUS_CANCELLED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result->timed_out) {
        return UMI_STATUS_TIMEOUT;
    }
    return result->exit_code == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_INTERNAL_ERROR;
}

/*
 * Process transport success and child-program success are different concepts.
 * A compiler, CMake or CTest process may launch correctly and still return a
 * non-zero exit code. The Framework therefore records both the child evidence
 * and an operation status suitable for Studio/automation.
 */
static UmiStatus umi_build_run_process(
    const UmiToolInfo *tool,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *build_request,
    UmiBuildAction action,
    int *out_exit_code,
    UmiBuildReport *report)
{
    UmiProcessRequest process_request;
    UmiProcessResult result;
    UmiStatus transport_status;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tool == NULL || tool->state != UMI_TOOL_VALIDATED) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (report != NULL) {
            report->last_action = action;
            report->last_status = UMI_STATUS_NOT_FOUND;
        }
        return UMI_STATUS_NOT_FOUND;
    }

    (void)memset(&process_request, 0, sizeof(process_request));
    (void)memset(&result, 0, sizeof(result));
    result.exit_code = -1;

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
    process_request.timeout_ms =
        build_request != NULL ? build_request->timeout_ms : 0U;
    process_request.poll_interval_ms = 25U;
    process_request.cancellation =
        build_request != NULL ? build_request->cancellation : NULL;
    process_request.window_mode =
        build_request != NULL
            ? build_request->window_mode
            : UMI_PROCESS_WINDOW_HIDDEN;

    transport_status = umi_process_execute(&process_request, &result);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }

    umi_build_accumulate_result(report, action, &result);
    status = umi_build_process_status(&result, transport_status);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) {
        report->last_status = status;
    }

    return status;
}

/*
 * Provide the build clean internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_clean_internal(const UmiBuildRequest *request,
                                          UmiBuildReport *report)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        !umi_build_has_text(request->build_directory)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_fs_remove_tree(request->build_directory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) {
        status = UMI_STATUS_OK;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report != NULL) {
        report->last_action = UMI_BUILD_CLEAN;
        report->last_status = status;
        (void)snprintf(
            report->last_output,
            sizeof(report->last_output),
            "%s",
            status == UMI_STATUS_OK
                ? "Build directory cleaned."
                : "Build directory clean failed.");
    }

    return status;
}

/*
 * Provide the build configure internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_configure_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *cmake =
        umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE);
    const char *arguments[12];
    size_t count = 0U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->preset)) {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        arguments[count++] = "-S";
        arguments[count++] = request->source_root;
        arguments[count++] = "-B";
        arguments[count++] = request->build_directory;
        arguments[count++] = "-G";
        arguments[count++] = "Ninja";
        arguments[count++] = "-DCMAKE_BUILD_TYPE=Debug";
        arguments[count++] = "-DBUILD_TESTING=ON";
    }

    return umi_build_run_process(
        cmake,
        arguments,
        count,
        request->source_root,
        environment,
        request,
        UMI_BUILD_CONFIGURE,
        &report->configure_exit_code,
        report);
}

/*
 * Provide the build compile internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_compile_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *cmake =
        umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE);
    const char *arguments[10];
    char jobs[32];
    size_t count = 0U;

    arguments[count++] = "--build";

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->preset)) {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        arguments[count++] = request->build_directory;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->target)) {
        arguments[count++] = "--target";
        arguments[count++] = request->target;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->jobs > 0) {
        (void)snprintf(jobs, sizeof(jobs), "%d", request->jobs);
        arguments[count++] = "--parallel";
        arguments[count++] = jobs;
    }

    return umi_build_run_process(
        cmake,
        arguments,
        count,
        request->source_root,
        environment,
        request,
        UMI_BUILD_COMPILE,
        &report->build_exit_code,
        report);
}

/*
 * Provide the build test internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_test_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *ctest =
        umi_toolchain_profile_tool(profile, UMI_TOOL_CTEST);
    const char *arguments[8];
    size_t count = 0U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->preset)) {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        arguments[count++] = "--test-dir";
        arguments[count++] = request->build_directory;
        arguments[count++] = "--output-on-failure";
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->configuration)) {
        arguments[count++] = "-C";
        arguments[count++] = request->configuration;
    }

    return umi_build_run_process(
        ctest,
        arguments,
        count,
        request->source_root,
        environment,
        request,
        UMI_BUILD_TEST,
        &report->test_exit_code,
        report);
}

/*
 * Provide the build install internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_install_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *cmake =
        umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE);
    const char *arguments[8];
    size_t count = 0U;

    arguments[count++] = "--install";
    arguments[count++] = request->build_directory;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->configuration)) {
        arguments[count++] = "--config";
        arguments[count++] = request->configuration;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->install_prefix)) {
        arguments[count++] = "--prefix";
        arguments[count++] = request->install_prefix;
    }

    return umi_build_run_process(
        cmake,
        arguments,
        count,
        request->source_root,
        environment,
        request,
        UMI_BUILD_INSTALL,
        &report->install_exit_code,
        report);
}

/*
 * Provide the build package internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_package_internal(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildReport *report)
{
    const UmiToolInfo *cmake =
        umi_toolchain_profile_tool(profile, UMI_TOOL_CMAKE);
    const char *arguments[10];
    const char *package_target =
        umi_build_has_text(request->package_target)
            ? request->package_target
            : "package";
    char jobs[32];
    size_t count = 0U;

    arguments[count++] = "--build";

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_build_has_text(request->preset)) {
        arguments[count++] = "--preset";
        arguments[count++] = request->preset;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        arguments[count++] = request->build_directory;
    }

    arguments[count++] = "--target";
    arguments[count++] = package_target;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->jobs > 0) {
        (void)snprintf(jobs, sizeof(jobs), "%d", request->jobs);
        arguments[count++] = "--parallel";
        arguments[count++] = jobs;
    }

    return umi_build_run_process(
        cmake,
        arguments,
        count,
        request->source_root,
        environment,
        request,
        UMI_BUILD_PACKAGE,
        &report->package_exit_code,
        report);
}

/*
 * Provide the build run internal operation used by this module and its client
 * applications.
 */
static UmiStatus umi_build_run_internal(
    UmiEnvironmentPlan *environment,
    const UmiBuildRequest *request,
    UmiBuildAction action,
    UmiBuildReport *report)
{
    UmiProcessRequest process_request;
    UmiProcessResult result;
    UmiStatus transport_status;
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_build_has_text(request->executable)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&process_request, 0, sizeof(process_request));
    (void)memset(&result, 0, sizeof(result));
    result.exit_code = -1;

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
    process_request.timeout_ms = request->timeout_ms;
    process_request.poll_interval_ms = 25U;
    process_request.cancellation = request->cancellation;
    process_request.window_mode = request->window_mode;

    transport_status = umi_process_execute(&process_request, &result);
    report->run_exit_code = result.exit_code;
    umi_build_accumulate_result(report, action, &result);

    status = umi_build_process_status(&result, transport_status);
    report->last_status = status;
    return status;
}

/*
 * Perform build through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_build_execute(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    UmiBuildAction action,
    const UmiBuildRequest *request,
    UmiBuildReport *out_report)
{
    UmiBuildReport local_report;
    UmiBuildReport *report =
        out_report != NULL ? out_report : &local_report;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_build_report_init(report);

    status = umi_build_request_validate(
        action, request, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        report->last_action = action;
        report->last_status = status;
        return status;
    }

    /* Create this optional product surface only when its build option is enabled. */
    if (request->clean && action != UMI_BUILD_CLEAN) {
        status = umi_build_clean_internal(request, report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Create this optional product surface only when its build option is enabled. */
    if (action == UMI_BUILD_CLEAN) {
        return umi_build_clean_internal(request, report);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_BUILD_CONFIGURE ||
        action == UMI_BUILD_MAKE ||
        action == UMI_BUILD_DELIVER) {
        status = umi_build_configure_internal(
            profile, environment, request, report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_BUILD_COMPILE ||
        action == UMI_BUILD_MAKE ||
        action == UMI_BUILD_DELIVER) {
        status = umi_build_compile_internal(
            profile, environment, request, report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_BUILD_TEST ||
        action == UMI_BUILD_MAKE ||
        action == UMI_BUILD_DELIVER) {
        status = umi_build_test_internal(
            profile, environment, request, report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_BUILD_INSTALL ||
        action == UMI_BUILD_DELIVER) {
        status = umi_build_install_internal(
            profile, environment, request, report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (action == UMI_BUILD_PACKAGE ||
        action == UMI_BUILD_DELIVER) {
        status = umi_build_package_internal(
            profile, environment, request, report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Create this optional product surface only when its build option is enabled. */
    if (action == UMI_BUILD_RUN || action == UMI_BUILD_COMMAND) {
        return umi_build_run_internal(
            environment, request, action, report);
    }

    report->last_status = UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

/*
 * Provide the build repair cache operation used by this module and its client
 * applications.
 */
UmiStatus umi_build_repair_cache(
    const UmiToolchainProfile *profile,
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || build_directory == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_fs_join(
        cache_path,
        sizeof(cache_path),
        build_directory,
        "CMakeCache.txt");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || !umi_fs_is_file(cache_path)) {
        return UMI_STATUS_OK;
    }

    /*
     * Cache repair follows the selected profile compiler. The Windows UCRT64
     * profile is GCC-first; CLANG64 profiles select Clang. Hard-coding Clang
     * here made a healthy GCC cache look stale.
     */
    compiler = umi_toolchain_profile_c_compiler(profile);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compiler == NULL ||
        compiler->state != UMI_TOOL_VALIDATED ||
        compiler->path[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_fs_read_text(cache_path, &cache, NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)snprintf(
        marker,
        sizeof(marker),
        "CMAKE_C_COMPILER:FILEPATH=%s",
        compiler->path);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(cache, marker) != NULL) {
        umi_fs_free_text(cache);
        return UMI_STATUS_OK;
    }

    umi_fs_free_text(cache);

    now = time(NULL);
    time_info = localtime(&now);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (time_info == NULL ||
        strftime(timestamp,
                 sizeof(timestamp),
                 "%Y%m%d-%H%M%S",
                 time_info) == 0U) {
        (void)snprintf(timestamp,
                       sizeof(timestamp),
                       "unknown-time");
    }

    (void)snprintf(recovery,
                   sizeof(recovery),
                   "%s.recovery-%s",
                   build_directory,
                   timestamp);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_recovery_path != NULL && capacity > 0U) {
        (void)snprintf(out_recovery_path,
                       capacity,
                       "%s",
                       recovery);
    }
    return dry_run
        ? UMI_STATUS_OK
        : umi_fs_rename(build_directory, recovery);
}

/*
 * Provide the build write user presets operation used by this module and its client
 * applications.
 */
UmiStatus umi_build_write_user_presets(
    const UmiToolchainProfile *profile,
    const char *project_root,
    const char *path)
{
    const UmiToolInfo *compiler;
    const UmiToolInfo *ninja;
    const UmiToolInfo *pkg_config;
    char text[16384];
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || project_root == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Generate a preset from the compiler selected by discovery rather than
     * imposing Clang on every environment. This preserves GCC/UCRT64 as the
     * canonical Windows developer profile while retaining Clang profiles.
     */
    compiler = umi_toolchain_profile_c_compiler(profile);
    ninja = umi_toolchain_profile_tool(profile, UMI_TOOL_NINJA);
    pkg_config =
        umi_toolchain_profile_tool(profile, UMI_TOOL_PKG_CONFIG);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (compiler == NULL ||
        compiler->state != UMI_TOOL_VALIDATED ||
        ninja == NULL ||
        ninja->state != UMI_TOOL_VALIDATED ||
        pkg_config == NULL ||
        pkg_config->state != UMI_TOOL_VALIDATED) {
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
        profile->prefix_directory);

    (void)project_root;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return umi_fs_write_text(path, text);
}

/* Provide the build open shell operation used by this module and its client applications. */
UmiStatus umi_build_open_shell(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const char *working_directory,
    const char *shell_program)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    const char *program = shell_program;

    (void)profile;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

#ifdef _WIN32
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (program == NULL || program[0] == '\0') {
        program = "powershell.exe";
    }
#else
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (program == NULL || program[0] == '\0') {
        program = getenv("SHELL");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (program == NULL || program[0] == '\0') {
            program = "/bin/sh";
        }
    }
#endif

    (void)memset(&request, 0, sizeof(request));
    (void)memset(&result, 0, sizeof(result));

    request.program = program;
    request.working_directory = working_directory;
    request.environment =
        umi_environment_plan_variables(environment);
    request.environment_count = environment->count;
    request.window_mode = UMI_PROCESS_WINDOW_VISIBLE;

    return umi_process_execute(&request, &result);
}
