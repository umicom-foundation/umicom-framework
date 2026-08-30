/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/repository.c
 *
 * PURPOSE:
 *   Implement native repository creation through argument-based Git and GitHub
 *   CLI child processes.  Remote creation is always explicit and supports a
 *   dry-run so no repository or network state is changed accidentally.
 *
 *   Phase 5 also owns native submodule gitlink locking.  This deliberately
 *   replaces script-driven version-lock refreshes with Framework C23 code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/repository.h"
#include "umicom/repository/submodule_lock.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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
    (void)memset(&result, 0, sizeof(result));
    result.exit_code = -1;
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

/*
 * Repository operations use argument arrays rather than a shell command line.
 * For lock/update operations we additionally require a zero Git exit status;
 * process creation succeeding is not enough to prove the Git operation worked.
 */
static UmiStatus umi_repository_run_checked(
    const UmiToolInfo *tool,
    UmiEnvironmentPlan *environment,
    const char *working_directory,
    const char *const *arguments,
    size_t argument_count,
    int *out_exit_code,
    char *out_text,
    size_t capacity)
{
    int exit_code = -1;
    UmiStatus status = umi_repository_run(
        tool,
        environment,
        working_directory,
        arguments,
        argument_count,
        &exit_code,
        out_text,
        capacity);

    if (out_exit_code != NULL) {
        *out_exit_code = exit_code;
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return exit_code == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

static char *umi_repository_trim_left(char *text)
{
    if (text == NULL) return NULL;
    while (*text != '\0' && isspace((unsigned char)*text) != 0) {
        ++text;
    }
    return text;
}

static void umi_repository_trim_right(char *text)
{
    size_t length;

    if (text == NULL) return;
    length = strlen(text);
    while (length > 0U &&
           isspace((unsigned char)text[length - 1U]) != 0) {
        text[--length] = '\0';
    }
}

static UmiStatus umi_repository_copy_field(
    char *destination,
    size_t capacity,
    const char *source,
    size_t length)
{
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static UmiStatus umi_repository_parse_submodule_header(
    const char *line,
    char *out_name,
    size_t capacity)
{
    const char *first_quote;
    const char *second_quote;

    if (line == NULL || out_name == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_name[0] = '\0';
    if (strncmp(line, "[submodule", 10U) != 0) {
        return UMI_STATUS_NOT_FOUND;
    }
    first_quote = strchr(line, '"');
    if (first_quote == NULL) return UMI_STATUS_PARSE_ERROR;
    second_quote = strchr(first_quote + 1, '"');
    if (second_quote == NULL || second_quote == first_quote + 1) {
        return UMI_STATUS_PARSE_ERROR;
    }
    return umi_repository_copy_field(
        out_name,
        capacity,
        first_quote + 1,
        (size_t)(second_quote - (first_quote + 1)));
}

static UmiStatus umi_repository_parse_gitmodules(
    char *text,
    UmiRepositorySubmoduleLockReport *report)
{
    char current_name[UMI_REPOSITORY_TEXT_CAPACITY];
    char *cursor;

    if (text == NULL || report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    current_name[0] = '\0';
    cursor = text;

    while (cursor != NULL && *cursor != '\0') {
        char *line = cursor;
        char *newline = strchr(cursor, '\n');
        char *trimmed;

        if (newline != NULL) {
            *newline = '\0';
            cursor = newline + 1;
        } else {
            cursor = NULL;
        }

        trimmed = umi_repository_trim_left(line);
        umi_repository_trim_right(trimmed);
        if (trimmed == NULL || trimmed[0] == '\0' ||
            trimmed[0] == '#' || trimmed[0] == ';') {
            continue;
        }

        if (trimmed[0] == '[') {
            UmiStatus header_status = umi_repository_parse_submodule_header(
                trimmed,
                current_name,
                sizeof(current_name));
            if (header_status == UMI_STATUS_NOT_FOUND) {
                current_name[0] = '\0';
            } else if (header_status != UMI_STATUS_OK) {
                return header_status;
            }
            continue;
        }

        if (current_name[0] != '\0') {
            char *equals = strchr(trimmed, '=');
            if (equals != NULL) {
                char *key;
                char *value;
                UmiRepositorySubmoduleLockEntry *entry;
                UmiStatus status;

                *equals = '\0';
                key = umi_repository_trim_left(trimmed);
                umi_repository_trim_right(key);
                value = umi_repository_trim_left(equals + 1);
                umi_repository_trim_right(value);

                if (strcmp(key, "path") != 0) continue;
                if (value[0] == '\0') return UMI_STATUS_PARSE_ERROR;
                if (report->count >= UMI_REPOSITORY_SUBMODULE_LOCK_CAPACITY) {
                    return UMI_STATUS_CAPACITY_EXCEEDED;
                }

                entry = &report->entries[report->count];
                status = umi_repository_copy_field(
                    entry->name,
                    sizeof(entry->name),
                    current_name,
                    strlen(current_name));
                if (status != UMI_STATUS_OK) return status;
                status = umi_repository_copy_field(
                    entry->path,
                    sizeof(entry->path),
                    value,
                    strlen(value));
                if (status != UMI_STATUS_OK) return status;
                report->count += 1U;
            }
        }
    }

    return report->count > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

static UmiStatus umi_repository_extract_git_sha(
    const char *text,
    char *out_sha,
    size_t capacity)
{
    const char *cursor;
    size_t length = 0U;

    if (text == NULL || out_sha == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    cursor = text;
    while (*cursor != '\0' && isspace((unsigned char)*cursor) != 0) {
        ++cursor;
    }
    while (cursor[length] != '\0' &&
           isxdigit((unsigned char)cursor[length]) != 0) {
        ++length;
    }

    /* Git repositories normally use SHA-1 (40) or SHA-256 (64). */
    if ((length != 40U && length != 64U) || length >= capacity) {
        return UMI_STATUS_PARSE_ERROR;
    }
    if (cursor[length] != '\0' &&
        isspace((unsigned char)cursor[length]) == 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    return umi_repository_copy_field(out_sha, capacity, cursor, length);
}

static UmiStatus umi_repository_verify_gitlink(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    const char *submodule_path,
    UmiRepositorySubmoduleLockReport *report)
{
    const char *arguments[] = {
        "ls-files", "--stage", "--", submodule_path
    };
    UmiStatus status;

    status = umi_repository_run_checked(
        git,
        environment,
        repository_root,
        arguments,
        4U,
        &report->last_exit_code,
        report->last_output,
        sizeof(report->last_output));
    if (status != UMI_STATUS_OK) return status;

    /* Mode 160000 is the Git index mode reserved for submodule gitlinks. */
    return strncmp(report->last_output, "160000 ", 7U) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_STATE;
}

static UmiStatus umi_repository_resolve_submodule_head(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    UmiRepositorySubmoduleLockEntry *entry,
    UmiRepositorySubmoduleLockReport *report)
{
    const char *arguments[] = {
        "-C", entry->path, "rev-parse", "--verify", "HEAD"
    };
    char full_path[UMI_REPOSITORY_PATH_CAPACITY];
    UmiStatus status;

    status = umi_fs_join(
        full_path,
        sizeof(full_path),
        repository_root,
        entry->path);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_directory(full_path)) return UMI_STATUS_NOT_FOUND;

    status = umi_repository_run_checked(
        git,
        environment,
        repository_root,
        arguments,
        5U,
        &report->last_exit_code,
        report->last_output,
        sizeof(report->last_output));
    if (status != UMI_STATUS_OK) return status;

    return umi_repository_extract_git_sha(
        report->last_output,
        entry->head,
        sizeof(entry->head));
}

static UmiStatus umi_repository_stage_gitlink(
    const UmiToolInfo *git,
    UmiEnvironmentPlan *environment,
    const char *repository_root,
    UmiRepositorySubmoduleLockEntry *entry,
    UmiRepositorySubmoduleLockReport *report)
{
    const char *arguments[] = {"add", "--", entry->path};
    UmiStatus status = umi_repository_run_checked(
        git,
        environment,
        repository_root,
        arguments,
        3U,
        &report->last_exit_code,
        report->last_output,
        sizeof(report->last_output));

    if (status == UMI_STATUS_OK) {
        entry->staged = 1;
        report->staged_count += 1U;
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

UmiStatus umi_repository_submodule_lock(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositorySubmoduleLockRequest *request,
    UmiRepositorySubmoduleLockReport *out_report)
{
    const UmiToolInfo *git;
    char gitmodules_path[UMI_REPOSITORY_PATH_CAPACITY];
    char *gitmodules_text = NULL;
    size_t gitmodules_size = 0U;
    size_t index;
    UmiStatus status;

    if (profile == NULL || request == NULL || out_report == NULL ||
        request->repository_root == NULL ||
        request->repository_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    git = umi_toolchain_profile_tool(profile, UMI_TOOL_GIT);
    if (git == NULL || git->state != UMI_TOOL_VALIDATED) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (!umi_fs_is_directory(request->repository_root)) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_fs_join(
        gitmodules_path,
        sizeof(gitmodules_path),
        request->repository_root,
        ".gitmodules");
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_file(gitmodules_path)) return UMI_STATUS_NOT_FOUND;

    status = umi_fs_read_text(
        gitmodules_path,
        &gitmodules_text,
        &gitmodules_size);
    if (status != UMI_STATUS_OK) return status;
    (void)gitmodules_size;

    status = umi_repository_parse_gitmodules(gitmodules_text, out_report);
    if (status != UMI_STATUS_OK) {
        umi_fs_free_text(gitmodules_text);
        return status;
    }

    for (index = 0U; index < out_report->count; ++index) {
        UmiRepositorySubmoduleLockEntry *entry = &out_report->entries[index];

        status = umi_repository_verify_gitlink(
            git,
            environment,
            request->repository_root,
            entry->path,
            out_report);
        if (status != UMI_STATUS_OK) break;

        status = umi_repository_resolve_submodule_head(
            git,
            environment,
            request->repository_root,
            entry,
            out_report);
        if (status != UMI_STATUS_OK) break;

        out_report->locked_count += 1U;
        if (!request->dry_run) {
            status = umi_repository_stage_gitlink(
                git,
                environment,
                request->repository_root,
                entry,
                out_report);
            if (status != UMI_STATUS_OK) break;
        }
    }

    umi_fs_free_text(gitmodules_text);
    return status;
}
