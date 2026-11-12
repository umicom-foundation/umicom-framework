/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/launch_plan.c
 *
 * PURPOSE:
 *   Implement launch-profile translation into run/debug developer operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/launch_plan.h"

#include <stdio.h>
#include <string.h>

#include "umicom/developer/command_line.h"
#include "../base/storage_internal.h"

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the parse arguments operation used by this module and its client applications. */
/* The former formatted-string helper is retained for review. Exact-span
 * tokenisation below now preserves empty arguments without scanning past a
 * restored profile field. */
#if 0
static UmiStatus parse_arguments(
    const char *arguments,
    UmiDeveloperCommandLine *out_command)
{
    char combined[1024U + 32U];
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (arguments == NULL || arguments[0] == '\0') {
        return umi_developer_command_line_parse("__umicom_launch__", out_command);
    }
    written = snprintf(combined, sizeof(combined), "__umicom_launch__ %s", arguments);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(combined)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_developer_command_line_parse(combined, out_command);
}

#endif

/* A launch profile already stores its executable separately. Prefix only a
 * private parser placeholder and pass the exact byte span; no shell command
 * is constructed or executed. Capacity follows the public profile field. */
static UmiStatus parse_arguments(
    const char *arguments,
    size_t arguments_length,
    UmiDeveloperCommandLine *out_command)
{
    static const char prefix[] = "__umicom_launch__ ";
    char combined[sizeof(((UmiProjectLaunchProfileSnapshot *)0)->arguments) +
                  sizeof(prefix)];
    const size_t prefix_length = sizeof(prefix) - 1U;
    if (arguments == NULL || out_command == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (arguments_length > sizeof(combined) - prefix_length)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(combined, prefix, prefix_length);
    if (arguments_length > 0U)
        memcpy(combined + prefix_length, arguments, arguments_length);
    return umi_developer_command_line_parse_n(
        combined, prefix_length + arguments_length, out_command);
}

/*
 * Provide the developer launch plan submit operation used by this module and its client
 * applications.
 */
/* The former combined planner/submission is retained for review. The shared
 * builder below translates the same project metadata without publishing work,
 * so a multi-step workflow can admit every task before changing the runtime.
 * The compatibility submit entry point still queues one translated operation. */
#if 0
UmiStatus umi_developer_launch_plan_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    UmiProjectLaunchProfileSnapshot profile;
    UmiDeveloperCommandLine command;
    UmiDeveloperOperationSnapshot operation;
    UmiProjectLaunchProfileRegistry *registry;
    char generated_id[UMI_DEVELOPER_ID_CAPACITY];
    const char *operation_id;
    const char *arguments_end;
    UmiDeveloperOperationKind kind;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || request == NULL || request->launch_profile_id == NULL ||
        request->launch_profile_id[0] == '\0' ||
        request->debug_mode < -1 || request->debug_mode > 1) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_project_workspace_launch_profile(
        umi_developer_runtime_projects(runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_project_launch_profile_registry_find(
        registry, request->launch_profile_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (profile.program[0] == '\0') return UMI_STATUS_INVALID_STATE;

    /* Keep parsing within the fixed profile field before publishing a plan. */
#if 0
    /* Retained old call used an unbounded formatted-string conversion. */
    status = parse_arguments(profile.arguments, &command);
#endif
    arguments_end = (const char *)memchr(
        profile.arguments, '\0', sizeof(profile.arguments));
    if (arguments_end == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = parse_arguments(profile.arguments,
        (size_t)(arguments_end - profile.arguments), &command);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    kind = request->debug_mode == 1 ||
           (request->debug_mode == -1 && profile.debug != 0)
               ? UMI_DEVELOPER_OPERATION_DEBUG
               : UMI_DEVELOPER_OPERATION_RUN;

    operation_id = request->operation_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation_id == NULL || operation_id[0] == '\0') {
        int written = snprintf(generated_id, sizeof(generated_id),
                               "%s.%s",
                               kind == UMI_DEVELOPER_OPERATION_DEBUG ? "debug" : "run",
                               profile.id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(generated_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        operation_id = generated_id;
    }

    status = umi_developer_operation_init(
        &operation,
        operation_id,
        kind,
        profile.name[0] != '\0' ? profile.name : profile.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_operation_set_program(
        &operation,
        profile.program,
        profile.working_directory[0] != '\0' ? profile.working_directory : NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < command.argument_count; ++index) {
        status = umi_developer_operation_add_argument(&operation, command.arguments[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = copy_text(operation.project_id, sizeof(operation.project_id), profile.project_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->configuration_id != NULL) {
        status = copy_text(operation.configuration_id,
                           sizeof(operation.configuration_id),
                           request->configuration_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->target_id != NULL) {
        status = copy_text(operation.target_id, sizeof(operation.target_id), request->target_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    operation.timeout_ms = request->timeout_ms;
    operation.max_attempts = request->max_attempts == 0U ? 1U : request->max_attempts;

    status = umi_developer_runtime_submit_operation(runtime, &operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan != NULL) {
        memset(out_plan, 0, sizeof(*out_plan));
        out_plan->struct_size = (uint32_t)sizeof(*out_plan);
        out_plan->api_version = UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION;
        (void)copy_text(out_plan->operation_id, sizeof(out_plan->operation_id), operation.id);
        (void)copy_text(out_plan->launch_profile_id,
                        sizeof(out_plan->launch_profile_id), profile.id);
        (void)copy_text(out_plan->project_id, sizeof(out_plan->project_id), profile.project_id);
        (void)copy_text(out_plan->environment_id,
                        sizeof(out_plan->environment_id), profile.environment_id);
        out_plan->operation_kind = kind;
    }
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_launch_plan_build(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    if (out_operation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProjectLaunchProfileSnapshot profile;
    UmiDeveloperCommandLine command;
    UmiDeveloperOperationSnapshot operation;
    UmiProjectLaunchProfileRegistry *registry;
    char generated_id[UMI_DEVELOPER_ID_CAPACITY];
    const char *operation_id;
    const char *arguments_end;
    UmiDeveloperOperationKind kind;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || request == NULL || request->launch_profile_id == NULL ||
        request->launch_profile_id[0] == '\0' ||
        request->debug_mode < -1 || request->debug_mode > 1) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_project_workspace_launch_profile(
        umi_developer_runtime_projects(runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_project_launch_profile_registry_find(
        registry, request->launch_profile_id, &profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (profile.program[0] == '\0') return UMI_STATUS_INVALID_STATE;

    /* Keep parsing within the fixed profile field before publishing a plan. */
#if 0
    /* Retained old call used an unbounded formatted-string conversion. */
    status = parse_arguments(profile.arguments, &command);
#endif
    arguments_end = (const char *)memchr(
        profile.arguments, '\0', sizeof(profile.arguments));
    if (arguments_end == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = parse_arguments(profile.arguments,
        (size_t)(arguments_end - profile.arguments), &command);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    kind = request->debug_mode == 1 ||
           (request->debug_mode == -1 && profile.debug != 0)
               ? UMI_DEVELOPER_OPERATION_DEBUG
               : UMI_DEVELOPER_OPERATION_RUN;

    operation_id = request->operation_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation_id == NULL || operation_id[0] == '\0') {
        int written = snprintf(generated_id, sizeof(generated_id),
                               "%s.%s",
                               kind == UMI_DEVELOPER_OPERATION_DEBUG ? "debug" : "run",
                               profile.id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(generated_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        operation_id = generated_id;
    }

    status = umi_developer_operation_init(
        &operation,
        operation_id,
        kind,
        profile.name[0] != '\0' ? profile.name : profile.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_developer_operation_set_program(
        &operation,
        profile.program,
        profile.working_directory[0] != '\0' ? profile.working_directory : NULL);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < command.argument_count; ++index) {
        status = umi_developer_operation_add_argument(&operation, command.arguments[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = copy_text(operation.project_id, sizeof(operation.project_id), profile.project_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->configuration_id != NULL) {
        status = copy_text(operation.configuration_id,
                           sizeof(operation.configuration_id),
                           request->configuration_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->target_id != NULL) {
        status = copy_text(operation.target_id, sizeof(operation.target_id), request->target_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    operation.timeout_ms = request->timeout_ms;
    operation.max_attempts = request->max_attempts == 0U ? 1U : request->max_attempts;

    /* Planning owns no queue or journal mutation. Publish output values only
     * after the complete project record has been translated successfully. */
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan != NULL) {
        memset(out_plan, 0, sizeof(*out_plan));
        out_plan->struct_size = (uint32_t)sizeof(*out_plan);
        out_plan->api_version = UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION;
        (void)copy_text(out_plan->operation_id, sizeof(out_plan->operation_id), operation.id);
        (void)copy_text(out_plan->launch_profile_id,
                        sizeof(out_plan->launch_profile_id), profile.id);
        (void)copy_text(out_plan->project_id, sizeof(out_plan->project_id), profile.project_id);
        (void)copy_text(out_plan->environment_id,
                        sizeof(out_plan->environment_id), profile.environment_id);
        out_plan->operation_kind = kind;
    }
    *out_operation = operation;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_launch_plan_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    UmiDeveloperOperationSnapshot operation;
    UmiDeveloperLaunchPlanSnapshot plan;
    UmiStatus status = umi_developer_launch_plan_build(runtime, request, &operation, &plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_runtime_submit_operation(runtime, &operation);
    if (status == UMI_STATUS_OK && out_plan != NULL) *out_plan = plan;
    return status;
}

/* Profile review belongs to Framework so Studio and native CLI consumers use
 * the same validated saved fields. It observes only: queue publication and
 * process execution remain separate operations on the serialized runtime. */
UmiStatus umi_developer_launch_plan_compare_profile(
    UmiDeveloperRuntime *runtime,
    const UmiProjectLaunchProfileSnapshot *expected,
    void *workspace, size_t capacity,
    UmiValueArchiveComparison *out_comparison)
{
    if (runtime == NULL || expected == NULL || out_comparison == NULL ||
        (workspace == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_project_launch_profile_snapshot_validate(expected, NULL);
    if (status != UMI_STATUS_OK) return status;
    UmiProjectLaunchProfileRegistry *registry = umi_project_workspace_launch_profile(
        umi_developer_runtime_projects(runtime));
    if (registry == NULL) return UMI_STATUS_INVALID_STATE;
    UmiProjectLaunchProfileSnapshot current;
    status = umi_project_launch_profile_registry_find(registry, expected->id, &current);
    if (status != UMI_STATUS_OK) return status;
    return umi_project_launch_profile_snapshot_archive_compare_with_workspace(
        expected, &current, workspace, capacity, out_comparison);
}

/* A displayed profile is a captured value, not authority to use whichever
 * executable later occupies its ID. Admit the review and publish on the same
 * serialized runtime turn. The existing submit API retains its live-lookup
 * contract for clients that intentionally do not present a prior review. */
UmiStatus umi_developer_launch_plan_submit_reviewed(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    const UmiProjectLaunchProfileSnapshot *expected,
    void *workspace, size_t capacity,
    UmiDeveloperLaunchPlanSnapshot *out_plan)
{
    if (runtime == NULL || request == NULL || expected == NULL || workspace == NULL ||
        request->struct_size != sizeof(*request) ||
        request->api_version != UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION ||
        request->launch_profile_id == NULL || request->debug_mode < -1 ||
        request->debug_mode > 1)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_project_launch_profile_snapshot_validate(expected, NULL);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(request->launch_profile_id, expected->id) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Canonical comparison writes scratch bytes. Refuse storage aliases before
     * those writes could destroy a request field needed by the later planner.
     * Borrowed strings remain valid and stable until this call returns. */
    if (UmiStorageOverlaps(workspace, capacity, request, sizeof(*request)) ||
        UmiStorageOverlaps(workspace, capacity, expected, sizeof(*expected)) ||
        (out_plan != NULL &&
         (UmiStorageOverlaps(workspace, capacity, out_plan, sizeof(*out_plan)) ||
          UmiStorageOverlaps(out_plan, sizeof(*out_plan), request, sizeof(*request)) ||
          UmiStorageOverlaps(out_plan, sizeof(*out_plan), expected, sizeof(*expected)))))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *strings[] = {request->launch_profile_id, request->operation_id,
                            request->configuration_id, request->target_id};
    for (size_t i = 0U; i < sizeof(strings) / sizeof(strings[0]); ++i) {
        if (strings[i] == NULL) continue;
        size_t size = strlen(strings[i]) + 1U;
        if (UmiStorageOverlaps(workspace, capacity, strings[i], size) ||
            (out_plan != NULL &&
             UmiStorageOverlaps(out_plan, sizeof(*out_plan), strings[i], size)))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiValueArchiveComparison comparison;
    status = umi_developer_launch_plan_compare_profile(runtime, expected,
        workspace, capacity, &comparison);
    if (status != UMI_STATUS_OK) return status;
    if (comparison.relation != UMI_VALUE_ARCHIVE_EQUAL)
        return UMI_STATUS_INVALID_STATE;
    return umi_developer_launch_plan_submit(runtime, request, out_plan);
}
