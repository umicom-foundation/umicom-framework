/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/workflow_executor.c
 *
 * PURPOSE:
 *   Execute repository workflows through argument vectors, explicit working
 *   directories and the Framework process abstraction. Shell command strings,
 *   force pushes and automatic lock deletion are deliberately excluded.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_executor.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process.h"
#include "umicom/repository/repository.h"
#include "umicom/repository/workflow_validation.h"

static UmiStatus umi_repository_workflow_append_output(
    UmiRepositoryWorkflowReport *report,
    const char *text)
{
    size_t current_length;
    size_t available;
    size_t text_length;

    if (report == NULL || text == NULL || text[0] == '\0') {
        return UMI_STATUS_OK;
    }
    current_length = strlen(report->output);
    text_length = strlen(text);
    if (current_length > 0U && report->output[current_length - 1U] != '\n') {
        if (current_length + 1U >= sizeof(report->output)) {
            report->output_truncated = 1;
            return UMI_STATUS_OK;
        }
        report->output[current_length++] = '\n';
        report->output[current_length] = '\0';
    }
    available = sizeof(report->output) - current_length - 1U;
    if (text_length > available) {
        (void)memcpy(report->output + current_length, text, available);
        report->output[current_length + available] = '\0';
        report->output_truncated = 1;
        return UMI_STATUS_OK;
    }
    (void)memcpy(report->output + current_length, text, text_length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus umi_repository_workflow_run_git(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *working_directory,
    const char *const *arguments,
    size_t argument_count,
    int accept_nonzero,
    UmiRepositoryWorkflowReport *report)
{
    UmiProcessRequest process_request;
    UmiProcessResult process_result;
    UmiStatus status;

    if (git == NULL || git->state != UMI_TOOL_VALIDATED ||
        working_directory == NULL || arguments == NULL ||
        argument_count == 0U || report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&process_request, 0, sizeof(process_request));
    (void)memset(&process_result, 0, sizeof(process_result));
    process_request.program = git->path;
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
    process_request.timeout_ms = 300000U;
    process_request.poll_interval_ms = 20U;
    process_request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;

    status = umi_process_execute(&process_request, &process_result);
    report->last_exit_code = process_result.exit_code;
    report->output_truncated =
        report->output_truncated || process_result.output_truncated;
    (void)umi_repository_workflow_append_output(report, process_result.output);
    if (status != UMI_STATUS_OK) return status;
    if (!accept_nonzero && process_result.exit_code != 0) {
        return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
}

static UmiStatus umi_repository_workflow_check_index_lock(
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    char git_directory[UMI_REPOSITORY_PATH_CAPACITY];
    char lock_path[UMI_REPOSITORY_PATH_CAPACITY];
    UmiStatus status;

    if (!umi_repository_workflow_action_writes_index(request->action)) {
        return UMI_STATUS_OK;
    }
    status = umi_fs_join(git_directory,
                         sizeof(git_directory),
                         request->repository_root,
                         ".git");
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_directory(git_directory)) return UMI_STATUS_OK;
    status = umi_fs_join(lock_path,
                         sizeof(lock_path),
                         git_directory,
                         "index.lock");
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_file(lock_path)) return UMI_STATUS_OK;

    (void)snprintf(
        report->output,
        sizeof(report->output),
        "Git index lock exists: %s\n"
        "Wait for active Git processes to finish. Remove the lock only after "
        "confirming no Git process is running.",
        lock_path);
    return UMI_STATUS_BUSY;
}

static UmiStatus umi_repository_workflow_status(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    UmiRepositoryWorkflowReport *report)
{
    const char *arguments[] = {"status", "--short", "--branch"};
    return umi_repository_workflow_run_git(
        git, environment, repository_root, arguments, 3U, 0, report);
}

static UmiStatus umi_repository_workflow_stage(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    UmiRepositoryWorkflowReport *report)
{
    const char *arguments[] = {"add", "-A"};
    UmiStatus status = umi_repository_workflow_run_git(
        git, environment, repository_root, arguments, 2U, 0, report);
    if (status != UMI_STATUS_OK) return status;
    report->staged = 1;
    return umi_repository_workflow_status(
        git, environment, repository_root, report);
}

static UmiStatus umi_repository_workflow_commit(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    const char *message,
    UmiRepositoryWorkflowReport *report)
{
    const char *quiet_arguments[] = {"diff", "--cached", "--quiet"};
    const char *check_arguments[] = {"diff", "--cached", "--check"};
    const char *commit_arguments[] = {"commit", "-m", message};
    UmiStatus status;

    status = umi_repository_workflow_run_git(
        git, environment, repository_root,
        quiet_arguments, 3U, 1, report);
    if (status != UMI_STATUS_OK) return status;
    if (report->last_exit_code == 0) {
        report->no_changes = 1;
        return UMI_STATUS_OK;
    }
    if (report->last_exit_code != 1) return UMI_STATUS_IO_ERROR;

    status = umi_repository_workflow_run_git(
        git, environment, repository_root,
        check_arguments, 3U, 0, report);
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_workflow_run_git(
        git, environment, repository_root,
        commit_arguments, 3U, 0, report);
    if (status == UMI_STATUS_OK) report->commit_created = 1;
    return status;
}

static UmiStatus umi_repository_workflow_push(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    const char *arguments[5];
    size_t count = 0U;
    UmiStatus status;

    arguments[count++] = "push";
    if (request->set_upstream) arguments[count++] = "--set-upstream";
    arguments[count++] = request->remote_name;
    arguments[count++] = request->branch;
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        arguments, count, 0, report);
    if (status == UMI_STATUS_OK) report->pushed = 1;
    return status;
}

