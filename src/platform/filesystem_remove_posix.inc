/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/filesystem_remove_posix.inc
 *
 * PURPOSE:
 *   Remove owned POSIX directory entries without following leaf links.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <fcntl.h>

/* A descriptor pins each enumerated directory. Child names are resolved
 * relative to that directory; changing an ancestor's spelling cannot redirect
 * subsequent child traversal. A symbolic link is removed as one entry.
 * Callers still own the tree: concurrent replacement, mount changes and
 * same-filesystem bind mounts are not an isolation boundary. */
static UmiStatus umi_posix_remove_at(int parent, const char *name,
    size_t depth, dev_t device, int has_device)
{
    struct stat before;
    if (fstatat(parent, name, &before, AT_SYMLINK_NOFOLLOW) != 0)
        return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    if (!S_ISDIR(before.st_mode))
        return unlinkat(parent, name, 0) == 0 || errno == ENOENT
            ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    if (depth >= 128U) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A nested mounted filesystem is a separate cleanup ownership boundary.
     * Refuse it rather than deleting another volume's ordinary directory data. */
    if (has_device && before.st_dev != device) return UMI_STATUS_INVALID_STATE;
    int descriptor = openat(parent, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (descriptor < 0) return UMI_STATUS_IO_ERROR;
    struct stat opened;
    UmiStatus status = UMI_STATUS_OK;
    if (fstat(descriptor, &opened) != 0) status = UMI_STATUS_IO_ERROR;
    else if (opened.st_dev != before.st_dev || opened.st_ino != before.st_ino)
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK) {
        (void)close(descriptor);
        return status;
    }
    DIR *directory = fdopendir(descriptor);
    if (directory == NULL) { (void)close(descriptor); return UMI_STATUS_IO_ERROR; }
    /* fdopendir owns descriptor on success; closedir releases it exactly once.
     * errno distinguishes a complete enumeration from an interrupted read. */
    for (;;) {
        errno = 0;
        struct dirent *entry = readdir(directory);
        if (entry == NULL) {
            if (errno != 0) status = UMI_STATUS_IO_ERROR;
            break;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        status = umi_posix_remove_at(dirfd(directory), entry->d_name,
            depth + 1U, opened.st_dev, 1);
        if (status != UMI_STATUS_OK) break;
    }
    if (closedir(directory) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) return status;
    struct stat current;
    if (fstatat(parent, name, &current, AT_SYMLINK_NOFOLLOW) != 0)
        return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    if (!S_ISDIR(current.st_mode) || current.st_dev != opened.st_dev ||
        current.st_ino != opened.st_ino) return UMI_STATUS_INVALID_STATE;
    return unlinkat(parent, name, AT_REMOVEDIR) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

static UmiStatus umi_posix_remove_tree(const char *path)
{
    if (path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strnlen(path, UMI_PATH_CAPACITY);
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char selected[UMI_PATH_CAPACITY];
    memcpy(selected, path, length + 1U);
    /* Trim separators without resolving the leaf link. Native POSIX bytes,
     * including a backslash in a filename, retain their original meaning. */
    while (length != 0U && selected[length - 1U] == '/') selected[--length] = '\0';
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    char *separator = strrchr(selected, '/');
    const char *name = separator != NULL ? separator + 1U : selected;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    char parent_path[UMI_PATH_CAPACITY];
    if (separator != NULL) {
        size_t parent_length = (size_t)(separator - selected) + 1U;
        memcpy(parent_path, selected, parent_length);
        parent_path[parent_length] = '\0';
    } else memcpy(parent_path, ".", 2U);
    /* Parent aliases are allowed for this legacy API. Their directory handle
     * is pinned before touching the selected leaf; callers must trust parents. */
    int parent = open(parent_path, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (parent < 0) return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    UmiStatus status = umi_posix_remove_at(parent, name, 0U, (dev_t)0, 0);
    if (close(parent) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    return status;
}
