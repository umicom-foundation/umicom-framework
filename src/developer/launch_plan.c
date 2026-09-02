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

/*
 * Provide the developer launch plan submit operation used by this module and its client
 * applications.
 */
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

    status = parse_arguments(profile.arguments, &command);
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
