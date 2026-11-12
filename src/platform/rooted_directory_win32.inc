/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_directory_win32.inc
 *
 * PURPOSE:
 *   Create Unicode directory descendants while retaining checked Windows parents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* Preserve the ensure-only traversal for review. One active traversal now
 * handles both ordinary ensure and exclusive final-directory creation. */
#if 0
static UmiStatus RootedEnsureDirectories(const char *root, const char *relative,
    RootedParent *parent, UmiRootedDirectoryCreation *report)
{
    char full[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(root, relative, full, sizeof full);
    wchar_t *nativeRoot = NULL, *nativeFull = NULL;
    if (status == UMI_STATUS_OK) status = UmiNativePathWide(root, &nativeRoot);
    if (status == UMI_STATUS_OK) status = UmiNativePathWide(full, &nativeFull);
    if (status != UMI_STATUS_OK) { free(nativeRoot); free(nativeFull); return status; }
    size_t rootLength = wcslen(nativeRoot), length = wcslen(nativeFull);
    if (length >= UMI_PATH_CAPACITY) {
        free(nativeRoot); free(nativeFull); return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(parent->path, nativeFull, (length + 1U) * sizeof *nativeFull);
    free(nativeFull);
    status = RootedHoldDirectory(parent, nativeRoot);
    free(nativeRoot);
    /* The existing root is held first. Each component is then created, if
     * missing, and opened without following reparse points before descending.
     * An existing object is never accepted solely from CreateDirectory's error. */
    for (size_t index = rootLength; status == UMI_STATUS_OK && index <= length; ++index) {
        if (index == rootLength) continue;
        if (index != length && parent->path[index] != L'\\' && parent->path[index] != L'/')
            continue;
        wchar_t saved = parent->path[index];
        parent->path[index] = L'\0';
        status = RootedHoldDirectory(parent, parent->path);
        if (status == UMI_STATUS_NOT_FOUND) {
            if (CreateDirectoryW(parent->path, NULL))
                ++report->directories_created;
            else {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS && error != ERROR_FILE_EXISTS) {
                    status = RootedError(error);
                    parent->path[index] = saved;
                    break;
                }
            }
            status = RootedHoldDirectory(parent, parent->path);
        }
        parent->path[index] = saved;
    }
    return status;
}
#endif

static UmiStatus RootedPrepareDirectories(const char *root, const char *relative,
    RootedParent *parent, UmiRootedDirectoryCreation *report, int exclusiveLeaf)
{
    char full[UMI_PATH_CAPACITY];
    UmiStatus status = umi_path_join(root, relative, full, sizeof full);
    wchar_t *nativeRoot = NULL, *nativeFull = NULL;
    if (status == UMI_STATUS_OK) status = UmiNativePathWide(root, &nativeRoot);
    if (status == UMI_STATUS_OK) status = UmiNativePathWide(full, &nativeFull);
    if (status != UMI_STATUS_OK) { free(nativeRoot); free(nativeFull); return status; }
    size_t rootLength = wcslen(nativeRoot), length = wcslen(nativeFull);
    if (length >= UMI_PATH_CAPACITY) {
        free(nativeRoot); free(nativeFull); return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(parent->path, nativeFull, (length + 1U) * sizeof *nativeFull);
    free(nativeFull);
    status = RootedHoldDirectory(parent, nativeRoot);
    free(nativeRoot);
    /* The existing root is held first. Each component is then created, if
     * missing, and opened without following reparse points before descending.
     * An existing object is never accepted solely from CreateDirectory's error. */
    for (size_t index = rootLength; status == UMI_STATUS_OK && index <= length; ++index) {
        if (index == rootLength) continue;
        if (index != length && parent->path[index] != L'\\' && parent->path[index] != L'/')
            continue;
        wchar_t saved = parent->path[index];
        parent->path[index] = L'\0';
        if (exclusiveLeaf && index == length) {
            /* CreateDirectory owns the exclusive decision for this final name. */
            if (!CreateDirectoryW(parent->path, NULL))
                status = RootedError(GetLastError());
            else {
                ++report->directories_created;
                status = RootedHoldDirectory(parent, parent->path);
            }
            parent->path[index] = saved;
            continue;
        }
        status = RootedHoldDirectory(parent, parent->path);
        if (status == UMI_STATUS_NOT_FOUND) {
            if (CreateDirectoryW(parent->path, NULL))
                ++report->directories_created;
            else {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS && error != ERROR_FILE_EXISTS) {
                    status = RootedError(error);
                    parent->path[index] = saved;
                    break;
                }
            }
            status = RootedHoldDirectory(parent, parent->path);
        }
        parent->path[index] = saved;
    }
    return status;
}
static UmiStatus RootedDirectoryClose(RootedParent *parent, UmiStatus status)
{
    while (parent->held != 0U)
        if (!CloseHandle(parent->parents[--parent->held]) && status == UMI_STATUS_OK)
            status = UMI_STATUS_IO_ERROR;
    return status;
}
