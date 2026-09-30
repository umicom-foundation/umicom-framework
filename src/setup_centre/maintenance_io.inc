/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/setup_centre/maintenance_io.inc
 * PURPOSE:
 *   The maintenance primitives share the existing checked path walker and file handles in
 *   io.c. This avoids a second installer filesystem implementation. Moves are no-replace
 *   operations on one filesystem. No recursive eraser exists.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/maintenance_io.inc
 * The maintenance primitives share the existing checked path walker and file
 * handles in io.c. This avoids a second installer filesystem implementation.
 * Moves are no-replace operations on one filesystem. No recursive eraser exists.
 *---------------------------------------------------------------------------*/
#include "maintenance_internal.h"
#ifdef _WIN32
static UmiStatus SmFileInfo(ScHandle *handle, int directory, UmiSetupReport *report)
{
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(handle->file, &info)) return Error(report, "Cannot inspect maintenance input");
    if ((info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U ||
        ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0U) != !!directory ||
        (!directory && info.nNumberOfLinks != 1U)) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}
UmiStatus SmObserveRegular(const char *path, UmiSetupReport *report)
{
    ScHandle handle;
    UmiStatus status = Open(&handle, path, 0, 0, NULL, report);
    if (status == UMI_STATUS_OK) status = SmFileInfo(&handle, 0, report);
    Close(&handle);
    return status;
}
UmiStatus SmCheckRoot(const char *path, UmiSetupReport *report)
{
    ScHandle handle;
    UmiStatus status = Open(&handle, path, 0, 1, NULL, report);
    if (status == UMI_STATUS_OK) status = SmFileInfo(&handle, 1, report);
    Close(&handle);
    /* UNC/device paths are already rejected by the canonical path contract.
     * Mapped network drives are not a qualified transaction filesystem. */
    if (status == UMI_STATUS_OK) {
        wchar_t drive[4] = {(wchar_t)path[0], L':', L'\\', 0};
        if (GetDriveTypeW(drive) != DRIVE_FIXED) status = UMI_STATUS_UNAVAILABLE;
    }
    return status;
}
UmiStatus SmSyncParent(const char *path, UmiSetupReport *report)
{
    (void)path; (void)report;
    /* ScWriteNew/ScCopy flush file handles and SmMoveNew requests write-through.
     * Windows directory durability is not represented as a POSIX fsync proof. */
    return UMI_STATUS_OK;
}
UmiStatus SmLock(const char *path, void **out, UmiSetupReport *report)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    ScHandle *handle = calloc(1U, sizeof(*handle));
    if (handle == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    handle->file = INVALID_HANDLE_VALUE;
    wchar_t wide[UMI_SETUP_PATH_CAPACITY + 8U];
    UmiStatus status = Wide(path, wide);
    if (status == UMI_STATUS_OK) status = Parents(handle, wide, report);
    if (status == UMI_STATUS_OK) {
        handle->file = CreateFileW(wide, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
            FILE_FLAG_OPEN_REPARSE_POINT, NULL);
        if (handle->file == INVALID_HANDLE_VALUE) status = Error(report, "Cannot open maintenance lock");
    }
    if (status == UMI_STATUS_OK) status = SmFileInfo(handle, 0, report);
    if (status == UMI_STATUS_OK) {
        OVERLAPPED range = {0};
        if (!LockFileEx(handle->file, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY,
                0U, 1U, 0U, &range)) {
            status = GetLastError() == ERROR_LOCK_VIOLATION ? UMI_STATUS_BUSY : Error(report, "Cannot lock installation");
        }
    }
    if (status != UMI_STATUS_OK) { Close(handle); free(handle); }
    else *out = handle;
    return status;
}
void SmUnlock(void *lock)
{
    ScHandle *handle = lock;
    if (handle != NULL) {
        OVERLAPPED range = {0};
        (void)UnlockFileEx(handle->file, 0U, 1U, 0U, &range);
        Close(handle); free(handle);
    }
}
UmiStatus SmMoveNew(const char *source, const char *destination, UmiSetupReport *report)
{
    if (strcmp(source, destination) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    ScHandle from = {0}, to = {0};
    from.file = to.file = INVALID_HANDLE_VALUE;
    wchar_t oldName[UMI_SETUP_PATH_CAPACITY + 8U], newName[UMI_SETUP_PATH_CAPACITY + 8U];
    UmiStatus status = Wide(source, oldName);
    if (status == UMI_STATUS_OK) status = Wide(destination, newName);
    if (status == UMI_STATUS_OK) status = Parents(&from, oldName, report);
    if (status == UMI_STATUS_OK) status = Parents(&to, newName, report);
    if (status == UMI_STATUS_OK) {
        from.file = CreateFileW(oldName, DELETE | FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
            FILE_FLAG_OPEN_REPARSE_POINT, NULL);
        if (from.file == INVALID_HANDLE_VALUE) status = Error(report, "Cannot move an owned file");
    }
    if (status == UMI_STATUS_OK) status = SmFileInfo(&from, 0, report);
    /* Without COPY_ALLOWED the move cannot silently become copy-and-delete
     * across volumes. Without REPLACE_EXISTING it cannot erase a destination. */
    if (status == UMI_STATUS_OK && !MoveFileExW(oldName, newName, MOVEFILE_WRITE_THROUGH))
        status = Error(report, "Cannot move file without replacement");
    Close(&to); Close(&from);
    return status;
}
#else
#include <sys/file.h>
#if defined(__linux__)
#include <sys/vfs.h>
#endif
UmiStatus SmObserveRegular(const char *path, UmiSetupReport *report)
{
    ScHandle handle;
    UmiStatus status = Open(&handle, path, 0, 0, NULL, report);
    struct stat info;
    if (status == UMI_STATUS_OK && fstat(handle.file, &info) != 0) status = Error(report, "Cannot inspect maintenance input");
    if (status == UMI_STATUS_OK && (info.st_nlink != 1 || info.st_uid != geteuid())) status = UMI_STATUS_PERMISSION_DENIED;
    Close(&handle);
    return status;
}
UmiStatus SmCheckRoot(const char *path, UmiSetupReport *report)
{
    ScHandle handle;
    UmiStatus status = Open(&handle, path, 0, 1, NULL, report);
    struct stat info;
    if (status == UMI_STATUS_OK && fstat(handle.file, &info) != 0) status = Error(report, "Cannot inspect installation directory");
    if (status == UMI_STATUS_OK && (info.st_uid != geteuid() || (info.st_mode & 0022) != 0)) status = UMI_STATUS_PERMISSION_DENIED;
#if defined(__linux__)
    struct statfs filesystem;
    if (status == UMI_STATUS_OK && fstatfs(handle.file, &filesystem) != 0) status = Error(report, "Cannot inspect filesystem");
    if (status == UMI_STATUS_OK && (filesystem.f_type == 0x6969L ||
        (unsigned long)filesystem.f_type == 0xff534d42UL ||
        (unsigned long)filesystem.f_type == 0xfe534d42UL)) status = UMI_STATUS_UNAVAILABLE;
#else
    if (status == UMI_STATUS_OK) status = UMI_STATUS_UNAVAILABLE;
#endif
    Close(&handle);
    return status;
}
UmiStatus SmSyncParent(const char *path, UmiSetupReport *report)
{
    int parent = -1;
    char leaf[256];
    UmiStatus status = Parent(path, &parent, leaf, report);
    if (status == UMI_STATUS_OK && fsync(parent) != 0) status = Error(report, "Cannot flush parent directory");
    if (parent >= 0) (void)close(parent);
    return status;
}
UmiStatus SmLock(const char *path, void **out, UmiSetupReport *report)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    ScHandle *handle = malloc(sizeof(*handle));
    if (handle == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    handle->file = handle->parent = -1;
    char leaf[256];
    UmiStatus status = Parent(path, &handle->parent, leaf, report);
    if (status == UMI_STATUS_OK) {
        handle->file = openat(handle->parent, leaf, O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK, 0600);
        if (handle->file < 0) status = Error(report, "Cannot open maintenance lock");
    }
    struct stat info;
    if (status == UMI_STATUS_OK && fstat(handle->file, &info) != 0) status = Error(report, "Cannot inspect maintenance lock");
    if (status == UMI_STATUS_OK && (!S_ISREG(info.st_mode) || info.st_nlink != 1 ||
            info.st_uid != geteuid() || (info.st_mode & 0022) != 0)) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && flock(handle->file, LOCK_EX | LOCK_NB) != 0)
        status = errno == EWOULDBLOCK ? UMI_STATUS_BUSY : Error(report, "Cannot lock installation");
    if (status != UMI_STATUS_OK) { Close(handle); free(handle); }
    else *out = handle;
    return status;
}
void SmUnlock(void *lock)
{
    ScHandle *handle = lock;
    if (handle != NULL) { (void)flock(handle->file, LOCK_UN); Close(handle); free(handle); }
}
UmiStatus SmMoveNew(const char *source, const char *destination, UmiSetupReport *report)
{
#if defined(__linux__)
    if (strcmp(source, destination) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    int from = -1, to = -1;
    char oldName[256], newName[256];
    UmiStatus status = Parent(source, &from, oldName, report);
    if (status == UMI_STATUS_OK) status = Parent(destination, &to, newName, report);
    struct stat info;
    if (status == UMI_STATUS_OK && fstatat(from, oldName, &info, AT_SYMLINK_NOFOLLOW) != 0) status = Error(report, "Cannot inspect moved file");
    if (status == UMI_STATUS_OK && (!S_ISREG(info.st_mode) || info.st_nlink != 1 || info.st_uid != geteuid())) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && renameat2(from, oldName, to, newName, RENAME_NOREPLACE) != 0)
        status = (errno == ENOSYS || errno == EINVAL || errno == EOPNOTSUPP) ? UMI_STATUS_UNAVAILABLE : Error(report, "Cannot move file without replacement");
    if (status == UMI_STATUS_OK && (fsync(from) != 0 || fsync(to) != 0)) status = Error(report, "Cannot flush moved file directories");
    if (from >= 0) (void)close(from);
    if (to >= 0) (void)close(to);
    return status;
#else
    (void)source; (void)destination; (void)report;
    return UMI_STATUS_UNAVAILABLE;
#endif
}
#endif
