/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_files.c
 *
 * PURPOSE:
 *   Implement exclusive project-entry creation behind the existing workspace
 *   authority, with checked names and platform-specific handle ownership.
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
#endif
#include "umicom/platform/workspace_files.h"

#include <ctype.h>
#include <errno.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/* Windows device aliases are invalid even with an extension, so a project
 * created on Linux can be checked out and edited on Windows as well. */
static int ReservedName(const char *name, size_t length)
{
    char stem[5] = {0};
    size_t count = 0U;
    while (count < length && name[count] != '.' && count < sizeof(stem) - 1U) {
        stem[count] = (char)toupper((unsigned char)name[count]);
        ++count;
    }
    if (count < length && name[count] != '.') return 0;
    return strcmp(stem, "CON") == 0 || strcmp(stem, "PRN") == 0 ||
        strcmp(stem, "AUX") == 0 || strcmp(stem, "NUL") == 0 ||
        (count == 4U && (memcmp(stem, "COM", 3U) == 0 ||
         memcmp(stem, "LPT", 3U) == 0) && stem[3] >= '1' && stem[3] <= '9');
}

/* Validate before normalising so parent traversal is never erased by cleanup.
 * This only checks path syntax; the OS operations also check actual parents. */
static UmiStatus ValidateRelative(const char *path, char *normalised)
{
    size_t length = 0U;
    size_t start = 0U;
    if (path == NULL || path[0] == '\0' || path[0] == '/' || path[0] == '\\')
        return UMI_STATUS_INVALID_ARGUMENT;
    while (length < UMI_PATH_CAPACITY && path[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (path[length - 1U] == '/' || path[length - 1U] == '\\')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t pos = 0U; pos <= length; ++pos) {
        unsigned char value = (unsigned char)path[pos];
        if (value == '\0' || value == '/' || value == '\\') {
            size_t count = pos - start;
            if (count == 0U || (count == 1U && path[start] == '.') ||
                (count == 2U && path[start] == '.' && path[start + 1U] == '.') ||
                path[pos - 1U] == '.' || path[pos - 1U] == ' ' ||
                ReservedName(path + start, count)) return UMI_STATUS_INVALID_ARGUMENT;
            if (count == 4U && path[start] == '.' &&
                tolower((unsigned char)path[start + 1U]) == 'g' &&
                tolower((unsigned char)path[start + 2U]) == 'i' &&
                tolower((unsigned char)path[start + 3U]) == 't')
                return UMI_STATUS_PERMISSION_DENIED;
            start = pos + 1U;
        } else if (value < 32U || value == 127U || strchr("<>:\"|?*", value) != NULL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return umi_path_normalise(path, normalised, UMI_PATH_CAPACITY);
}

#ifdef _WIN32
/* Translate only known OS outcomes; never turn denied access into success. */
static UmiStatus CreationError(DWORD error)
{
    if (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS)
        return UMI_STATUS_ALREADY_EXISTS;
    if (error == ERROR_PATH_NOT_FOUND || error == ERROR_FILE_NOT_FOUND)
        return UMI_STATUS_NOT_FOUND;
    if (error == ERROR_ACCESS_DENIED || error == ERROR_SHARING_VIOLATION)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}

/* Open and retain each parent without delete sharing. Existing directories may
 * be read or edited, but a reparse-point parent is never accepted here. */
static UmiStatus OpenParent(const wchar_t *path, HANDLE *handle)
{
    BY_HANDLE_FILE_INFORMATION info;
    *handle = CreateFileW(path, FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (*handle == INVALID_HANDLE_VALUE) return CreationError(GetLastError());
    if (!GetFileInformationByHandle(*handle, &info)) return UMI_STATUS_IO_ERROR;
    if ((info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Windows consumes UTF-16 paths. The public boundary continues to use UTF-8. */
static UmiStatus CreateEntry(const char *root, const char *relative,
    UmiWorkspaceEntryKind kind, UmiWorkspaceEntryResult *result)
{
    wchar_t path[UMI_PATH_CAPACITY];
    HANDLE parents[UMI_PATH_CAPACITY / 2U];
    size_t held = 0U;
    int rootLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
        root, -1, path, (int)UMI_PATH_CAPACITY);
    UmiStatus status;
    (void)relative;
    if (rootLength <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    status = OpenParent(path, &parents[held]);
    if (parents[held] != INVALID_HANDLE_VALUE) ++held;
    if (status != UMI_STATUS_OK) goto cleanup;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, result->path, -1,
        path, (int)UMI_PATH_CAPACITY) <= 0) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto cleanup;
    }
    /* Start after the already-held workspace root; retain each descendant. */
    for (size_t i = (size_t)rootLength; path[i] != L'\0'; ++i) {
        if (path[i] == L'/' || path[i] == L'\\') {
            wchar_t separator = path[i];
            if (held >= sizeof(parents) / sizeof(parents[0])) {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                goto cleanup;
            }
            path[i] = L'\0';
            status = OpenParent(path, &parents[held]);
            path[i] = separator;
            if (parents[held] != INVALID_HANDLE_VALUE) ++held;
            if (status != UMI_STATUS_OK) goto cleanup;
        }
    }
    if (kind == UMI_WORKSPACE_ENTRY_DIRECTORY) {
        if (!CreateDirectoryW(path, NULL)) status = CreationError(GetLastError());
        else result->created = 1;
    } else {
        HANDLE file = CreateFileW(path, GENERIC_WRITE, 0U, NULL, CREATE_NEW,
                                   FILE_ATTRIBUTE_NORMAL, NULL);
        if (file == INVALID_HANDLE_VALUE) status = CreationError(GetLastError());
        else {
            result->created = 1;
            if (!CloseHandle(file)) status = UMI_STATUS_IO_ERROR;
        }
    }
cleanup:
    while (held != 0U) (void)CloseHandle(parents[--held]);
    return status;
}
#else
/* Map exclusive-create and path failures without losing an existing file. */
static UmiStatus CreationError(int error)
{
    if (error == EEXIST) return UMI_STATUS_ALREADY_EXISTS;
    if (error == ENOENT) return UMI_STATUS_NOT_FOUND;
    if (error == EACCES || error == EPERM || error == ELOOP || error == ENOTDIR)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}

/* Descriptor-relative traversal keeps each operation in its opened parent.
 * O_NOFOLLOW rejects symlinks even if another process replaces a path entry. */
static UmiStatus CreateEntry(const char *root, const char *relative,
    UmiWorkspaceEntryKind kind, UmiWorkspaceEntryResult *result)
{
    char parts[UMI_PATH_CAPACITY];
    char *name = parts;
    char *separator;
    int parent = open(root, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    UmiStatus status = UMI_STATUS_OK;
    if (parent < 0) return CreationError(errno);
    (void)memcpy(parts, relative, strlen(relative) + 1U);
    while ((separator = strchr(name, '/')) != NULL) {
        int next;
        *separator = '\0';
        next = openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        if (next < 0) {
            status = CreationError(errno);
            goto cleanup;
        }
        (void)close(parent);
        parent = next;
        name = separator + 1;
    }
    if (kind == UMI_WORKSPACE_ENTRY_DIRECTORY) {
        if (mkdirat(parent, name, 0775) != 0) status = CreationError(errno);
        else result->created = 1;
    } else {
        int file = openat(parent, name,
            O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0666);
        if (file < 0) status = CreationError(errno);
        else {
            result->created = 1;
            if (close(file) != 0) status = UMI_STATUS_IO_ERROR;
        }
    }
cleanup:
    (void)close(parent);
    return status;
}
#endif

/* Authorise and resolve once, then create through the platform implementation. */
UmiStatus UmiWorkspaceCreateEntry(const UmiWorkspaceGraph *graph,
    uint64_t expectedRevision, const char *relativePath,
    UmiWorkspaceEntryKind kind, UmiWorkspaceEntryResult *outResult)
{
    UmiWorkspaceGraphSnapshot snapshot;
    char relative[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(outResult, 0, sizeof(*outResult));
    if (graph == NULL || (kind != UMI_WORKSPACE_ENTRY_FILE &&
        kind != UMI_WORKSPACE_ENTRY_DIRECTORY)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workspace_graph_snapshot(graph, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!snapshot.open) return UMI_STATUS_INVALID_STATE;
    if (expectedRevision != 0U && expectedRevision != snapshot.revision)
        return UMI_STATUS_BUSY;
    if (!snapshot.trusted) return UMI_STATUS_PERMISSION_DENIED;
    status = ValidateRelative(relativePath, relative);
    if (status != UMI_STATUS_OK) return status;
    status = umi_path_join(snapshot.root, relative, outResult->path,
                            sizeof(outResult->path));
    if (status != UMI_STATUS_OK) return status;
    return CreateEntry(snapshot.root, relative, kind, outResult);
}
