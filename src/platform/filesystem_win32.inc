/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/filesystem_win32.inc
 * PURPOSE: Implement native Unicode file operations while retaining portable filesystem policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This fragment belongs to filesystem.c. Keeping encoding at this boundary
 * lets every application use UTF-8 without changing process-wide locale state. */
#include "native_path_internal.h"

static FILE *umi_native_fopen(const char *path, const char *mode)
{
    wchar_t *wide = NULL;
    if (UmiNativePathWide(path, &wide) != UMI_STATUS_OK)
        return NULL;
    /* These are the byte-preserving modes used by this translation unit. Path
     * conversion must never opt into CRT text or Unicode-content translation. */
    const wchar_t *nativeMode = strcmp(mode, "rb") == 0   ? L"rb"
                                : strcmp(mode, "wb") == 0 ? L"wb"
                                : strcmp(mode, "ab") == 0 ? L"ab"
                                                          : NULL;
    FILE *file = nativeMode == NULL ? NULL : _wfopen(wide, nativeMode);
    free(wide);
    return file;
}

static int umi_windows_stat(const char *path, struct _stat64 *info)
{
    wchar_t *wide = NULL;
    if (UmiNativePathWide(path, &wide) != UMI_STATUS_OK)
        return -1;
    int result = _wstat64(wide, info);
    free(wide);
    return result;
}

/* Resolve relative names once before a multi-step operation. This does not
 * change the current directory, resolve links or lock a parent against changes.
 * All following steps use the copied absolute native name. */