/*
 * Update is deliberately fast-forward-only. A pull that creates an automatic
 * merge commit makes it harder for a beginner to understand what changed and
 * can hide a conflict until much later. The two quiet diff checks protect both
 * unstaged and staged tracked work before any remote state is applied.
 */
static UmiStatus umi_repository_workflow_update(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    const char *worktree_arguments[] = {"diff", "--quiet"};
    const char *index_arguments[] = {"diff", "--cached", "--quiet"};
    const char *fetch_arguments[] = {
        "fetch", "--prune", request->remote_name, request->branch
    };
    const char *merge_arguments[] = {"merge", "--ff-only", "FETCH_HEAD"};
    const char *sync_arguments[] = {"submodule", "sync", "--recursive"};
    const char *submodule_arguments[] = {
        "submodule", "update", "--init", "--recursive"
    };
    UmiStatus status;

    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        worktree_arguments, 2U, 1, report);
    if (status != UMI_STATUS_OK) return status;
    if (report->last_exit_code != 0) {
        if (report->last_exit_code != 1) return UMI_STATUS_IO_ERROR;
        (void)umi_repository_workflow_append_output(
            report,
            "Update stopped: tracked working-tree changes must be committed "
            "or stashed first.");
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        index_arguments, 3U, 1, report);
    if (status != UMI_STATUS_OK) return status;
    if (report->last_exit_code != 0) {
        if (report->last_exit_code != 1) return UMI_STATUS_IO_ERROR;
        (void)umi_repository_workflow_append_output(
            report,
            "Update stopped: staged changes must be committed or stashed "
            "first.");
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        fetch_arguments, 4U, 0, report);
    if (status != UMI_STATUS_OK) return status;
    report->fetched = 1;

    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        merge_arguments, 3U, 0, report);
    if (status != UMI_STATUS_OK) return status;
    report->updated = 1;

    if (!request->recursive) return UMI_STATUS_OK;
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        sync_arguments, 3U, 0, report);
    if (status != UMI_STATUS_OK) return status;
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        submodule_arguments, 4U, 0, report);
    if (status == UMI_STATUS_OK) report->submodules_updated = 1;
    return status;
}

