/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_workspace/local.c
 * PURPOSE:
 *   Lifetime guard for one desktop workspace. The lock file is never unlinked: removing a
 *   locked filename would let a second process lock a different inode. SQLite access remains
 *   exclusively inside the canonical Data Server.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Lifetime guard for one desktop workspace. The lock file is never unlinked:
 * removing a locked filename would let a second process lock a different inode.
 * SQLite access remains exclusively inside the canonical Data Server.
 *---------------------------------------------------------------------------*/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include "internal.h"
#include <errno.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
struct DwGuard { HANDLE lock, directory; };
static UmiStatus Error(void)
{
    DWORD e = GetLastError();
    if (e == ERROR_ACCESS_DENIED) return UMI_STATUS_PERMISSION_DENIED;
    if (e == ERROR_SHARING_VIOLATION || e == ERROR_LOCK_VIOLATION) return UMI_STATUS_BUSY;
    if (e == ERROR_FILE_NOT_FOUND || e == ERROR_PATH_NOT_FOUND) return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_IO_ERROR;
}
static int Ordinary(HANDLE file, int directory)
{
    BY_HANDLE_FILE_INFORMATION info;
    return GetFileInformationByHandle(file, &info) && GetFileType(file) == FILE_TYPE_DISK &&
        !(info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
        ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) == directory &&
        (directory || info.nNumberOfLinks == 1);
}
UmiStatus DwGuardAcquire(const char *directory, DwGuard **out, char path[1024])
{
    if (!out || !directory || !path) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL; size_t length = strlen(directory);
    if (length < 3U || length > 850U || directory[1] != ':' ||
        (directory[2] != '/' && directory[2] != '\\') ||
        !((directory[0] >= 'A' && directory[0] <= 'Z') || (directory[0] >= 'a' && directory[0] <= 'z')))
        return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t wide[1024];
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, directory, -1, wide, 1024)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = wcslen(wide);
    for (size_t i = 3; i < n; ++i) {
        if (wide[i] < 32 || wcschr(L"<>|?*:\"", wide[i])) return UMI_STATUS_INVALID_ARGUMENT;
        if (wide[i] == L'/') wide[i] = L'\\';
    }
    if (n <= 3 || wide[n-1U] == L'\\') return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate each component, and inspect every ancestor without resolving a
     * junction. Only the final dedicated directory may be created. */
    size_t start = 3;
    for (size_t i = 3; i <= n; ++i) {
        if (i != n && wide[i] != L'\\') continue;
        if (i == start || wide[i-1U] == L'.' || wide[i-1U] == L' ') return UMI_STATUS_INVALID_ARGUMENT;
        wchar_t save = wide[i]; wide[i] = 0;
        HANDLE dir = CreateFileW(wide, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
        if (dir == INVALID_HANDLE_VALUE && i == n && (GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)) {
            if (!CreateDirectoryW(wide, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) { wide[i] = save; return Error(); }
            dir = CreateFileW(wide, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
        }
        if (dir == INVALID_HANDLE_VALUE) { wide[i] = save; return Error(); }
        int valid = Ordinary(dir, 1); CloseHandle(dir); wide[i] = save;
        if (!valid) return UMI_STATUS_INVALID_ARGUMENT;
        start = i + 1U;
    }
    DwGuard *g = calloc(1, sizeof *g);
    if (!g) return UMI_STATUS_OUT_OF_MEMORY;
    g->lock = g->directory = INVALID_HANDLE_VALUE;
    g->directory = CreateFileW(wide, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (g->directory == INVALID_HANDLE_VALUE || !Ordinary(g->directory, 1)) { DwGuardRelease(g); return UMI_STATUS_IO_ERROR; }
    wcscat(wide, L"\\workspace.lock");
    g->lock = CreateFileW(wide, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (g->lock == INVALID_HANDLE_VALUE) { UmiStatus s = Error(); DwGuardRelease(g); return s; }
    if (!Ordinary(g->lock, 0)) { DwGuardRelease(g); return UMI_STATUS_INVALID_ARGUMENT; }
    wide[n] = 0; wcscat(wide, L"\\workspace.sqlite");
    HANDLE db = CreateFileW(wide, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (db == INVALID_HANDLE_VALUE) { UmiStatus s = Error(); DwGuardRelease(g); return s; }
    int valid = Ordinary(db, 0); CloseHandle(db);
    if (!valid) { DwGuardRelease(g); return UMI_STATUS_INVALID_ARGUMENT; }
    if (!WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, path, 1024, NULL, NULL)) {
        DwGuardRelease(g); return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out = g; return UMI_STATUS_OK;
}
void DwGuardRelease(DwGuard *g)
{
    if (!g) return;
    if (g->lock != INVALID_HANDLE_VALUE) CloseHandle(g->lock);
    if (g->directory != INVALID_HANDLE_VALUE) CloseHandle(g->directory);
    free(g);
}
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
struct DwGuard { int lock, directory; };
static UmiStatus Error(void)
{
    if (errno == EACCES || errno == EPERM) return UMI_STATUS_PERMISSION_DENIED;
    if (errno == EWOULDBLOCK || errno == EAGAIN) return UMI_STATUS_BUSY;
    if (errno == ENOENT) return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_IO_ERROR;
}
static int Ordinary(int fd, int directory)
{
    struct stat st;
    return fstat(fd, &st) == 0 && (directory ? S_ISDIR(st.st_mode) : S_ISREG(st.st_mode)) &&
        st.st_uid == getuid() && !(st.st_mode & 0077) && (directory || st.st_nlink == 1);
}
static int Directory(const char *path)
{
    char copy[1024]; size_t length = strlen(path);
    if (!length || length > 850U || path[0] != '/' || length == 1 || path[length-1U] == '/') { errno = EINVAL; return -1; }
    memcpy(copy, path, length + 1U);
    int fd = open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd < 0) return -1;
    char *part = copy + 1;
    for (;;) {
        char *slash = strchr(part, '/');
        if (slash) *slash = 0;
        if (!*part || !strcmp(part, ".") || !strcmp(part, "..")) { close(fd); errno = EINVAL; return -1; }
        for (const char *p = part; *p; ++p) if ((unsigned char)*p < 32U) { close(fd); errno = EINVAL; return -1; }
        int next = openat(fd, part, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        if (next < 0 && !slash && errno == ENOENT) {
            if (mkdirat(fd, part, 0700) != 0 && errno != EEXIST) { int e = errno; close(fd); errno = e; return -1; }
            next = openat(fd, part, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        }
        int saved = errno; close(fd); errno = saved;
        if (next < 0) return -1;
        fd = next;
        if (!slash) break;
        part = slash + 1;
    }
    return fd;
}
UmiStatus DwGuardAcquire(const char *directory, DwGuard **out, char path[1024])
{
    if (!out || !directory || !path) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    DwGuard *g = malloc(sizeof *g);
    if (!g) return UMI_STATUS_OUT_OF_MEMORY;
    g->lock = -1; g->directory = Directory(directory);
    if (g->directory < 0) { UmiStatus s = errno == EINVAL ? UMI_STATUS_INVALID_ARGUMENT : Error(); free(g); return s; }
    if (!Ordinary(g->directory, 1)) { DwGuardRelease(g); return UMI_STATUS_PERMISSION_DENIED; }
    g->lock = openat(g->directory, "workspace.lock", O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, 0600);
    if (g->lock < 0) { UmiStatus s = Error(); DwGuardRelease(g); return s; }
    if (!Ordinary(g->lock, 0)) { DwGuardRelease(g); return UMI_STATUS_INVALID_ARGUMENT; }
    if (flock(g->lock, LOCK_EX | LOCK_NB) != 0) { UmiStatus s = Error(); DwGuardRelease(g); return s; }
    int db = openat(g->directory, "workspace.sqlite", O_RDWR | O_CREAT | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, 0600);
    if (db < 0) { UmiStatus s = Error(); DwGuardRelease(g); return s; }
    int valid = Ordinary(db, 0); close(db);
    if (!valid) { DwGuardRelease(g); return UMI_STATUS_INVALID_ARGUMENT; }
    /* Refuse unsafe SQLite sidecars too. A trusted private directory is still
     * required: this guard is not a defence against malicious code at this uid. */
    const char *sidecars[] = {"workspace.sqlite-journal", "workspace.sqlite-wal", "workspace.sqlite-shm"};
    for (size_t i = 0; i < sizeof sidecars/sizeof sidecars[0]; ++i) {
        int side = openat(g->directory, sidecars[i], O_RDONLY | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK);
        if (side < 0 && errno == ENOENT) continue;
        if (side < 0 || !Ordinary(side, 0)) { if (side >= 0) close(side); DwGuardRelease(g); return UMI_STATUS_INVALID_ARGUMENT; }
        close(side);
    }
    int count = snprintf(path, 1024, "%s/workspace.sqlite", directory);
    if (count < 0 || count >= 1024) { DwGuardRelease(g); return UMI_STATUS_CAPACITY_EXCEEDED; }
    *out = g; return UMI_STATUS_OK;
}
void DwGuardRelease(DwGuard *g)
{
    if (!g) return;
    if (g->lock >= 0) { (void)flock(g->lock, LOCK_UN); close(g->lock); }
    if (g->directory >= 0) close(g->directory);
    free(g);
}
#endif
