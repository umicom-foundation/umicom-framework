/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/posix/ai_coding_scanner_posix.c
 *
 * PURPOSE:
 *   Implement bounded recursive repository scanning on POSIX systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L

#include "../../src/ai_coding_runtime/scanner_internal.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static UmiStatus scan_directory(
    const char *root,
    const char *relative,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiCodingScanVisitor visitor,
    void *user_data,
    size_t *file_count)
{
    char directory_path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
    DIR *directory;
    struct dirent *entry;
    int written;

    written = relative[0] != '\0'
        ? snprintf(directory_path, sizeof(directory_path), "%s/%s", root, relative)
        : snprintf(directory_path, sizeof(directory_path), "%s", root);

    if (written < 0 || (size_t)written >= sizeof(directory_path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    directory = opendir(directory_path);
    if (directory == NULL) return UMI_STATUS_IO_ERROR;

    while ((entry = readdir(directory)) != NULL) {
        char child_relative[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
        char child_full[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
        struct stat info;
        UmiAiCodingScanEntry item;
        int descend = 1;
        UmiStatus status;

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        written = relative[0] != '\0'
            ? snprintf(
                child_relative, sizeof(child_relative),
                "%s/%s", relative, entry->d_name)
            : snprintf(
                child_relative, sizeof(child_relative),
                "%s", entry->d_name);

        if (written < 0 ||
            (size_t)written >= sizeof(child_relative)) {
            (void)closedir(directory);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        written = snprintf(
            child_full, sizeof(child_full),
            "%s/%s", root, child_relative);
        if (written < 0 ||
            (size_t)written >= sizeof(child_full)) {
            (void)closedir(directory);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        if (lstat(child_full, &info) != 0) {
            if (errno == ENOENT) continue;
            (void)closedir(directory);
            return UMI_STATUS_IO_ERROR;
        }

        if (S_ISLNK(info.st_mode)) {
            continue;
        }

        (void)memset(&item, 0, sizeof(item));
        (void)snprintf(
            item.relative_path,
            sizeof(item.relative_path),
            "%s",
            child_relative);
        item.directory = S_ISDIR(info.st_mode) ? 1 : 0;
        item.byte_size =
            info.st_size > 0 ? (uint64_t)info.st_size : 0U;

        if (umi_ai_coding_ignore_path(
                ignore_policy,
                item.relative_path,
                item.directory)) {
            continue;
        }

        status = visitor(user_data, &item, &descend);
        if (status != UMI_STATUS_OK) {
            (void)closedir(directory);
            return status;
        }

        if (item.directory && descend) {
            status = scan_directory(
                root,
                child_relative,
                ignore_policy,
                visitor,
                user_data,
                file_count);
            if (status != UMI_STATUS_OK) {
                (void)closedir(directory);
                return status;
            }
        } else if (!item.directory) {
            *file_count += 1U;
        }
    }

    return closedir(directory) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_ai_coding_platform_scan_workspace(
    const char *root,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiCodingScanVisitor visitor,
    void *user_data,
    size_t *out_file_count)
{
    return scan_directory(
        root,
        "",
        ignore_policy,
        visitor,
        user_data,
        out_file_count);
}
#endif