static UmiStatus umi_repository_workflow_clone(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    const char *arguments[12];
    char depth[32];
    size_t count = 0U;
    UmiStatus status;

    if (!umi_fs_is_directory(request->repository_root)) {
        status = umi_fs_make_directories(request->repository_root);
        if (status != UMI_STATUS_OK) return status;
    }
    arguments[count++] = "clone";
    if (request->recursive) arguments[count++] = "--recurse-submodules";
    if (request->branch != NULL && request->branch[0] != '\0') {
        arguments[count++] = "--branch";
        arguments[count++] = request->branch;
    }
    if (request->clone_depth > 0U) {
        (void)snprintf(depth, sizeof(depth), "%u", request->clone_depth);
        arguments[count++] = "--depth";
        arguments[count++] = depth;
    }
    arguments[count++] = "--";
    arguments[count++] = request->source_url;
    arguments[count++] = request->destination;
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        arguments, count, 0, report);
    if (status == UMI_STATUS_OK) report->cloned = 1;
    return status;
}

static UmiStatus umi_repository_workflow_initialise(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    const char *init_arguments[] = {"init", "-b", request->branch};
    const char *remote_arguments[] = {
        "remote", "add", request->remote_name, request->remote_url
    };
    UmiStatus status;

    if (!umi_fs_is_directory(request->repository_root)) {
        status = umi_fs_make_directories(request->repository_root);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        init_arguments, 3U, 0, report);
    if (status != UMI_STATUS_OK) return status;
    report->initialised = 1;
    if (request->remote_url == NULL || request->remote_url[0] == '\0') {
        return UMI_STATUS_OK;
    }
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        remote_arguments, 4U, 0, report);
    if (status == UMI_STATUS_OK) report->remote_added = 1;
    return status;
}

static UmiStatus umi_repository_workflow_add_submodule(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    const char *arguments[10];
    size_t count = 0U;
    UmiStatus status;

    arguments[count++] = "submodule";
    arguments[count++] = "add";
    if (request->branch != NULL && request->branch[0] != '\0') {
        arguments[count++] = "-b";
        arguments[count++] = request->branch;
    }
    arguments[count++] = "--";
    arguments[count++] = request->source_url;
    arguments[count++] = request->submodule_path;
    status = umi_repository_workflow_run_git(
        git, environment, request->repository_root,
        arguments, count, 0, report);
    if (status == UMI_STATUS_OK) report->submodule_added = 1;
    return status;
}

static void umi_repository_workflow_format_plan(
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *report)
{
    /* A dry run should show the values a beginner needs to verify, not merely
     * the name of the operation. No command string is constructed or run. */
    switch (request->action) {
        case UMI_REPOSITORY_WORKFLOW_CLONE:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would clone %s into %s from parent %s. Branch: %s. "
                "Recursive submodules: %s. Depth: %u.",
                request->source_url,
                request->destination,
                request->repository_root,
                request->branch != NULL ? request->branch : "remote default",
                request->recursive ? "yes" : "no",
                request->clone_depth);
            break;
        case UMI_REPOSITORY_WORKFLOW_INITIALISE:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would initialise %s on branch %s.%s%s",
                request->repository_root,
                request->branch,
                request->remote_url != NULL ? " Remote URL: " : "",
                request->remote_url != NULL ? request->remote_url : "");
            break;
        case UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would add submodule %s at %s in %s on branch %s.",
                request->source_url,
                request->submodule_path,
                request->repository_root,
                request->branch != NULL ? request->branch : "remote default");
            break;
        case UMI_REPOSITORY_WORKFLOW_STAGE:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would stage all tracked, changed and removed files in %s.",
                request->repository_root);
            break;
        case UMI_REPOSITORY_WORKFLOW_COMMIT:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would validate and commit staged files in %s. Message: %s",
                request->repository_root,
                request->commit_message);
            break;
        case UMI_REPOSITORY_WORKFLOW_PUSH:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would push %s to %s/%s.%s",
                request->repository_root,
                request->remote_name,
                request->branch,
                request->set_upstream ? " Upstream tracking would be set." : "");
            break;
        case UMI_REPOSITORY_WORKFLOW_PUBLISH:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would stage, validate, conditionally commit and push %s to "
                "%s/%s. Message: %s%s",
                request->repository_root,
                request->remote_name,
                request->branch,
                request->commit_message,
                request->set_upstream ? " Upstream tracking would be set." : "");
            break;
        case UMI_REPOSITORY_WORKFLOW_UPDATE:
            (void)snprintf(
                report->output,
                sizeof(report->output),
                "Would verify a clean tracked worktree, fetch %s/%s, apply "
                "only a fast-forward update in %s and update submodules: %s.",
                request->remote_name,
                request->branch,
                request->repository_root,
                request->recursive ? "yes" : "no");
            break;
        default:
            report->output[0] = '\0';
            break;
    }
    report->planned = 1;
}