static UmiStatus umi_windows_full_path(const char *path, wchar_t **out)
{
    wchar_t *wide = NULL;
    UmiStatus status = UmiNativePathWide(path, &wide);
    if (status != UMI_STATUS_OK)
        return status;
    wchar_t *absolute = calloc(32768U, sizeof(*absolute));
    if (absolute == NULL)
    {
        free(wide);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    DWORD count = GetFullPathNameW(wide, 32768U, absolute, NULL);
    free(wide);
    if (count == 0U || count >= 32768U)
    {
        free(absolute);
        return count == 0U ? UMI_STATUS_IO_ERROR : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out = absolute;
    return UMI_STATUS_OK;
}

/* Drive and UNC roots are existing locations, not directories to create or
 * remove. Device namespaces are outside this ordinary filesystem contract. */
static size_t umi_windows_root_length(const wchar_t *path)
{
    size_t length = wcslen(path);
    if (length >= 3U && ((path[0] >= L'A' && path[0] <= L'Z') || (path[0] >= L'a' && path[0] <= L'z')) &&
        path[1] == L':' && path[2] == L'\\')
        return 3U;
    if (length < 5U || path[0] != L'\\' || path[1] != L'\\' || path[2] == L'?' || path[2] == L'.' ||
        path[2] == L'\\')
        return 0U;
    const wchar_t *share = wcschr(path + 2, L'\\');
    if (share == NULL || share[1] == L'\0' || share[1] == L'\\')
        return 0U;
    const wchar_t *end = wcschr(share + 1, L'\\');
    return end == NULL ? length : (size_t)(end - path) + 1U;
}

static UmiStatus umi_windows_ensure_directory(const wchar_t *path)
{
    DWORD attributes = GetFileAttributesW(path);
    if (attributes != INVALID_FILE_ATTRIBUTES)
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0U ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    if (CreateDirectoryW(path, NULL))
        return UMI_STATUS_OK;
    /* Another creator may have won the race. Accept an existing directory, but
     * do not mistake a same-named ordinary file for successful folder creation. */
    if (GetLastError() != ERROR_ALREADY_EXISTS)
        return UMI_STATUS_IO_ERROR;
    attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0U
               ? UMI_STATUS_OK
               : UMI_STATUS_IO_ERROR;
}

static UmiStatus umi_windows_make_directories(const char *path)
{
    wchar_t *absolute = NULL;
    UmiStatus status = umi_windows_full_path(path, &absolute);
    if (status != UMI_STATUS_OK)
        return status;
    size_t root = umi_windows_root_length(absolute);
    if (root == 0U)
    {
        free(absolute);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (wchar_t *part = absolute + root; *part != L'\0'; ++part)
    {
        if (*part != L'\\')
            continue;
        *part = L'\0';
        status = umi_windows_ensure_directory(absolute);
        *part = L'\\';
        if (status != UMI_STATUS_OK)
            break;
    }
    if (status == UMI_STATUS_OK)
        status = umi_windows_ensure_directory(absolute);
    free(absolute);
    return status;
}

/* Preserve the existing bounded retry policy for antivirus scanners and other
 * short-lived file users. A reparse point is removed as a leaf, never traversed. */
static UmiStatus umi_windows_remove_leaf(const wchar_t *path, DWORD attributes)
{
    if ((attributes & FILE_ATTRIBUTE_READONLY) != 0U)
        (void)SetFileAttributesW(path, attributes & ~(DWORD)FILE_ATTRIBUTE_READONLY);
    for (unsigned attempt = 0U; attempt < 40U; ++attempt)
    {
        BOOL removed =
            (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0U ? RemoveDirectoryW(path) : DeleteFileW(path);
        if (removed)
            return UMI_STATUS_OK;
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
            return UMI_STATUS_OK;
        if (error != ERROR_ACCESS_DENIED && error != ERROR_SHARING_VIOLATION &&
            error != ERROR_LOCK_VIOLATION && error != ERROR_DIR_NOT_EMPTY)
            return UMI_STATUS_IO_ERROR;
        Sleep(25U);
    }
    return UMI_STATUS_IO_ERROR;
}

static UmiStatus umi_windows_remove_contents(const wchar_t *path, size_t depth)
{
    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND ? UMI_STATUS_OK
                                                                              : UMI_STATUS_IO_ERROR;
    }
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0U || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U)
        return umi_windows_remove_leaf(path, attributes);
    /* Bound recursion even if another process continually changes the tree.
     * Earlier removals are not rolled back when this or another error occurs. */
    if (depth >= 128U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t length = wcslen(path);
    if (length > 32764U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    wchar_t *pattern = malloc((length + 3U) * sizeof(*pattern));
    if (pattern == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(pattern, path, length * sizeof(*pattern));
    memcpy(pattern + length, L"\\*", 3U * sizeof(*pattern));
    WIN32_FIND_DATAW entry;
    HANDLE handle = FindFirstFileW(pattern, &entry);
    DWORD firstError = handle == INVALID_HANDLE_VALUE ? GetLastError() : ERROR_SUCCESS;
    free(pattern);
    if (handle == INVALID_HANDLE_VALUE)
        return firstError == ERROR_FILE_NOT_FOUND ? umi_windows_remove_leaf(path, attributes)
                                                  : UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_OK;
    for (;;)
    {
        if (wcscmp(entry.cFileName, L".") != 0 && wcscmp(entry.cFileName, L"..") != 0)
        {
            size_t leaf = wcslen(entry.cFileName);
            if (leaf > 32766U - length)
            {
                status = UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            wchar_t *child = malloc((length + leaf + 2U) * sizeof(*child));
            if (child == NULL)
            {
                status = UMI_STATUS_OUT_OF_MEMORY;
                break;
            }
            memcpy(child, path, length * sizeof(*child));
            child[length] = L'\\';
            memcpy(child + length + 1U, entry.cFileName, (leaf + 1U) * sizeof(*child));
            status = umi_windows_remove_contents(child, depth + 1U);
            free(child);
            if (status != UMI_STATUS_OK)
                break;
        }
        if (!FindNextFileW(handle, &entry))
        {
            if (GetLastError() != ERROR_NO_MORE_FILES)
                status = UMI_STATUS_IO_ERROR;
            break;
        }
    }
    FindClose(handle);
    if (status == UMI_STATUS_OK)
        status = umi_windows_remove_leaf(path, attributes);
    return status;
}

static UmiStatus umi_windows_remove_tree(const char *path)
{
    wchar_t *absolute = NULL;
    UmiStatus status = umi_windows_full_path(path, &absolute);
    if (status != UMI_STATUS_OK)
        return status;
    size_t root = umi_windows_root_length(absolute), length = wcslen(absolute);
    while (length > root && absolute[length - 1U] == L'\\')
        absolute[--length] = L'\0';
    if (root == 0U || length <= root)
        status = UMI_STATUS_INVALID_ARGUMENT;
    else
        status = umi_windows_remove_contents(absolute, 0U);
    free(absolute);
    return status;
}

static UmiStatus umi_windows_rename(const char *source, const char *destination)
{
    wchar_t *from = NULL, *to = NULL;
    UmiStatus status = UmiNativePathWide(source, &from);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(destination, &to);
    /* The wide CRT retains Windows rename's existing-destination policy. Atomic
     * replacement has a separate contract and is not implied by this operation. */
    if (status == UMI_STATUS_OK && _wrename(from, to) != 0)
        status = UMI_STATUS_IO_ERROR;
    free(from);
    free(to);
    return status;
}

static UmiStatus umi_windows_current_directory(char *out, size_t capacity)
{
    wchar_t *wide = calloc(32768U, sizeof(*wide));
    if (wide == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    DWORD count = GetCurrentDirectoryW(32768U, wide);
    UmiStatus status = count == 0U       ? UMI_STATUS_IO_ERROR
                       : count >= 32768U ? UMI_STATUS_CAPACITY_EXCEEDED
                                         : UmiNativePathUtf8(wide, out, capacity);
    free(wide);
    return status;
}

static UmiStatus umi_windows_temp_directory(char *out, size_t capacity)
{
    /* Keep TEMP, TMP, then '.' precedence. Query the native environment rather
     * than a locale-dependent narrow CRT snapshot. No directory is created. */
    const wchar_t *names[] = {L"TEMP", L"TMP"};
    wchar_t *value = calloc(32768U, sizeof(*value));
    if (value == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < 2U; ++i)
    {
        SetLastError(ERROR_SUCCESS);
        DWORD count = GetEnvironmentVariableW(names[i], value, 32768U);
        if (count >= 32768U)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (count != 0U)
        {
            status = UmiNativePathUtf8(value, out, capacity);
            free(value);
            return status;
        }
        DWORD error = GetLastError();
        if (error != ERROR_ENVVAR_NOT_FOUND && error != ERROR_SUCCESS)
        {
            status = UMI_STATUS_IO_ERROR;
            break;
        }
    }
    free(value);
    return status == UMI_STATUS_OK ? umi_copy_string(out, capacity, ".") : status;
}
