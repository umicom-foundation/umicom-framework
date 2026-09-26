/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/session_linux.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#define _GNU_SOURCE
#include "session_internal.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Opened { int executable, home, runtime; } Opened;
static void Close(Opened *opened)
{
    if (opened->executable >= 0) close(opened->executable);
    if (opened->home >= 0) close(opened->home);
    if (opened->runtime >= 0) close(opened->runtime);
    *opened = (Opened){-1, -1, -1};
}
static int NativeElf(int fd)
{
    unsigned char bytes[64]; ssize_t count;
    do { count = pread(fd, bytes, sizeof bytes, 0); } while (count < 0 && errno == EINTR);
    if (count != (ssize_t)sizeof bytes || memcmp(bytes, "\177ELF", 4) || bytes[4] != 2 || bytes[5] != 1 || bytes[6] != 1)
        return 0;
    unsigned type = bytes[16] | (unsigned)bytes[17] << 8;
    unsigned machine = bytes[18] | (unsigned)bytes[19] << 8;
    if (type != 2 && type != 3) return 0;
#if defined(__x86_64__)
    return machine == 62;
#elif defined(__aarch64__)
    return machine == 183;
#elif defined(__riscv) && __riscv_xlen == 64
    return machine == 243;
#else
    (void)machine; return 0;
#endif
}
static UmiStatus Prepare(const UmiDesktopSessionRequest *r, UmiDesktopSessionCheck *check, Opened *opened)
{
    *opened = (Opened){-1, -1, -1};
    UmiStatus status = UmiDesktopSessionValidate(r, check); if (status != UMI_STATUS_OK) return status;
    uid_t uid = getuid();
    if (uid == 0 || geteuid() != uid || getegid() != getgid())
        return UmiDesktopSessionExplain(check, UMI_STATUS_PERMISSION_DENIED, "Run Desk as your normal user, not root or a set-id launcher.");
    opened->runtime = open(r->runtimeDirectory, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    struct stat info;
    if (opened->runtime < 0 || fstat(opened->runtime, &info) != 0 || info.st_uid != uid || (info.st_mode & 0777) != 0700) {
        Close(opened); return UmiDesktopSessionExplain(check, UMI_STATUS_PERMISSION_DENIED, "XDG_RUNTIME_DIR must be a real directory owned by you with mode 0700.");
    }
    if (check->backend == UMI_DESKTOP_SESSION_WAYLAND &&
        (fstatat(opened->runtime, r->waylandDisplay, &info, AT_SYMLINK_NOFOLLOW) != 0 || !S_ISSOCK(info.st_mode) || info.st_uid != uid)) {
        Close(opened); return UmiDesktopSessionExplain(check, UMI_STATUS_UNAVAILABLE, "The selected Wayland socket is missing or not owned by this user.");
    }
    opened->home = open(r->home, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (opened->home < 0 || fstat(opened->home, &info) != 0 || info.st_uid != uid) {
        Close(opened); return UmiDesktopSessionExplain(check, UMI_STATUS_PERMISSION_DENIED, "The home directory must exist and belong to this user.");
    }
    if (check->backend == UMI_DESKTOP_SESSION_X11 && r->xauthority && *r->xauthority) {
        int file = open(r->xauthority, O_RDONLY | O_NOFOLLOW | O_NONBLOCK | O_CLOEXEC);
        int okay = file >= 0 && fstat(file, &info) == 0 && S_ISREG(info.st_mode) && info.st_uid == uid && !(info.st_mode & 0077);
        if (file >= 0) close(file);
        if (!okay) { Close(opened); return UmiDesktopSessionExplain(check, UMI_STATUS_PERMISSION_DENIED, "XAUTHORITY must be a private regular file owned by you."); }
    }
    opened->executable = open(r->deskPath, O_RDONLY | O_NOFOLLOW | O_NONBLOCK | O_CLOEXEC);
    if (opened->executable < 0 || fstat(opened->executable, &info) != 0 || !S_ISREG(info.st_mode) ||
        (info.st_uid != 0 && info.st_uid != uid) || (info.st_mode & (S_IWGRP | S_IWOTH | S_ISUID | S_ISGID)) ||
        !(info.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) || !NativeElf(opened->executable)) {
        Close(opened); return UmiDesktopSessionExplain(check, UMI_STATUS_PERMISSION_DENIED,
            "Select a non-set-id native ELF executable owned by you or root, without group/other writes or a symlink.");
    }
    return UmiDesktopSessionExplain(check, UMI_STATUS_OK,
        "Native preflight passed. No program or compositor connection has been started.");
}
UmiStatus UmiDesktopSessionInspect(const UmiDesktopSessionRequest *request, UmiDesktopSessionCheck *outCheck)
{
    if (!outCheck) return UMI_STATUS_INVALID_ARGUMENT;
    Opened opened; UmiStatus status = Prepare(request, outCheck, &opened); Close(&opened); return status;
}
static int ProtectDescriptors(void)
{
    /* A session is a process replacement, not a shell command. Do not leak a
     * terminal launcher's inherited files into Desk or its later applications.
     * The selected ELF fd may also be CLOEXEC: Linux consumes it before close. */
    DIR *dir = opendir("/proc/self/fd"); if (!dir) return 0;
    int okay = 1;
    for (;;) {
        errno = 0; struct dirent *entry = readdir(dir);
        if (!entry) { if (errno) okay = 0; break; }
        char *end = NULL; errno = 0; long fd = strtol(entry->d_name, &end, 10);
        if (errno || end == entry->d_name || *end || fd < 3 || fd > INT_MAX) continue;
        int flags = fcntl((int)fd, F_GETFD);
        if (flags < 0 || fcntl((int)fd, F_SETFD, flags | FD_CLOEXEC) < 0) { okay = 0; break; }
    }
    if (closedir(dir) != 0) okay = 0;
    return okay;
}
UmiStatus UmiDesktopSessionEnter(const UmiDesktopSessionRequest *r, UmiDesktopSessionCheck *check)
{
    if (!check) return UMI_STATUS_INVALID_ARGUMENT;
    Opened opened; UmiStatus status = Prepare(r, check, &opened); if (status != UMI_STATUS_OK) return status;
    char values[14][2112]; char *environment[15]; size_t count = 0;
#define ADD(name, value) do { \
    int written = snprintf(values[count], sizeof values[count], "%s=%s", (name), (value)); \
    if (written < 0 || (size_t)written >= sizeof values[count]) { Close(&opened); return UmiDesktopSessionExplain(check, UMI_STATUS_CAPACITY_EXCEEDED, "Environment value is too long."); } \
    environment[count] = values[count]; ++count; \
} while (0)
    ADD("HOME", r->home); ADD("XDG_RUNTIME_DIR", r->runtimeDirectory);
    ADD("XDG_CURRENT_DESKTOP", "Umicom"); ADD("UMICOM_DESKTOP_SESSION", "1");
    ADD("PATH", "/usr/local/bin:/usr/bin:/bin"); ADD("LANG", r->language && *r->language ? r->language : "C");
    if (check->backend == UMI_DESKTOP_SESSION_WAYLAND) {
        ADD("WAYLAND_DISPLAY", r->waylandDisplay); ADD("XDG_SESSION_TYPE", "wayland"); ADD("GDK_BACKEND", "wayland");
    } else {
        ADD("DISPLAY", r->display); ADD("XDG_SESSION_TYPE", "x11"); ADD("GDK_BACKEND", "x11");
        if (r->xauthority && *r->xauthority) ADD("XAUTHORITY", r->xauthority);
    }
    if (r->sessionBus && *r->sessionBus) ADD("DBUS_SESSION_BUS_ADDRESS", r->sessionBus);
#undef ADD
    environment[count] = NULL;
    if (!ProtectDescriptors() || prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0 || fchdir(opened.home) != 0) {
        Close(&opened); return UmiDesktopSessionExplain(check, UMI_STATUS_IO_ERROR, "Could not prepare the session's descriptors, process policy or home directory.");
    }
    char *arguments[] = {(char *)r->deskPath, NULL};
    fexecve(opened.executable, arguments, environment);
    Close(&opened);
    return UmiDesktopSessionExplain(check, UMI_STATUS_IO_ERROR,
        "Desk did not execute. Check architecture, execute permissions and installed runtime dependencies.");
}