UmiStatus umi_repository_workflow_execute(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *out_report)
{
    const UmiToolInfo *git;
    UmiStatus status;

    if (out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_repository_workflow_report_init(
        out_report,
        request != NULL ? request->action : UMI_REPOSITORY_WORKFLOW_UNKNOWN);
    status = umi_repository_workflow_validate(request);
    if (status != UMI_STATUS_OK) {
        out_report->status = status;
        return status;
    }
    if (request->dry_run) {
        umi_repository_workflow_format_plan(request, out_report);
        return UMI_STATUS_OK;
    }
    if (profile == NULL) {
        out_report->status = UMI_STATUS_INVALID_ARGUMENT;
        return out_report->status;
    }
    git = umi_toolchain_profile_tool(profile, UMI_TOOL_GIT);
    if (git == NULL || git->state != UMI_TOOL_VALIDATED) {
        out_report->status = UMI_STATUS_NOT_FOUND;
        return out_report->status;
    }
    if (request->action != UMI_REPOSITORY_WORKFLOW_CLONE &&
        request->action != UMI_REPOSITORY_WORKFLOW_INITIALISE &&
        !umi_fs_is_directory(request->repository_root)) {
        out_report->status = UMI_STATUS_NOT_FOUND;
        return out_report->status;
    }
    status = umi_repository_workflow_check_index_lock(request, out_report);
    if (status != UMI_STATUS_OK) {
        out_report->status = status;
        return status;
    }

    switch (request->action) {
        case UMI_REPOSITORY_WORKFLOW_CLONE:
            status = umi_repository_workflow_clone(
                git, environment, request, out_report);
            break;
        case UMI_REPOSITORY_WORKFLOW_INITIALISE:
            status = umi_repository_workflow_initialise(
                git, environment, request, out_report);
            break;
        case UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD:
            status = umi_repository_workflow_add_submodule(
                git, environment, request, out_report);
            break;
        case UMI_REPOSITORY_WORKFLOW_STAGE:
            status = umi_repository_workflow_stage(
                git, environment, request->repository_root, out_report);
            break;
        case UMI_REPOSITORY_WORKFLOW_COMMIT:
            status = umi_repository_workflow_commit(
                git, environment, request->repository_root,
                request->commit_message, out_report);
            break;
        case UMI_REPOSITORY_WORKFLOW_PUSH:
            status = umi_repository_workflow_push(
                git, environment, request, out_report);
            break;
        case UMI_REPOSITORY_WORKFLOW_PUBLISH:
            status = umi_repository_workflow_stage(
                git, environment, request->repository_root, out_report);
            if (status == UMI_STATUS_OK) {
                status = umi_repository_workflow_commit(
                    git, environment, request->repository_root,
                    request->commit_message, out_report);
            }
            if (status == UMI_STATUS_OK) {
                status = umi_repository_workflow_push(
                    git, environment, request, out_report);
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_UPDATE:
            status = umi_repository_workflow_update(
                git, environment, request, out_report);
            break;
        default:
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
    }
    out_report->status = status;
    return status;
}
