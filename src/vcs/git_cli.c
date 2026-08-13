/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/git_cli.c
 *
 * PURPOSE:
 *   Implement Git status, history, stage, unstage, commit, pull and push through safe argument arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/git_cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/process.h"
#include "umicom/vcs/status.h"

typedef struct UmiGitCliProvider {
    int reserved;
} UmiGitCliProvider;

static UmiStatus run_git(const char *root,
                         const char *const *arguments,
                         size_t argument_count,
                         char *out_text,
                         size_t capacity,
                         int *out_exit_code)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiStatus status;
    (void)memset(&request, 0, sizeof(request));
    (void)memset(&result, 0, sizeof(result));
    if (out_text != NULL && capacity > 0U) out_text[0] = '\0';
    if (out_exit_code != NULL) *out_exit_code = -1;
    request.program = "git";
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.working_directory = root;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    status = umi_process_execute(&request, &result);
    if (status != UMI_STATUS_OK) return status;
    if (out_text != NULL && capacity > 0U) {
        size_t output_length = strlen(result.output);
        if (result.output_truncated || output_length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_text, result.output, output_length + 1U);
    }
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }
    return status;
}

static UmiStatus run_git_checked(const char *root,
                                 const char *const *arguments,
                                 size_t argument_count,
                                 char *out_text,
                                 size_t capacity)
{
    int exit_code = -1;
    UmiStatus status = run_git(root, arguments, argument_count,
                               out_text, capacity, &exit_code);
    if (status != UMI_STATUS_OK) return status;
    return exit_code == 0 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

static UmiStatus git_status(void *instance,
                            const char *root,
                            UmiVcsChangeList *out_changes,
                            UmiVcsBranch *out_branch)
{
    const char *arguments[] = {
        "status",
        "--porcelain=v1",
        "--branch",
        "--untracked-files=all"
    };
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    const char *newline;
    int exit_code = 0;
    UmiStatus status;
    (void)instance;
    if (root == NULL || out_changes == NULL || out_branch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = run_git(root,
                     arguments,
                     sizeof(arguments) / sizeof(arguments[0]),
                     output,
                     sizeof(output),
                     &exit_code);
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_UNAVAILABLE;
    }
    newline = strchr(output, '\n');
    if (strncmp(output, "## ", 3U) == 0) {
        char header[1024];
        size_t length = newline != NULL
            ? (size_t)(newline - output)
            : strlen(output);
        if (length >= sizeof(header)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(header, output, length);
        header[length] = '\0';
        status = umi_vcs_branch_parse_status_header(header, out_branch);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    } else {
        (void)memset(out_branch, 0, sizeof(*out_branch));
    }
    return umi_vcs_status_parse_porcelain(
        newline != NULL ? newline + 1 : "",
        out_changes,
        NULL
    );
}

static UmiStatus git_history(void *instance,
                             const char *root,
                             size_t limit,
                             UmiVcsHistory *out_history)
{
    const char *arguments[5];
    char limit_text[64];
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    int exit_code = 0;
    UmiStatus status;
    (void)instance;
    if (root == NULL || out_history == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(limit_text,
                   sizeof(limit_text),
                   "--max-count=%zu",
                   limit > 0U ? limit : 20U);
    arguments[0] = "log";
    arguments[1] = limit_text;
    arguments[2] = "--date=unix";
    arguments[3] = "--pretty=format:%H%x1f%ct%x1f%an%x1f%ae%x1f%s";
    status = run_git(root,
                     arguments,
                     4U,
                     output,
                     sizeof(output),
                     &exit_code);
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_UNAVAILABLE;
    }
    return umi_vcs_history_parse(out_history, output, NULL);
}

static UmiStatus git_simple_path(const char *root,
                                 const char *verb,
                                 const char *path)
{
    const char *arguments[3];
    int exit_code = 0;
    if (root == NULL || verb == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    arguments[0] = verb;
    arguments[1] = "--";
    arguments[2] = path;
    if (run_git(root, arguments, 3U, NULL, 0U, &exit_code) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_IO_ERROR;
    }
    return exit_code == 0 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

static UmiStatus git_stage(void *instance,
                           const char *root,
                           const char *path)
{
    (void)instance;
    return git_simple_path(root, "add", path);
}

static UmiStatus git_unstage(void *instance,
                             const char *root,
                             const char *path)
{
    const char *arguments[] = {"restore", "--staged", "--", path};
    int exit_code = 0;
    (void)instance;
    if (root == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return run_git(root,
                   arguments,
                   sizeof(arguments) / sizeof(arguments[0]),
                   NULL,
                   0U,
                   &exit_code) == UMI_STATUS_OK && exit_code == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_INTERNAL_ERROR;
}

static UmiStatus git_commit(void *instance,
                            const char *root,
                            const char *message,
                            char *out_commit_id,
                            size_t capacity)
{
    const char *commit_arguments[] = {"commit", "-m", message};
    const char *id_arguments[] = {"rev-parse", "HEAD"};
    char output[UMI_VCS_ID_CAPACITY + 64U];
    int exit_code = 0;
    UmiStatus status;
    (void)instance;
    if (root == NULL || message == NULL || message[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = run_git(root,
                     commit_arguments,
                     3U,
                     NULL,
                     0U,
                     &exit_code);
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INTERNAL_ERROR;
    }
    status = run_git(root,
                     id_arguments,
                     2U,
                     output,
                     sizeof(output),
                     &exit_code);
    if (status == UMI_STATUS_OK && exit_code == 0 &&
        out_commit_id != NULL && capacity > 0U) {
        size_t length = strcspn(output, "\r\n");
        if (length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_commit_id, output, length);
        out_commit_id[length] = '\0';
    }
    return status;
}

static UmiStatus git_remote(void *instance,
                            const char *root,
                            const char *verb)
{
    const char *arguments[] = {verb, "--ff-only"};
    int exit_code = 0;
    size_t count = strcmp(verb, "pull") == 0 ? 2U : 1U;
    (void)instance;
    return run_git(root,
                   arguments,
                   count,
                   NULL,
                   0U,
                   &exit_code) == UMI_STATUS_OK && exit_code == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_INTERNAL_ERROR;
}

static UmiStatus git_pull(void *instance, const char *root)
{
    return git_remote(instance, root, "pull");
}

static UmiStatus git_push(void *instance, const char *root)
{
    return git_remote(instance, root, "push");
}

static UmiStatus git_branches(void *instance,
                              const char *root,
                              UmiVcsBranchList *out_branches)
{
    const char *arguments[] = {
        "branch", "--format=%(HEAD)%09%(refname:short)%09%(upstream:short)%09%(upstream:track)"
    };
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    UmiStatus status;
    (void)instance;
    if (root == NULL || out_branches == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = run_git_checked(root, arguments, 2U, output, sizeof(output));
    return status == UMI_STATUS_OK ? umi_vcs_branch_list_parse(out_branches, output) : status;
}

static UmiStatus git_remotes(void *instance,
                             const char *root,
                             UmiVcsRemoteList *out_remotes)
{
    const char *arguments[] = {"remote", "-v"};
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    UmiStatus status;
    (void)instance;
    if (root == NULL || out_remotes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = run_git_checked(root, arguments, 2U, output, sizeof(output));
    return status == UMI_STATUS_OK ? umi_vcs_remote_list_parse(out_remotes, output) : status;
}

static UmiStatus git_tags(void *instance,
                          const char *root,
                          UmiVcsTagList *out_tags)
{
    const char *arguments[] = {
        "for-each-ref", "refs/tags",
        "--format=%(refname:short)%09%(objectname)%09%(contents:subject)"
    };
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    UmiStatus status;
    (void)instance;
    if (root == NULL || out_tags == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = run_git_checked(root, arguments, 3U, output, sizeof(output));
    return status == UMI_STATUS_OK ? umi_vcs_tag_list_parse(out_tags, output) : status;
}

static UmiStatus git_diff_text(void *instance,
                               const char *root,
                               const char *path,
                               int staged,
                               char *out_text,
                               size_t capacity)
{
    const char *unstaged_arguments[] = {"diff", "--no-ext-diff", "--", path};
    const char *staged_arguments[] = {"diff", "--cached", "--no-ext-diff", "--", path};
    (void)instance;
    if (root == NULL || path == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return staged
        ? run_git_checked(root, staged_arguments, 5U, out_text, capacity)
        : run_git_checked(root, unstaged_arguments, 4U, out_text, capacity);
}

static UmiStatus git_stage_all(void *instance, const char *root)
{
    const char *arguments[] = {"add", "-A"};
    (void)instance;
    return root != NULL ? run_git_checked(root, arguments, 2U, NULL, 0U)
                        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus git_unstage_all(void *instance, const char *root)
{
    const char *arguments[] = {"restore", "--staged", "."};
    (void)instance;
    return root != NULL ? run_git_checked(root, arguments, 3U, NULL, 0U)
                        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus git_discard(void *instance, const char *root, const char *path)
{
    const char *arguments[] = {"restore", "--worktree", "--", path};
    (void)instance;
    if (root == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return run_git_checked(root, arguments, 4U, NULL, 0U);
}

static UmiStatus git_fetch(void *instance, const char *root)
{
    const char *arguments[] = {"fetch", "--all", "--prune"};
    (void)instance;
    return root != NULL ? run_git_checked(root, arguments, 3U, NULL, 0U)
                        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus git_validate_branch_name(const char *root, const char *name)
{
    const char *arguments[] = {"check-ref-format", "--branch", name};
    if (root == NULL || name == NULL || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return run_git_checked(root, arguments, 3U, NULL, 0U);
}

static UmiStatus git_branch_create(void *instance,
                                   const char *root,
                                   const char *name,
                                   int checkout)
{
    const char *create_arguments[] = {"branch", "--", name};
    const char *switch_arguments[] = {"switch", "-c", "--", name};
    UmiStatus status;
    (void)instance;
    status = git_validate_branch_name(root, name);
    if (status != UMI_STATUS_OK) return status;
    return checkout
        ? run_git_checked(root, switch_arguments, 4U, NULL, 0U)
        : run_git_checked(root, create_arguments, 3U, NULL, 0U);
}

static UmiStatus git_branch_checkout(void *instance,
                                     const char *root,
                                     const char *name)
{
    const char *arguments[] = {"switch", "--", name};
    UmiStatus status;
    (void)instance;
    status = git_validate_branch_name(root, name);
    return status == UMI_STATUS_OK
        ? run_git_checked(root, arguments, 3U, NULL, 0U) : status;
}

static UmiStatus git_branch_delete(void *instance,
                                   const char *root,
                                   const char *name,
                                   int force)
{
    const char *safe_arguments[] = {"branch", "-d", "--", name};
    const char *force_arguments[] = {"branch", "-D", "--", name};
    UmiStatus status;
    (void)instance;
    status = git_validate_branch_name(root, name);
    if (status != UMI_STATUS_OK) return status;
    return force
        ? run_git_checked(root, force_arguments, 4U, NULL, 0U)
        : run_git_checked(root, safe_arguments, 4U, NULL, 0U);
}

static void git_destroy(void *instance)
{
    free(instance);
}

UmiStatus umi_vcs_git_cli_provider(UmiVcsProvider *out_provider)
{
    UmiGitCliProvider *instance;
    if (out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    instance = calloc(1U, sizeof(*instance));
    if (instance == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)memset(out_provider, 0, sizeof(*out_provider));
    out_provider->structure_size = (uint32_t)sizeof(*out_provider);
    out_provider->provider_id = "umicom.vcs.git-cli";
    out_provider->instance = instance;
    out_provider->status = git_status;
    out_provider->history = git_history;
    out_provider->stage = git_stage;
    out_provider->unstage = git_unstage;
    out_provider->commit = git_commit;
    out_provider->pull = git_pull;
    out_provider->push = git_push;
    out_provider->destroy = git_destroy;
    out_provider->branches = git_branches;
    out_provider->remotes = git_remotes;
    out_provider->tags = git_tags;
    out_provider->diff = git_diff_text;
    out_provider->stage_all = git_stage_all;
    out_provider->unstage_all = git_unstage_all;
    out_provider->discard = git_discard;
    out_provider->fetch = git_fetch;
    out_provider->branch_create = git_branch_create;
    out_provider->branch_checkout = git_branch_checkout;
    out_provider->branch_delete = git_branch_delete;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_git_cli_is_repository(const char *root, int *out_is_repo)
{
    const char *arguments[] = {
        "rev-parse",
        "--is-inside-work-tree"
    };
    char output[128];
    int exit_code = 0;
    UmiStatus status;
    if (root == NULL || out_is_repo == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = run_git(root,
                     arguments,
                     2U,
                     output,
                     sizeof(output),
                     &exit_code);
    *out_is_repo = status == UMI_STATUS_OK &&
                   exit_code == 0 &&
                   strstr(output, "true") != NULL;
    return status == UMI_STATUS_OK
        ? UMI_STATUS_OK
        : status;
}
