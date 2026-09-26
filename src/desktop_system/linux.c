/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/linux.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#define _POSIX_C_SOURCE 200809L
#include "internal.h"
#include "umicom/desktop_system/linux_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

static UmiStatus FileError(void)
{
    if (errno == ENOENT || errno == ESRCH) return UMI_STATUS_NOT_FOUND;
    if (errno == EACCES || errno == EPERM) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
uint64_t UmiDesktopSystemClock(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0 || now.tv_sec < 0) return 0;
    return (uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / 1000000;
}
UmiStatus UmiDesktopSystemReadText(const char *path, char *buffer, size_t capacity, size_t *outLength)
{
    if (!path || !buffer || capacity < 2 || !outLength) return UMI_STATUS_INVALID_ARGUMENT;
    int fd = open(path, O_RDONLY | O_CLOEXEC | O_NONBLOCK | O_NOFOLLOW);
    if (fd < 0) return FileError();
    struct stat info;
    if (fstat(fd, &info) != 0 || !S_ISREG(info.st_mode)) { close(fd); return UMI_STATUS_INVALID_ARGUMENT; }
    size_t used = 0; UmiStatus result = UMI_STATUS_OK;
    while (used < capacity - 1) {
        ssize_t count = read(fd, buffer + used, capacity - 1 - used);
        if (count < 0) { if (errno == EINTR) continue; result = FileError(); break; }
        if (count == 0) break;
        used += (size_t)count;
    }
    if (result == UMI_STATUS_OK && used == capacity - 1) {
        char extra; ssize_t count;
        do { count = read(fd, &extra, 1); } while (count < 0 && errno == EINTR);
        if (count > 0) result = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (count < 0) result = FileError();
    }
    if (close(fd) != 0 && result == UMI_STATUS_OK) result = UMI_STATUS_IO_ERROR;
    if (result == UMI_STATUS_OK) { buffer[used] = 0; *outLength = used; }
    else buffer[0] = 0;
    return result;
}
static UmiStatus ReadAt(const char *root, const char *suffix, char *text, size_t capacity, size_t *length)
{
    char path[UMI_DESKTOP_SYSTEM_PATH_CAPACITY];
    int n = snprintf(path, sizeof path, "%s/%s", root, suffix);
    if (n < 0 || (size_t)n >= sizeof path) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiDesktopSystemReadText(path, text, capacity, length);
}
static int NumericPid(const char *text, uint64_t *out)
{
    if (!*text || *text == '0') return 0;
    uint64_t value = 0;
    for (; *text; ++text) {
        if (*text < '0' || *text > '9' || value > (UINT64_MAX - (unsigned)(*text - '0')) / 10) return 0;
        value = value * 10 + (unsigned)(*text - '0');
    }
    *out = value; return 1;
}
static void InsertProcess(UmiDesktopSystemSnapshot *s, const UmiDesktopSystemProcess *value)
{
    /* Keep the lowest PIDs deterministically, regardless of directory order. */
    size_t at = 0; while (at < s->processCount && s->processes[at].pid < value->pid) ++at;
    if (at == UMI_DESKTOP_SYSTEM_PROCESS_LIMIT) return;
    size_t count = s->processCount < UMI_DESKTOP_SYSTEM_PROCESS_LIMIT ? s->processCount + 1 : s->processCount;
    for (size_t i = count - 1; i > at; --i) s->processes[i] = s->processes[i - 1];
    s->processes[at] = *value; s->processCount = count;
}
static void Processes(const char *root, UmiDesktopSystemSnapshot *s)
{
    DIR *dir = opendir(root); if (!dir) { s->processStatus = FileError(); return; }
    long pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize <= 0) { closedir(dir); s->processStatus = UMI_STATUS_UNAVAILABLE; return; }
    s->processStatus = UMI_STATUS_OK;
    for (;;) {
        errno = 0; struct dirent *entry = readdir(dir);
        if (!entry) { if (errno != 0) s->processStatus = UMI_STATUS_IO_ERROR; break; }
        uint64_t pid; if (!NumericPid(entry->d_name, &pid)) continue;
        ++s->processesSeen;
        if (s->processesSeen > UMI_DESKTOP_SYSTEM_SCAN_LIMIT) { s->processStatus = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        char suffix[64], text[8192]; size_t length = 0; UmiDesktopSystemProcess process;
        int n = snprintf(suffix, sizeof suffix, "%s/stat", entry->d_name);
        if (n < 0 || (size_t)n >= sizeof suffix ||
            ReadAt(root, suffix, text, sizeof text, &length) != UMI_STATUS_OK ||
            UmiDesktopSystemParseProcess(text, length, (uint64_t)pageSize, &process) != UMI_STATUS_OK || process.pid != pid) {
            ++s->processesUnreadable; continue;
        }
        InsertProcess(s, &process);
    }
    if (closedir(dir) != 0) s->processStatus = UMI_STATUS_IO_ERROR;
    if (s->processStatus == UMI_STATUS_OK && s->processesSeen > UMI_DESKTOP_SYSTEM_PROCESS_LIMIT)
        s->processStatus = UMI_STATUS_CAPACITY_EXCEEDED;
}
static int InterfaceCompare(const void *a, const void *b)
{
    const UmiDesktopSystemInterface *left = a, *right = b;
    return strcmp(left->name, right->name);
}
static void Network(const char *root, char *text, size_t capacity, UmiDesktopSystemSnapshot *s)
{
    size_t length = 0; s->networkStatus = ReadAt(root, "net/dev", text, capacity, &length);
    if (s->networkStatus != UMI_STATUS_OK) return;
    const char *p = text, *end = text + length;
    for (unsigned header = 0; header < 2; ++header) {
        const char *lineEnd = memchr(p, '\n', (size_t)(end - p));
        if (!lineEnd || !memchr(p, '|', (size_t)(lineEnd - p))) { s->networkStatus = UMI_STATUS_PARSE_ERROR; return; }
        p = lineEnd + 1;
    }
    while (p < end) {
        const char *lineEnd = memchr(p, '\n', (size_t)(end - p)); if (!lineEnd) lineEnd = end;
        UmiDesktopSystemInterface value;
        if (UmiDesktopSystemParseInterface(p, (size_t)(lineEnd - p), &value) != UMI_STATUS_OK) {
            s->networkStatus = UMI_STATUS_PARSE_ERROR; s->interfaceCount = 0; return;
        }
        ++s->interfacesSeen;
        for (size_t i = 0; i < s->interfaceCount; ++i) if (strcmp(s->interfaces[i].name, value.name) == 0) {
            s->networkStatus = UMI_STATUS_PARSE_ERROR; s->interfaceCount = 0; return;
        }
        if (s->interfaceCount < UMI_DESKTOP_SYSTEM_INTERFACE_LIMIT) s->interfaces[s->interfaceCount++] = value;
        else s->networkStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        p = lineEnd < end ? lineEnd + 1 : end;
    }
    qsort(s->interfaces, s->interfaceCount, sizeof s->interfaces[0], InterfaceCompare);
}
void UmiDesktopSystemCaptureNative(const UmiDesktopSystemOptions *options, UmiDesktopSystemSnapshot *s)
{
    const char *root = options->linuxProcRoot ? options->linuxProcRoot : "/proc";
    s->fixture = strcmp(root, "/proc") != 0;
    (void)snprintf(s->source, sizeof s->source, "%s", root);
    char *text = malloc(65537); if (!text) {
        s->memoryStatus = s->cpuStatus = s->networkStatus = UMI_STATUS_OUT_OF_MEMORY; return;
    }
    size_t length = 0;
    s->memoryStatus = ReadAt(root, "meminfo", text, 65537, &length);
    if (s->memoryStatus == UMI_STATUS_OK) s->memoryStatus = UmiDesktopSystemParseMemory(text, length, &s->memory);
    s->cpuStatus = ReadAt(root, "stat", text, 65537, &length);
    if (s->cpuStatus == UMI_STATUS_OK) s->cpuStatus = UmiDesktopSystemParseCpu(text, length, &s->cpu);
    Processes(root, s); Network(root, text, 65537, s); free(text);
    const char *storage = options->storagePath ? options->storagePath : "/";
    (void)snprintf(s->storage.path, sizeof s->storage.path, "%s", storage);
    struct statvfs volume;
    if (statvfs(storage, &volume) != 0) s->storageStatus = FileError();
    else if (!volume.f_frsize || !UmiDesktopSystemMultiply((uint64_t)volume.f_blocks, (uint64_t)volume.f_frsize, &s->storage.totalBytes) ||
        !UmiDesktopSystemMultiply((uint64_t)volume.f_bavail, (uint64_t)volume.f_frsize, &s->storage.availableBytes) ||
        s->storage.availableBytes > s->storage.totalBytes) {
        s->storage.totalBytes = s->storage.availableBytes = 0; s->storageStatus = UMI_STATUS_PARSE_ERROR;
    } else s->storageStatus = UMI_STATUS_OK;
    UmiDesktopSystemReadBoot(options->bootReportPath ? options->bootReportPath : "/run/umicom/boot.report", s);
}
