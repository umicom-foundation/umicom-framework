/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/repository.c
 *
 * PURPOSE:
 *   Implement native repository creation through argument-based Git and GitHub
 *   CLI child processes.  Remote creation is always explicit and supports a
 *   dry-run so no repository or network state is changed accidentally.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/repository.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"

static UmiStatus umi_repository_run(
    const UmiToolInfo *tool,
    UmiEnvironmentPlan *environment,
    const char *working_directory,
    const char *const *arguments,
    size_t argument_count,
    int *out_exit_code,
    char *out_text,
    size_t capacity)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiStatus status;

    if (tool == NULL || tool->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(&request, 0, sizeof(request));
    request.program = tool->path;
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.working_directory = working_directory;
    request.environment = environment != NULL
        ? umi_environment_plan_variables(environment)
        : NULL;
    request.environment_count = environment != NULL ? environment->count : 0U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    status = umi_process_execute(&request, &result);
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }
    if (out_text != NULL && capacity > 0U) {
        (void)snprintf(out_text, capacity, "%s", result.output);
    }
    return status;
}

UmiStatus umi_repository_initialise(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryRequest *request,
    UmiRepositoryReport *out_report)
{
    const UmiToolInfo *git;
    const char *init_arguments[] = {"init", "-b", "main"};
    const char *add_arguments[] = {"add", "--all"};
    const char *commit_arguments[] = {
        "commit", "-m", "feat: initialise Umicom Framework application"
    };
    UmiStatus status;

    if (profile == NULL || request == NULL || out_report == NULL ||
        request->repository_root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)snprintf(out_report->repository_root,
                   sizeof(out_report->repository_root),
                   "%s",
                   request->repository_root);
    git = umi_toolchain_profile_tool(profile, UMI_TOOL_GIT);
    if (git == NULL || git->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }

    if (request->dry_run) {
        out_report->local_initialised = request->initialise_local;
        out_report->initial_commit_created = request->create_initial_commit;
        return UMI_STATUS_OK;
    }
    if (!umi_fs_is_directory(request->repository_root)) {
        status = umi_fs_make_directories(request->repository_root);
        if (status != UMI_STATUS_OK) return status;
    }
    if (request->initialise_local) {
        status = umi_repository_run(git,
                                    environment,
                                    request->repository_root,
                                    init_arguments,
                                    3U,
                                    &out_report->last_exit_code,
                                    out_report->last_output,
                                    sizeof(out_report->last_output));
        if (status != UMI_STATUS_OK) return status;
        out_report->local_initialised = 1;
    }
    if (request->create_initial_commit) {
        status = umi_repository_run(git,
                                    environment,
                                    request->repository_root,
                                    add_arguments,
                                    2U,
                                    &out_report->last_exit_code,
                                    out_report->last_output,
                                    sizeof(out_report->last_output));
        if (status != UMI_STATUS_OK) return status;
        status = umi_repository_run(git,
                                    environment,
                                    request->repository_root,
                                    commit_arguments,
                                    3U,
                                    &out_report->last_exit_code,
                                    out_report->last_output,
                                    sizeof(out_report->last_output));
        if (status != UMI_STATUS_OK) return status;
        out_report->initial_commit_created = 1;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_create_remote(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryRequest *request,
    UmiRepositoryReport *out_report)
{
    const UmiToolInfo *gh;
    char full_name[UMI_REPOSITORY_TEXT_CAPACITY * 2U];
    const char *arguments[16];
    size_t count = 0U;
    UmiStatus status;

    if (profile == NULL || request == NULL || out_report == NULL ||
        request->repository_root == NULL ||
        request->remote_name == NULL || request->remote_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    gh = umi_toolchain_profile_tool(profile, UMI_TOOL_GH);
    if (gh == NULL || gh->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (request->remote_owner != NULL && request->remote_owner[0] != '\0') {
        (void)snprintf(full_name,
                       sizeof(full_name),
                       "%s/%s",
                       request->remote_owner,
                       request->remote_name);
    } else {
        (void)snprintf(full_name,
                       sizeof(full_name),
                       "%s",
                       request->remote_name);
    }

    arguments[count++] = "repo";
    arguments[count++] = "create";
    arguments[count++] = full_name;
    arguments[count++] = request->visibility == UMI_REPOSITORY_PUBLIC
        ? "--public"
        : "--private";
    arguments[count++] = "--source";
    arguments[count++] = request->repository_root;
    arguments[count++] = "--remote";
    arguments[count++] = "origin";
    if (request->remote_description != NULL &&
        request->remote_description[0] != '\0') {
        arguments[count++] = "--description";
        arguments[count++] = request->remote_description;
    }
    if (request->push_initial) {
        arguments[count++] = "--push";
    }

    {
        size_t remote_name_length = strlen(full_name);
        if (remote_name_length >= sizeof(out_report->remote_name)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_report->remote_name,
                     full_name,
                     remote_name_length + 1U);
    }
    if (request->dry_run) {
        return UMI_STATUS_OK;
    }

    status = umi_repository_run(gh,
                                environment,
                                request->repository_root,
                                arguments,
                                count,
                                &out_report->last_exit_code,
                                out_report->last_output,
                                sizeof(out_report->last_output));
    if (status == UMI_STATUS_OK) {
        out_report->remote_created = 1;
        out_report->pushed = request->push_initial;
    }
    return status;
}

UmiStatus umi_repository_add_submodule(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    const char *url,
    const char *destination,
    const char *branch,
    int dry_run)
{
    const UmiToolInfo *git;
    const char *arguments[8];
    size_t count = 0U;
    int exit_code;
    char output[UMI_PROCESS_OUTPUT_CAPACITY];

    if (profile == NULL || repository_root == NULL || url == NULL ||
        destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    git = umi_toolchain_profile_tool(profile, UMI_TOOL_GIT);
    if (git == NULL || git->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (dry_run) {
        return UMI_STATUS_OK;
    }
    arguments[count++] = "submodule";
    arguments[count++] = "add";
    if (branch != NULL && branch[0] != '\0') {
        arguments[count++] = "-b";
        arguments[count++] = branch;
    }
    arguments[count++] = url;
    arguments[count++] = destination;
    return umi_repository_run(git,
                              environment,
                              repository_root,
                              arguments,
                              count,
                              &exit_code,
                              output,
                              sizeof(output));
}

UmiStatus umi_repository_status(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    char *out_text,
    size_t capacity)
{
    const UmiToolInfo *git;
    const char *arguments[] = {"status", "--short", "--branch"};
    int exit_code;

    if (profile == NULL || repository_root == NULL || out_text == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    git = umi_toolchain_profile_tool(profile, UMI_TOOL_GIT);
    return umi_repository_run(git,
                              environment,
                              repository_root,
                              arguments,
                              3U,
                              &exit_code,
                              out_text,
                              capacity);
}
