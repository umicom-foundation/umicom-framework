/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_files_posix.inc
 * PURPOSE: Use descriptor-relative file operations beneath retained workspace directories on POSIX.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus RootedError(int error)
{
    if (error == ENOENT)
        return UMI_STATUS_NOT_FOUND;
    if (error == EEXIST)
        return UMI_STATUS_ALREADY_EXISTS;
    if (error == ELOOP || error == ENOTDIR || error == EACCES || error == EPERM)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
static void RootedClose(RootedParent *parent)
{
    if (parent->parent >= 0)
        (void)close(parent->parent);
    parent->parent = -1;
}
static UmiStatus RootedOpenParent(const char *root, const char *relative, RootedParent *parent)
{
    parent->parent = open(root, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (parent->parent < 0)
        return RootedError(errno);
    memcpy(parent->parts, relative, strlen(relative) + 1U);
    parent->leaf = parent->parts;
    char *separator;
    /* Each next directory is opened relative to its held parent, not through
     * a second absolute lookup which could follow a newly replaced symlink. */
    while ((separator = strchr(parent->leaf, '/')) != NULL)
    {
        *separator = '\0';
        int next = openat(parent->parent, parent->leaf, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        if (next < 0)
            return RootedError(errno);
        (void)close(parent->parent);
        parent->parent = next;
        parent->leaf = separator + 1;
    }
    return UMI_STATUS_OK;
}
static UmiStatus RootedInspect(RootedParent *parent, struct stat *info)
{
    if (fstatat(parent->parent, parent->leaf, info, AT_SYMLINK_NOFOLLOW) != 0)
        return RootedError(errno);
    return S_ISREG(info->st_mode) ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
}
static UmiStatus RootedExists(RootedParent *parent)
{
    struct stat info;
    return RootedInspect(parent, &info);
}
static int RootedSameRead(const struct stat *before, const struct stat *after)
{
    if (before->st_dev != after->st_dev || before->st_ino != after->st_ino ||
        before->st_size != after->st_size)
        return 0;
#ifdef __APPLE__
    return before->st_mtimespec.tv_sec == after->st_mtimespec.tv_sec &&
           before->st_mtimespec.tv_nsec == after->st_mtimespec.tv_nsec &&
           before->st_ctimespec.tv_sec == after->st_ctimespec.tv_sec &&
           before->st_ctimespec.tv_nsec == after->st_ctimespec.tv_nsec;
#else
    return before->st_mtim.tv_sec == after->st_mtim.tv_sec &&
           before->st_mtim.tv_nsec == after->st_mtim.tv_nsec &&
           before->st_ctim.tv_sec == after->st_ctim.tv_sec &&
           before->st_ctim.tv_nsec == after->st_ctim.tv_nsec;
#endif
}
static UmiStatus RootedRead(RootedParent *parent, size_t maximum, unsigned char **out, size_t *outSize)
{
    /* O_NONBLOCK prevents a substituted pipe/device from blocking before the
     * regular-file check. O_NOFOLLOW rejects a leaf symlink atomically. */
    int file = openat(parent->parent, parent->leaf, O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_CLOEXEC);
    if (file < 0)
        return RootedError(errno);
    struct stat before, after;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    unsigned char *bytes = NULL;
    size_t size = 0U, used = 0U;
    if (fstat(file, &before) != 0)
        goto done;
    if (!S_ISREG(before.st_mode))
    {
        status = UMI_STATUS_PERMISSION_DENIED;
        goto done;
    }
    if (before.st_size < 0 || (uintmax_t)before.st_size > maximum)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    size = (size_t)before.st_size;
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
        ssize_t count = read(file, bytes + used, remaining);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0)
            goto done;
        used += (size_t)count;
    }
    unsigned char extra;
    ssize_t count;
    do
    {
        count = read(file, &extra, 1U);
    } while (count < 0 && errno == EINTR);
    if (count < 0 || fstat(file, &after) != 0)
        goto done;
    if (count != 0 || !RootedSameRead(&before, &after))
    {
        status = UMI_STATUS_BUSY;
        goto done;
    }
    status = UMI_STATUS_OK;
done:
    if (close(file) != 0 && status == UMI_STATUS_OK)
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
    struct stat before, current;
    UmiStatus status = RootedInspect(parent, &before);
    int existed = status == UMI_STATUS_OK;
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND)
        return status;
    char stage[96];
    int file = -1;
    for (unsigned attempt = 0U; attempt < 64U; ++attempt)
    {
        unsigned long sequence = atomic_fetch_add_explicit(&rootedSequence, 1UL, memory_order_relaxed);
        int length =
            snprintf(stage, sizeof(stage), ".umicom-write-%lu-%lu", (unsigned long)getpid(), sequence);
        if (length < 0 || (size_t)length >= sizeof(stage))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        /* A user may choose a filename that resembles our staging name.
         * Never create the destination itself before the complete write. */
        if (strcmp(stage, parent->leaf) == 0)
            continue;
        file = openat(parent->parent, stage, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0666);
        if (file >= 0)
            break;
        if (errno != EEXIST)
            return RootedError(errno);
    }
    if (file < 0)
        return UMI_STATUS_ALREADY_EXISTS;
    status = UMI_STATUS_OK;
    size_t used = 0U;
    while (used < size)
    {
        size_t remaining = size - used;
        if (remaining > 1048576U)
            remaining = 1048576U;
        ssize_t count = write(file, (const unsigned char *)bytes + used, remaining);
        if (count < 0 && errno == EINTR)
            continue;
        if (count <= 0)
        {
            status = UMI_STATUS_IO_ERROR;
            break;
        }
        used += (size_t)count;
    }
    if (status == UMI_STATUS_OK && existed && fchmod(file, before.st_mode & 0777) != 0)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK && fsync(file) != 0)
        status = UMI_STATUS_IO_ERROR;
    if (close(file) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK)
    {
        status = RootedInspect(parent, &current);
        if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK && renameat(parent->parent, stage, parent->parent, parent->leaf) != 0)
        status = RootedError(errno);
    if (status != UMI_STATUS_OK)
        (void)unlinkat(parent->parent, stage, 0);
    return status;
}
static UmiStatus RootedRemove(RootedParent *parent)
{
    struct stat info;
    UmiStatus status = RootedInspect(parent, &info);
    if (status != UMI_STATUS_OK)
        return status;
    /* unlinkat never follows the leaf. A concurrently substituted symlink can
     * only be unlinked itself; its target is not opened or recursively removed. */
    return unlinkat(parent->parent, parent->leaf, 0) == 0 ? UMI_STATUS_OK : RootedError(errno);
}
