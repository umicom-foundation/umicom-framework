/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/git_cli.c
 *
 * PURPOSE:
 *   Implement Git status, history, stage, unstage, commit, pull and push through safe argument arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Provide the run git operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text != NULL && capacity > 0U) out_text[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) *out_exit_code = -1;
    request.program = "git";
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.working_directory = root;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    status = umi_process_execute(&request, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text != NULL && capacity > 0U) {
        size_t output_length = strlen(result.output);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (result.output_truncated || output_length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_text, result.output, output_length + 1U);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_exit_code != NULL) {
        *out_exit_code = result.exit_code;
    }
    return status;
}

/* Provide the run git checked operation used by this module and its client applications. */
static UmiStatus run_git_checked(const char *root,
                                 const char *const *arguments,
                                 size_t argument_count,
                                 char *out_text,
                                 size_t capacity)
{
    int exit_code = -1;
    UmiStatus status = run_git(root, arguments, argument_count,
                               out_text, capacity, &exit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return exit_code == 0 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/* Provide the git status operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || out_changes == NULL || out_branch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = run_git(root,
                     arguments,
                     sizeof(arguments) / sizeof(arguments[0]),
                     output,
                     sizeof(output),
                     &exit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_UNAVAILABLE;
    }
    newline = strchr(output, '\n');
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strncmp(output, "## ", 3U) == 0) {
        char header[1024];
        size_t length = newline != NULL
            ? (size_t)(newline - output)
            : strlen(output);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(header)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(header, output, length);
        header[length] = '\0';
        status = umi_vcs_branch_parse_status_header(header, out_branch);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)memset(out_branch, 0, sizeof(*out_branch));
    }
    return umi_vcs_status_parse_porcelain(
        newline != NULL ? newline + 1 : "",
        out_changes,
        NULL
    );
}

/* Provide the git history operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_UNAVAILABLE;
    }
    return umi_vcs_history_parse(out_history, output, NULL);
}

/* Provide the git simple path operation used by this module and its client applications. */
static UmiStatus git_simple_path(const char *root,
                                 const char *verb,
                                 const char *path)
{
    const char *arguments[3];
    int exit_code = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || verb == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    arguments[0] = verb;
    arguments[1] = "--";
    arguments[2] = path;
    /* Apply this branch only when its contract condition is satisfied. */
    if (run_git(root, arguments, 3U, NULL, 0U, &exit_code) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_IO_ERROR;
    }
    return exit_code == 0 ? UMI_STATUS_OK : UMI_STATUS_INTERNAL_ERROR;
}

/* Provide the git stage operation used by this module and its client applications. */
static UmiStatus git_stage(void *instance,
                           const char *root,
                           const char *path)
{
    (void)instance;
    return git_simple_path(root, "add", path);
}

/* Provide the git unstage operation used by this module and its client applications. */
static UmiStatus git_unstage(void *instance,
                             const char *root,
                             const char *path)
{
    const char *arguments[] = {"restore", "--staged", "--", path};
    int exit_code = 0;
    (void)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the git commit operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || message == NULL || message[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = run_git(root,
                     commit_arguments,
                     3U,
                     NULL,
                     0U,
                     &exit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INTERNAL_ERROR;
    }
    status = run_git(root,
                     id_arguments,
                     2U,
                     output,
                     sizeof(output),
                     &exit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && exit_code == 0 &&
        out_commit_id != NULL && capacity > 0U) {
        size_t length = strcspn(output, "\r\n");
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length + 1U > capacity) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_commit_id, output, length);
        out_commit_id[length] = '\0';
    }
    return status;
}

/* Provide the git remote operation used by this module and its client applications. */
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

/* Provide the git pull operation used by this module and its client applications. */
static UmiStatus git_pull(void *instance, const char *root)
{
    return git_remote(instance, root, "pull");
}

/* Provide the git push operation used by this module and its client applications. */
static UmiStatus git_push(void *instance, const char *root)
{
    return git_remote(instance, root, "push");
}

/* Provide the git branches operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || out_branches == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = run_git_checked(root, arguments, 2U, output, sizeof(output));
    return status == UMI_STATUS_OK ? umi_vcs_branch_list_parse(out_branches, output) : status;
}

/* Provide the git remotes operation used by this module and its client applications. */
static UmiStatus git_remotes(void *instance,
                             const char *root,
                             UmiVcsRemoteList *out_remotes)
{
    const char *arguments[] = {"remote", "-v"};
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    UmiStatus status;
    (void)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || out_remotes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = run_git_checked(root, arguments, 2U, output, sizeof(output));
    return status == UMI_STATUS_OK ? umi_vcs_remote_list_parse(out_remotes, output) : status;
}

/* Provide the git tags operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || out_tags == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = run_git_checked(root, arguments, 3U, output, sizeof(output));
    return status == UMI_STATUS_OK ? umi_vcs_tag_list_parse(out_tags, output) : status;
}

/* Provide the git diff text operation used by this module and its client applications. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || path == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return staged
        ? run_git_checked(root, staged_arguments, 5U, out_text, capacity)
        : run_git_checked(root, unstaged_arguments, 4U, out_text, capacity);
}

/* Provide the git stage all operation used by this module and its client applications. */
static UmiStatus git_stage_all(void *instance, const char *root)
{
    const char *arguments[] = {"add", "-A"};
    (void)instance;
    return root != NULL ? run_git_checked(root, arguments, 2U, NULL, 0U)
                        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the git unstage all operation used by this module and its client applications. */
static UmiStatus git_unstage_all(void *instance, const char *root)
{
    const char *arguments[] = {"restore", "--staged", "."};
    (void)instance;
    return root != NULL ? run_git_checked(root, arguments, 3U, NULL, 0U)
                        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the git discard operation used by this module and its client applications. */
static UmiStatus git_discard(void *instance, const char *root, const char *path)
{
    const char *arguments[] = {"restore", "--worktree", "--", path};
    (void)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return run_git_checked(root, arguments, 4U, NULL, 0U);
}

/* Provide the git fetch operation used by this module and its client applications. */
static UmiStatus git_fetch(void *instance, const char *root)
{
    const char *arguments[] = {"fetch", "--all", "--prune"};
    (void)instance;
    return root != NULL ? run_git_checked(root, arguments, 3U, NULL, 0U)
                        : UMI_STATUS_INVALID_ARGUMENT;
}

/*
 * Provide the git validate branch name operation used by this module and its client
 * applications.
 */
static UmiStatus git_validate_branch_name(const char *root, const char *name)
{
    const char *arguments[] = {"check-ref-format", "--branch", name};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || name == NULL || name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return run_git_checked(root, arguments, 3U, NULL, 0U);
}

/*
 * Initialise git branch from caller-provided values so later operations receive a known
 * state.
 */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return checkout
        ? run_git_checked(root, switch_arguments, 4U, NULL, 0U)
        : run_git_checked(root, create_arguments, 3U, NULL, 0U);
}

/*
 * Provide the git branch checkout operation used by this module and its client
 * applications.
 */
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

/* Provide the git branch delete operation used by this module and its client applications. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return force
        ? run_git_checked(root, force_arguments, 4U, NULL, 0U)
        : run_git_checked(root, safe_arguments, 4U, NULL, 0U);
}

/* Release or reset state held by git so the same storage can be reused safely. */
static void git_destroy(void *instance)
{
    free(instance);
}

/*
 * Provide the vcs git cli provider operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_git_cli_provider(UmiVcsProvider *out_provider)
{
    UmiGitCliProvider *instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    instance = calloc(1U, sizeof(*instance));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the vcs git cli is repository operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_git_cli_is_repository(const char *root, int *out_is_repo)
{
    const char *arguments[] = {
        "rev-parse",
        "--is-inside-work-tree"
    };
    char output[128];
    int exit_code = 0;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
