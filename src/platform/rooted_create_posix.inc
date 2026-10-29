/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_create_posix.inc
 *
 * PURPOSE:
 *   Create a new practice or export file relative to a retained directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* O_EXCL makes refusal of an existing leaf part of the native open operation.
 * A separate existence check would race other writers. Retain any partial new
 * file after failure so the caller can explain the outcome without deleting
 * another process's subsequent work. The existing replacement writer remains
 * available for callers that deliberately edit an existing document. */
static UmiStatus RootedCreateNew(RootedParent *parent, const void *bytes, size_t size,
    UmiRootedFileCreation *report)
{
    int file = openat(parent->parent, parent->leaf,
        O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    if (file < 0) return RootedError(errno);
    report->created = 1;
    UmiStatus status = UMI_STATUS_OK;
    struct stat info;
    if (fstat(file, &info) != 0 || !S_ISREG(info.st_mode))
        status = UMI_STATUS_IO_ERROR;
    while (status == UMI_STATUS_OK && report->bytes_written < size) {
        size_t remaining = size - report->bytes_written;
        if (remaining > 1048576U) remaining = 1048576U;
        ssize_t count = write(file, (const unsigned char *)bytes + report->bytes_written, remaining);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0 || (size_t)count > remaining) {
            status = UMI_STATUS_IO_ERROR;
            break;
        }
        report->bytes_written += (size_t)count;
    }
    if (status == UMI_STATUS_OK) {
        int flushed;
        do { flushed = fsync(file); } while (flushed != 0 && errno == EINTR);
        if (flushed != 0) status = UMI_STATUS_IO_ERROR;
    }
    /* Retrying close can close a descriptor reused by another thread. */
    if (close(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    report->complete = status == UMI_STATUS_OK;
    return status;
}
