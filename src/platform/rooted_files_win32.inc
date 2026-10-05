/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_files_win32.inc
 * PURPOSE: Hold Windows workspace parents while accessing regular Unicode files and publishing staged edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus RootedError(DWORD error)
{
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
        return UMI_STATUS_NOT_FOUND;
    if (error == ERROR_ALREADY_EXISTS || error == ERROR_FILE_EXISTS)
        return UMI_STATUS_ALREADY_EXISTS;
    if (error == ERROR_ACCESS_DENIED || error == ERROR_SHARING_VIOLATION || error == ERROR_LOCK_VIOLATION)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
static void RootedClose(RootedParent *parent)
{
    while (parent->held != 0U)
        (void)CloseHandle(parent->parents[--parent->held]);
}
static UmiStatus RootedHoldDirectory(RootedParent *parent, const wchar_t *path)
{
    if (parent->held >= sizeof(parent->parents) / sizeof(parent->parents[0]))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    HANDLE handle =
        CreateFileW(path, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (handle == INVALID_HANDLE_VALUE)
        return RootedError(GetLastError());
    parent->parents[parent->held++] = handle;
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(handle, &info))
        return UMI_STATUS_IO_ERROR;
    if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0U ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}
static UmiStatus RootedOpenParent(const char *root, const char *relative, RootedParent *parent)
{
    char normalRoot[UMI_PATH_CAPACITY], full[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(root, normalRoot, sizeof(normalRoot));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(normalRoot, relative, full, sizeof(full));
    if (status != UMI_STATUS_OK)
        return status;
    wchar_t *nativeRoot = NULL, *nativeFull = NULL;
    status = UmiNativePathWide(normalRoot, &nativeRoot);
    if (status == UMI_STATUS_OK)
        status = UmiNativePathWide(full, &nativeFull);
    if (status != UMI_STATUS_OK)
    {
        free(nativeRoot);
        free(nativeFull);
        return status;
    }
    size_t rootLength = wcslen(nativeRoot), length = wcslen(nativeFull);
    if (length >= UMI_PATH_CAPACITY)
    {
        free(nativeRoot);
        free(nativeFull);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(parent->path, nativeFull, (length + 1U) * sizeof(*nativeFull));
    free(nativeFull);
    status = RootedHoldDirectory(parent, nativeRoot);
    free(nativeRoot);
    /* Root is already held. Retain every descendant parent, without delete
     * sharing, before publishing or removing any leaf beneath that chain. */
    for (size_t i = rootLength; status == UMI_STATUS_OK && i < length; ++i)
    {
        if (parent->path[i] != L'\\' && parent->path[i] != L'/')
            continue;
        if (i == rootLength)
            continue;
        wchar_t saved = parent->path[i];
        parent->path[i] = L'\0';
        status = RootedHoldDirectory(parent, parent->path);
        parent->path[i] = saved;
    }
    return status;
}
static UmiStatus RootedInspectHandle(HANDLE file, BY_HANDLE_FILE_INFORMATION *info)
{
    if (GetFileType(file) != FILE_TYPE_DISK || !GetFileInformationByHandle(file, info))
        return UMI_STATUS_IO_ERROR;
    return (info->dwFileAttributes &
            (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DEVICE)) == 0U
               ? UMI_STATUS_OK
               : UMI_STATUS_PERMISSION_DENIED;
}
static UmiStatus RootedInspect(RootedParent *parent, DWORD *outAttributes)
{
    DWORD attributes = GetFileAttributesW(parent->path);
    if (attributes == INVALID_FILE_ATTRIBUTES)
        return RootedError(GetLastError());
    if ((attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DEVICE)) !=
        0U)
        return UMI_STATUS_PERMISSION_DENIED;
    *outAttributes = attributes;
    return UMI_STATUS_OK;
}
static UmiStatus RootedExists(RootedParent *parent)
{
    DWORD attributes;
    return RootedInspect(parent, &attributes);
}
static UmiStatus RootedRead(RootedParent *parent, size_t maximum, unsigned char **out, size_t *outSize)
{
    HANDLE file = CreateFileW(
        parent->path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return RootedError(GetLastError());
    BY_HANDLE_FILE_INFORMATION before, after;
    UmiStatus status = RootedInspectHandle(file, &before);
    unsigned char *bytes = NULL;
    size_t size = 0U, used = 0U;
    if (status != UMI_STATUS_OK)
        goto done;
    ULARGE_INTEGER length;
    length.HighPart = before.nFileSizeHigh;
    length.LowPart = before.nFileSizeLow;
    if (length.QuadPart > maximum)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    size = (size_t)length.QuadPart;
    bytes = malloc(size + 1U);
    if (bytes == NULL)
    {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    while (used < size)
    {
        size_t remaining = size - used;
        if (remaining > 1048576U)
            remaining = 1048576U;
        DWORD count = 0U;
        if (!ReadFile(file, bytes + used, (DWORD)remaining, &count, NULL) || count == 0U)
        {
            status = UMI_STATUS_IO_ERROR;
            goto done;
        }
        used += count;
    }
    unsigned char extra;
    DWORD count = 0U;
    if (!ReadFile(file, &extra, 1U, &count, NULL) || !GetFileInformationByHandle(file, &after))
    {
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    if (count != 0U || before.nFileSizeHigh != after.nFileSizeHigh ||
        before.nFileSizeLow != after.nFileSizeLow ||
        CompareFileTime(&before.ftLastWriteTime, &after.ftLastWriteTime) != 0)
        status = UMI_STATUS_BUSY;
done:
    if (!CloseHandle(file) && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
    {
        bytes[size] = 0U;
        *out = bytes;
        *outSize = size;
    }
    else
        free(bytes);
    return status;
}
static UmiStatus RootedWrite(RootedParent *parent, const void *bytes, size_t size)
{
    DWORD attributes = 0U;
    UmiStatus status = RootedInspect(parent, &attributes);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
        return status;
    if (status == UMI_STATUS_OK && (attributes & FILE_ATTRIBUTE_READONLY) != 0U)
        return UMI_STATUS_PERMISSION_DENIED;
    wchar_t stage[UMI_PATH_CAPACITY];
    const wchar_t *separator = wcsrchr(parent->path, L'\\');
    if (separator == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t prefix = (size_t)(separator - parent->path) + 1U;
    HANDLE file = INVALID_HANDLE_VALUE;
    for (unsigned attempt = 0U; attempt < 64U; ++attempt)
    {
        unsigned long sequence = atomic_fetch_add_explicit(&rootedSequence, 1UL, memory_order_relaxed);
        char leaf[96];
        wchar_t *wideLeaf = NULL;
        int length = snprintf(leaf, sizeof(leaf), ".umicom-write-%lu-%lu",
                              (unsigned long)GetCurrentProcessId(), sequence);
        if (length < 0 || (size_t)length >= sizeof(leaf))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = UmiNativePathWide(leaf, &wideLeaf);
        if (status != UMI_STATUS_OK)
            return status;
        size_t leafLength = wcslen(wideLeaf);
        if (prefix + leafLength >= UMI_PATH_CAPACITY)
        {
            free(wideLeaf);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        memcpy(stage, parent->path, prefix * sizeof(*stage));
        memcpy(stage + prefix, wideLeaf, (leafLength + 1U) * sizeof(*stage));
        free(wideLeaf);
        /* Compare using Windows ordinal case rules: a user-selected leaf
         * must never double as its own still-incomplete staging file. */
        if (CompareStringOrdinal(stage, -1, parent->path, -1, TRUE) == CSTR_EQUAL)
            continue;
        file = CreateFileW(stage, GENERIC_WRITE, 0U, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        if (file != INVALID_HANDLE_VALUE)
            break;
        status = RootedError(GetLastError());
        if (status != UMI_STATUS_ALREADY_EXISTS)
            return status;
    }
    if (file == INVALID_HANDLE_VALUE)
        return UMI_STATUS_ALREADY_EXISTS;
    status = UMI_STATUS_OK;
    size_t used = 0U;
    while (used < size)
    {
        size_t remaining = size - used;
        if (remaining > 1048576U)
            remaining = 1048576U;
        DWORD count = 0U;
        if (!WriteFile(file, (const unsigned char *)bytes + used, (DWORD)remaining, &count, NULL) ||
            count == 0U)
        {
            status = UMI_STATUS_IO_ERROR;
            break;
        }
        used += count;
    }
    if (status == UMI_STATUS_OK && !FlushFileBuffers(file))
        status = UMI_STATUS_IO_ERROR;
    if (!CloseHandle(file) && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
    {
        status = RootedInspect(parent, &attributes);
        if (status == UMI_STATUS_OK && (attributes & FILE_ATTRIBUTE_READONLY) != 0U)
            status = UMI_STATUS_PERMISSION_DENIED;
        if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK &&
        !MoveFileExW(stage, parent->path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        status = RootedError(GetLastError());
    if (status != UMI_STATUS_OK)
        (void)DeleteFileW(stage);
    return status;
}
static UmiStatus RootedRemove(RootedParent *parent)
{
    HANDLE file = CreateFileW(parent->path, DELETE | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL,
                              OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return RootedError(GetLastError());
    BY_HANDLE_FILE_INFORMATION info;
    UmiStatus status = RootedInspectHandle(file, &info);
    if (status == UMI_STATUS_OK)
    {
        FILE_DISPOSITION_INFO disposition = {TRUE};
        if (!SetFileInformationByHandle(file, FileDispositionInfo, &disposition, sizeof(disposition)))
            status = RootedError(GetLastError());
    }
    if (!CloseHandle(file) && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    return status;
}
