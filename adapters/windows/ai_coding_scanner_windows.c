/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/windows/ai_coding_scanner_windows.c
 *
 * PURPOSE:
 *   Implement bounded recursive repository scanning on Windows without leaking
 *   Win32 handles or path types through the public Framework API.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../../src/ai_coding_runtime/scanner_internal.h"

#include <stdio.h>
#include <string.h>

static UmiStatus scan_directory(
    const char *root,
    const char *relative,
    const UmiAiCodingIgnorePolicy *ignore_policy,
    UmiAiCodingScanVisitor visitor,
    void *user_data,
    size_t *file_count)
{
    char pattern[UMI_AI_CODING_RUNTIME_PATH_CAPACITY * 2U];
    WIN32_FIND_DATAA data;
    HANDLE handle;
    int written;

    written = relative[0] != '\0'
        ? snprintf(pattern, sizeof(pattern), "%s/%s/*", root, relative)
        : snprintf(pattern, sizeof(pattern), "%s/*", root);

    if (written < 0 || (size_t)written >= sizeof(pattern)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    handle = FindFirstFileA(pattern, &data);
    if (handle == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND
            ? UMI_STATUS_OK
            : UMI_STATUS_IO_ERROR;
    }

    do {
        char child_relative[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
        UmiAiCodingScanEntry item;
        ULARGE_INTEGER size;
        int descend = 1;
        UmiStatus status;

        if (strcmp(data.cFileName, ".") == 0 ||
            strcmp(data.cFileName, "..") == 0) {
            continue;
        }

        written = relative[0] != '\0'
            ? snprintf(
                child_relative, sizeof(child_relative),
                "%s/%s", relative, data.cFileName)
            : snprintf(
                child_relative, sizeof(child_relative),
                "%s", data.cFileName);

        if (written < 0 ||
            (size_t)written >= sizeof(child_relative)) {
            (void)FindClose(handle);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        if ((data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U) {
            continue;
        }

        (void)memset(&item, 0, sizeof(item));
        (void)snprintf(
            item.relative_path,
            sizeof(item.relative_path),
            "%s",
            child_relative);
        item.directory =
            (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0U;

        size.HighPart = data.nFileSizeHigh;
        size.LowPart = data.nFileSizeLow;
        item.byte_size = (uint64_t)size.QuadPart;

        if (umi_ai_coding_ignore_path(
                ignore_policy,
                item.relative_path,
                item.directory)) {
            continue;
        }

        status = visitor(user_data, &item, &descend);
        if (status != UMI_STATUS_OK) {
            (void)FindClose(handle);
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
                (void)FindClose(handle);
                return status;
            }
        } else if (!item.directory) {
            *file_count += 1U;
        }
    } while (FindNextFileA(handle, &data));

    if (GetLastError() != ERROR_NO_MORE_FILES) {
        (void)FindClose(handle);
        return UMI_STATUS_IO_ERROR;
    }

    return FindClose(handle) ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
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
