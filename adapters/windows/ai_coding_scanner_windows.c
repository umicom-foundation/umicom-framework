/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/windows/ai_coding_scanner_windows.c
 *
 * PURPOSE:
 *   Implement bounded recursive repository scanning on Windows without leaking
 *   Win32 handles or path types through the public Framework API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../../src/ai_coding_runtime/scanner_internal.h"

#include <stdio.h>
#include <string.h>

/* Provide the scan directory operation used by this module and its client applications. */
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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(pattern)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    handle = FindFirstFileA(pattern, &data);
    /* Apply this operation only while the related capability or state is available. */
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

        /* Use the stable identifier comparison to choose the matching record or policy. */
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

        /* Apply this branch only when its contract condition is satisfied. */
        if (written < 0 ||
            (size_t)written >= sizeof(child_relative)) {
            (void)FindClose(handle);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        /* Apply this branch only when its contract condition is satisfied. */
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

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_coding_ignore_path(
                ignore_policy,
                item.relative_path,
                item.directory)) {
            continue;
        }

        status = visitor(user_data, &item, &descend);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)FindClose(handle);
            return status;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (item.directory && descend) {
            status = scan_directory(
                root,
                child_relative,
                ignore_policy,
                visitor,
                user_data,
                file_count);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                (void)FindClose(handle);
                return status;
            }
        } else /* Apply this branch only when its contract condition is satisfied. */ if (!item.directory) {
            *file_count += 1U;
        }
    } /* Continue only while work remains available; the loop body advances the state on each pass. */ while (FindNextFileA(handle, &data));

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (GetLastError() != ERROR_NO_MORE_FILES) {
        (void)FindClose(handle);
        return UMI_STATUS_IO_ERROR;
    }

    return FindClose(handle) ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/*
 * Provide the ai coding platform scan workspace operation used by this module and its
 * client applications.
 */
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
