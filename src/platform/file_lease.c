/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/file_lease.c
 * Purpose: Use native handle lifetime instead of removable marker files for cooperative ownership.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/file_lease.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

struct UmiFileLease {
#ifdef _WIN32
    HANDLE handle;
#elif defined(__linux__) || defined(__APPLE__)
    int descriptor;
#else
    unsigned unavailable;
#endif
};

UmiStatus UmiFileLeaseTryAcquire(const char *path, UmiFileLease **outLease)
{
    if (outLease == NULL || *outLease != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
    return UMI_STATUS_UNAVAILABLE;
#else
    UmiFileLease *lease = malloc(sizeof(*lease));
    if (lease == NULL) return UMI_STATUS_OUT_OF_MEMORY;
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide,
                           (int)UMI_PATH_CAPACITY) == 0) {
        free(lease);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* OPEN_ALWAYS preserves existing contents. Share mode zero and a
     * non-inheritable handle keep ownership tied to this process lifetime. */
    lease->handle = CreateFileW(wide, GENERIC_READ | GENERIC_WRITE, 0U, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (lease->handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        free(lease);
        if (error == ERROR_SHARING_VIOLATION || error == ERROR_LOCK_VIOLATION)
            return UMI_STATUS_BUSY;
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND)
            return UMI_STATUS_NOT_FOUND;
        if (error == ERROR_ACCESS_DENIED) return UMI_STATUS_PERMISSION_DENIED;
        return UMI_STATUS_IO_ERROR;
    }
    BY_HANDLE_FILE_INFORMATION information;
    status = GetFileType(lease->handle) == FILE_TYPE_DISK &&
        GetFileInformationByHandle(lease->handle, &information) &&
        (information.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) == 0U
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
#else
    /* Separate opens contend even within one process. CLOEXEC avoids extending
     * the lease through compiler children. NONBLOCK prevents FIFO open waits;
     * inspect the same descriptor before taking an advisory regular-file lock. */
    lease->descriptor = open(path, O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK, 0600);
    if (lease->descriptor < 0) {
        int error = errno;
        free(lease);
        if (error == ENOENT) return UMI_STATUS_NOT_FOUND;
        if (error == EACCES || error == EPERM) return UMI_STATUS_PERMISSION_DENIED;
        if (error == ELOOP || error == EISDIR || error == ENOTDIR)
            return UMI_STATUS_INVALID_ARGUMENT;
        return UMI_STATUS_IO_ERROR;
    }
    struct stat information;
    status = fstat(lease->descriptor, &information) == 0 && S_ISREG(information.st_mode)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) {
        int locked;
        do { locked = flock(lease->descriptor, LOCK_EX | LOCK_NB); }
        while (locked != 0 && errno == EINTR);
        if (locked != 0) status = errno == EWOULDBLOCK || errno == EAGAIN
            ? UMI_STATUS_BUSY : UMI_STATUS_IO_ERROR;
    }
#endif
    if (status != UMI_STATUS_OK) {
        (void)UmiFileLeaseRelease(&lease);
        return status;
    }
    *outLease = lease;
    return UMI_STATUS_OK;
#endif
}

UmiStatus UmiFileLeaseRelease(UmiFileLease **lease)
{
    if (lease == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*lease == NULL) return UMI_STATUS_OK;
    UmiStatus status = UMI_STATUS_OK;
#ifdef _WIN32
    if (!CloseHandle((*lease)->handle)) status = UMI_STATUS_IO_ERROR;
#elif defined(__linux__) || defined(__APPLE__)
    /* Closing the owning description releases its flock. Never unlink the
     * pathname: another process may already be opening that same inode. */
    if (close((*lease)->descriptor) != 0) status = UMI_STATUS_IO_ERROR;
#else
    status = UMI_STATUS_UNAVAILABLE;
#endif
    free(*lease);
    *lease = NULL;
    return status;
}
