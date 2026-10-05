/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/directory_scan.c
 * PURPOSE: Keep recovery and other local catalogues bounded before collecting directory entries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/directory_scan.h"
#include "umicom/platform/output_file.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include "native_path_internal.h"
#else
#include <dirent.h>
#endif
static UmiStatus ScanChild(const char *root, const char *name, size_t maximum, size_t *visited,
                           UmiDirectoryVisitor visitor, void *context, const UmiCancellationToken *cancel)
{
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return UMI_STATUS_OK;
    if (*visited >= maximum)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++*visited;
    char path[UMI_PATH_CAPACITY];
    UmiFileInfo info;
    UmiStatus status = umi_path_join(root, name, path, sizeof(path));
    if (status == UMI_STATUS_OK)
        status = umi_directory_stat(path, &info);
    /* An entry removed while enumerating is absent, not an empty document. */
    if (status == UMI_STATUS_NOT_FOUND)
        return UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
    {
        info.depth = 1U;
        status = visitor(&info, context);
    }
    return status;
}
UmiStatus UmiDirectoryScanShallow(const char *root, size_t maximum, UmiDirectoryVisitor visitor,
                                  void *context, const UmiCancellationToken *cancel)
{
    if (root == NULL || visitor == NULL || maximum == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (maximum > UMI_DIRECTORY_SCAN_MAXIMUM_ENTRIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Validate the supplied spelling before any join can normalize dot segments. */
    size_t length = strlen(root);
    if (length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_PATH_CAPACITY - 32U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *separator = (root[length - 1U] == '/'
#ifdef _WIN32
                             || root[length - 1U] == '\\'
#endif
                             )
                                ? ""
                                : "/";
    char probe[UMI_PATH_CAPACITY];
    UmiFileInfo info;
    UmiStatus status = UMI_STATUS_OK;
    (void)snprintf(probe, sizeof(probe), "%s%sscan-check", root, separator);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(probe);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    status = umi_directory_stat(root, &info);
    if (status != UMI_STATUS_OK)
        return status;
    if (info.kind != UMI_FILE_KIND_DIRECTORY)
        return UMI_STATUS_PERMISSION_DENIED;
    size_t visited = 0U;
#ifdef _WIN32
    char pattern[UMI_PATH_CAPACITY];
    wchar_t *wide = NULL;
    status = umi_path_join(root, "*", pattern, sizeof(pattern));
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(pattern, &wide);
    if (status != UMI_STATUS_OK)
        return status;
    WIN32_FIND_DATAW entry;
    HANDLE handle = FindFirstFileW(wide, &entry);
    free(wide);
    if (handle == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    }
    for (;;)
    {
        char name[UMI_PATH_CAPACITY];
        status = UmiNativePathUtf8(entry.cFileName, name, sizeof(name));
        if (status == UMI_STATUS_OK)
            status = ScanChild(root, name, maximum, &visited, visitor, context, cancel);
        if (status != UMI_STATUS_OK)
            break;
        if (!FindNextFileW(handle, &entry))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
                status = UMI_STATUS_IO_ERROR;
            break;
        }
    }
    if (!FindClose(handle) && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
#else
    DIR *directory = opendir(root);
    if (directory == NULL)
        return UMI_STATUS_IO_ERROR;
    for (;;)
    {
        errno = 0;
        struct dirent *entry = readdir(directory);
        if (entry == NULL)
        {
            if (errno != 0)
                status = UMI_STATUS_IO_ERROR;
            break;
        }
        status = ScanChild(root, entry->d_name, maximum, &visited, visitor, context, cancel);
        if (status != UMI_STATUS_OK)
            break;
    }
    if (closedir(directory) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
#endif
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    return status;
}
