/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/scaffold/repository.c
 *
 * PURPOSE:
 *   Render a complete repository from the Framework repository template,
 *   initialise Git, attach Framework as a submodule when selected, and create
 *   an optional GitHub repository only after an explicit request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/scaffold/repository.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

#define UMI_SCAFFOLD_TEMPLATE_CAPACITY 262144U

static UmiStatus umi_scaffold_replace(char *text,
                                      size_t capacity,
                                      const char *token,
                                      const char *replacement)
{
    char *buffer;
    char *cursor;
    char *match;
    size_t used = 0U;
    size_t token_length;
    size_t replacement_length;

    if (text == NULL || token == NULL || replacement == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    buffer = (char *)malloc(capacity);
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    token_length = strlen(token);
    replacement_length = strlen(replacement);
    cursor = text;

    while ((match = strstr(cursor, token)) != NULL) {
        size_t prefix = (size_t)(match - cursor);
        if (used + prefix + replacement_length + 1U > capacity) {
            free(buffer);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(buffer + used, cursor, prefix);
        used += prefix;
        (void)memcpy(buffer + used, replacement, replacement_length);
        used += replacement_length;
        cursor = match + token_length;
    }
    if (used + strlen(cursor) + 1U > capacity) {
        free(buffer);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)strcpy(buffer + used, cursor);
    (void)snprintf(text, capacity, "%s", buffer);
    free(buffer);
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_scaffold_slug(const char *name,
                                       char *out_slug,
                                       size_t capacity)
{
    size_t read_index;
    size_t write_index = 0U;
    int previous_dash = 0;

    if (name == NULL || out_slug == NULL || capacity < 2U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_index = 0U; name[read_index] != '\0'; ++read_index) {
        unsigned char value = (unsigned char)name[read_index];
        if (isalnum(value)) {
            if (write_index + 1U >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_slug[write_index++] = (char)tolower(value);
            previous_dash = 0;
        } else if (!previous_dash && write_index > 0U) {
            if (write_index + 1U >= capacity) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_slug[write_index++] = '-';
            previous_dash = 1;
        }
    }
    while (write_index > 0U && out_slug[write_index - 1U] == '-') {
        --write_index;
    }
    out_slug[write_index] = '\0';
    return write_index > 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

static void umi_scaffold_identifier(const char *slug,
                                    char *out_identifier,
                                    size_t capacity)
{
    size_t index;
    for (index = 0U; slug[index] != '\0' && index + 1U < capacity; ++index) {
        out_identifier[index] = slug[index] == '-' ? '_' : slug[index];
    }
    out_identifier[index] = '\0';
}

static UmiStatus umi_scaffold_render_text(
    const UmiRepositoryScaffoldRequest *request,
    const UmiRepositoryScaffoldReport *report,
    const char *source,
    const char *destination)
{
    char *text;
    char identifier[UMI_REPOSITORY_TEXT_CAPACITY];
    char frontend_console[8];
    char frontend_gtk[8];
    char frontend_web[8];
    UmiStatus status = umi_fs_read_text(source, &text, NULL);

    if (status != UMI_STATUS_OK) return status;
    if (strlen(text) + 8192U > UMI_SCAFFOLD_TEMPLATE_CAPACITY) {
        umi_fs_free_text(text);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    {
        char *expanded = (char *)calloc(UMI_SCAFFOLD_TEMPLATE_CAPACITY, 1U);
        if (expanded == NULL) {
            umi_fs_free_text(text);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        (void)snprintf(expanded,
                       UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                       "%s",
                       text);
        umi_fs_free_text(text);
        text = expanded;
    }

    umi_scaffold_identifier(report->application_slug,
                            identifier,
                            sizeof(identifier));
    (void)snprintf(frontend_console,
                   sizeof(frontend_console),
                   "%s",
                   (request->frontends & UMI_FRONTEND_CONSOLE) != 0U
                       ? "ON" : "OFF");
    (void)snprintf(frontend_gtk,
                   sizeof(frontend_gtk),
                   "%s",
                   (request->frontends & UMI_FRONTEND_GTK4) != 0U
                       ? "ON" : "OFF");
    (void)snprintf(frontend_web,
                   sizeof(frontend_web),
                   "%s",
                   (request->frontends & UMI_FRONTEND_WEB) != 0U
                       ? "ON" : "OFF");

    status = umi_scaffold_replace(text,
                                  UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                  "@APP_NAME@",
                                  report->application_name);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(text,
                                      UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                      "@APP_ID@",
                                      report->application_id);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(text,
                                      UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                      "@APP_SLUG@",
                                      report->application_slug);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(text,
                                      UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                      "@APP_IDENTIFIER@",
                                      identifier);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(text,
                                      UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                      "@ENABLE_CONSOLE@",
                                      frontend_console);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(text,
                                      UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                      "@ENABLE_GTK@",
                                      frontend_gtk);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(text,
                                      UMI_SCAFFOLD_TEMPLATE_CAPACITY,
                                      "@ENABLE_WEB@",
                                      frontend_web);
    if (status == UMI_STATUS_OK)
        status = umi_scaffold_replace(
            text,
            UMI_SCAFFOLD_TEMPLATE_CAPACITY,
            "@FRAMEWORK_MODE@",
            request->framework_link == UMI_FRAMEWORK_LINK_SUBMODULE
                ? "submodule"
                : "installed"
        );
    if (status == UMI_STATUS_OK) {
        status = umi_fs_write_text(destination, text);
    }
    free(text);
    return status;
}

static UmiStatus umi_scaffold_render_path(
    const UmiRepositoryScaffoldReport *report,
    const char *relative,
    char *out_relative,
    size_t capacity)
{
    char identifier[UMI_REPOSITORY_TEXT_CAPACITY];
    int written;
    umi_scaffold_identifier(report->application_slug,
                            identifier,
                            sizeof(identifier));
    written = snprintf(out_relative, capacity, "%s", relative);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_scaffold_replace(out_relative,
                             capacity,
                             "@APP_SLUG@",
                             report->application_slug) != UMI_STATUS_OK ||
        umi_scaffold_replace(out_relative,
                             capacity,
                             "@APP_IDENTIFIER@",
                             identifier) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

static UmiStatus umi_scaffold_copy_tree(
    const UmiRepositoryScaffoldRequest *request,
    UmiRepositoryScaffoldReport *report,
    const char *source_root,
    const char *relative_root)
{
    char source_directory[UMI_PATH_CAPACITY];
    char rendered_relative[UMI_PATH_CAPACITY];
    char destination_directory[UMI_PATH_CAPACITY];
    DIR *directory;
    struct dirent *entry;
    UmiStatus status;

    status = umi_fs_join(source_directory,
                         sizeof(source_directory),
                         source_root,
                         relative_root);
    if (status != UMI_STATUS_OK) return status;
    status = umi_scaffold_render_path(report,
                                      relative_root,
                                      rendered_relative,
                                      sizeof(rendered_relative));
    if (status != UMI_STATUS_OK) return status;
    status = umi_fs_join(destination_directory,
                         sizeof(destination_directory),
                         report->repository_root,
                         rendered_relative);
    if (status != UMI_STATUS_OK) return status;
    status = umi_fs_make_directories(destination_directory);
    if (status != UMI_STATUS_OK) return status;
    report->directories_created += 1U;

    directory = opendir(source_directory);
    if (directory == NULL) return UMI_STATUS_IO_ERROR;
    while ((entry = readdir(directory)) != NULL) {
        char source_path[UMI_PATH_CAPACITY];
        char child_relative[UMI_PATH_CAPACITY];
        char rendered_child[UMI_PATH_CAPACITY];
        char destination_path[UMI_PATH_CAPACITY];
        const char *name = entry->d_name;
        size_t name_length;

        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        status = umi_fs_join(source_path,
                             sizeof(source_path),
                             source_directory,
                             name);
        if (status != UMI_STATUS_OK) break;
        status = umi_fs_join(child_relative,
                             sizeof(child_relative),
                             relative_root,
                             name);
        if (status != UMI_STATUS_OK) break;
        if (umi_fs_is_directory(source_path)) {
            status = umi_scaffold_copy_tree(request,
                                            report,
                                            source_root,
                                            child_relative);
            if (status != UMI_STATUS_OK) break;
            continue;
        }
        status = umi_scaffold_render_path(report,
                                          child_relative,
                                          rendered_child,
                                          sizeof(rendered_child));
        if (status != UMI_STATUS_OK) break;
        name_length = strlen(rendered_child);
        if (name_length > 3U &&
            strcmp(rendered_child + name_length - 3U, ".in") == 0) {
            rendered_child[name_length - 3U] = '\0';
        }
        status = umi_fs_join(destination_path,
                             sizeof(destination_path),
                             report->repository_root,
                             rendered_child);
        if (status != UMI_STATUS_OK) break;
        status = umi_scaffold_render_text(request,
                                          report,
                                          source_path,
                                          destination_path);
        if (status != UMI_STATUS_OK) break;
        report->files_created += 1U;
    }
    (void)closedir(directory);
    return status;
}

UmiStatus umi_repository_scaffold_create(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryScaffoldRequest *request,
    UmiRepositoryScaffoldReport *out_report)
{
    UmiRepositoryRequest repository_request;
    UmiStatus status;
    const char *framework_url;

    if (profile == NULL || request == NULL || out_report == NULL ||
        request->template_root == NULL ||
        request->destination_parent == NULL ||
        request->application_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_report, 0, sizeof(*out_report));
    (void)snprintf(out_report->application_name,
                   sizeof(out_report->application_name),
                   "%s",
                   request->application_name);
    status = umi_repository_scaffold_slug(
        request->repository_name != NULL
            ? request->repository_name
            : request->application_name,
        out_report->repository_name,
        sizeof(out_report->repository_name)
    );
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(out_report->application_slug,
                   sizeof(out_report->application_slug),
                   "%s",
                   out_report->repository_name);
    if (request->application_id != NULL &&
        request->application_id[0] != '\0') {
        (void)snprintf(out_report->application_id,
                       sizeof(out_report->application_id),
                       "%s",
                       request->application_id);
    } else {
        static const char prefix[] = "org.umicom.";
        size_t prefix_length = sizeof(prefix) - 1U;
        size_t slug_length = strlen(out_report->application_slug);
        if (prefix_length + slug_length >=
            sizeof(out_report->application_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)memcpy(out_report->application_id,
                     prefix,
                     prefix_length);
        (void)memcpy(out_report->application_id + prefix_length,
                     out_report->application_slug,
                     slug_length + 1U);
    }
    status = umi_fs_join(out_report->repository_root,
                         sizeof(out_report->repository_root),
                         request->destination_parent,
                         out_report->repository_name);
    if (status != UMI_STATUS_OK) return status;

    if (request->dry_run) {
        return UMI_STATUS_OK;
    }
    if (umi_fs_exists(out_report->repository_root)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    status = umi_fs_make_directories(out_report->repository_root);
    if (status != UMI_STATUS_OK) return status;
    status = umi_scaffold_copy_tree(request,
                                    out_report,
                                    request->template_root,
                                    ".");
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&repository_request, 0, sizeof(repository_request));
    repository_request.repository_root = out_report->repository_root;
    repository_request.remote_owner = request->remote_owner;
    repository_request.remote_name = out_report->repository_name;
    repository_request.remote_description = request->remote_description;
    repository_request.visibility = request->visibility;
    repository_request.initialise_local = request->initialise_git;
    repository_request.create_initial_commit = 0;
    repository_request.create_remote = request->create_remote;
    repository_request.push_initial = request->push_initial;
    status = umi_repository_initialise(profile,
                                       environment,
                                       &repository_request,
                                       &out_report->repository_report);
    if (status != UMI_STATUS_OK) return status;

    if (request->framework_link == UMI_FRAMEWORK_LINK_SUBMODULE) {
        framework_url = request->framework_url != NULL
            ? request->framework_url
            : "https://github.com/umicom-foundation/umicom-framework.git";
        status = umi_repository_add_submodule(
            profile,
            environment,
            out_report->repository_root,
            framework_url,
            "framework",
            request->framework_branch != NULL
                ? request->framework_branch
                : "main",
            0
        );
        if (status != UMI_STATUS_OK) return status;
        out_report->framework_linked = 1;
    } else if (request->framework_link == UMI_FRAMEWORK_LINK_VENDORED) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }

    if (request->create_initial_commit) {
        repository_request.initialise_local = 0;
        repository_request.create_initial_commit = 1;
        status = umi_repository_initialise(profile,
                                           environment,
                                           &repository_request,
                                           &out_report->repository_report);
        if (status != UMI_STATUS_OK) return status;
    }
    if (request->create_remote) {
        status = umi_repository_create_remote(profile,
                                              environment,
                                              &repository_request,
                                              &out_report->repository_report);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
