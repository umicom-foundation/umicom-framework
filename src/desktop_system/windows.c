/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/windows.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"

static uint64_t TimeValue(FILETIME time)
{ return ((uint64_t)time.dwHighDateTime << 32) | (uint64_t)time.dwLowDateTime; }
uint64_t UmiDesktopSystemClock(void) { return (uint64_t)GetTickCount64(); }
static UmiStatus WinError(void)
{
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return UMI_STATUS_NOT_FOUND;
    if (error == ERROR_ACCESS_DENIED) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
static int WidePath(const char *text, wchar_t *out, int capacity)
{ return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, out, capacity) > 0; }
static void Name(const wchar_t *text, char *out, size_t capacity)
{
    /* Names are labels, not execution paths. Replace unrepresentable/oversize
     * labels honestly; process identity remains the numeric PID/start pair. */
    char converted[1024];
    int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text, -1,
        converted, (int)sizeof converted, NULL, NULL);
    if (n > 0) UmiDesktopSystemDisplayName(out, capacity, converted, (size_t)n - 1);
    else UmiDesktopSystemDisplayName(out, capacity, "Name unavailable", 16);
}
UmiStatus UmiDesktopSystemReadText(const char *path, char *buffer, size_t capacity, size_t *outLength)
{
    wchar_t wide[UMI_DESKTOP_SYSTEM_PATH_CAPACITY];
    if (!path || !buffer || !outLength || capacity < 2 || capacity > UINT32_MAX ||
        !WidePath(path, wide, (int)(sizeof wide / sizeof wide[0]))) return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE file = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE) return WinError();
    BY_HANDLE_FILE_INFORMATION info;
    if (!GetFileInformationByHandle(file, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) || GetFileType(file) != FILE_TYPE_DISK) {
        CloseHandle(file); return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t used = 0; UmiStatus result = UMI_STATUS_OK;
    while (used < capacity - 1) {
        DWORD count = 0;
        if (!ReadFile(file, buffer + used, (DWORD)(capacity - 1 - used), &count, NULL)) { result = WinError(); break; }
        if (!count) break;
        used += count;
    }
    if (result == UMI_STATUS_OK && used == capacity - 1) {
        char extra; DWORD count = 0;
        if (!ReadFile(file, &extra, 1, &count, NULL)) result = WinError();
        else if (count) result = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!CloseHandle(file) && result == UMI_STATUS_OK) result = UMI_STATUS_IO_ERROR;
    if (result == UMI_STATUS_OK) { buffer[used] = 0; *outLength = used; } else buffer[0] = 0;
    return result;
}
static void Insert(UmiDesktopSystemSnapshot *s, UmiDesktopSystemProcess value)
{
    size_t at = 0; while (at < s->processCount && s->processes[at].pid < value.pid) ++at;
    if (at == UMI_DESKTOP_SYSTEM_PROCESS_LIMIT) return;
    size_t count = s->processCount < UMI_DESKTOP_SYSTEM_PROCESS_LIMIT ? s->processCount + 1 : s->processCount;
    for (size_t i = count - 1; i > at; --i) s->processes[i] = s->processes[i - 1];
    s->processes[at] = value; s->processCount = count;
}
static int CompareInterface(const void *a, const void *b)
{ return strcmp(((const UmiDesktopSystemInterface *)a)->name, ((const UmiDesktopSystemInterface *)b)->name); }
void UmiDesktopSystemCaptureNative(const UmiDesktopSystemOptions *options, UmiDesktopSystemSnapshot *s)
{
    strcpy(s->source, "Windows native APIs");
    MEMORYSTATUSEX memory; memset(&memory, 0, sizeof memory); memory.dwLength = sizeof memory;
    if (GlobalMemoryStatusEx(&memory)) {
        s->memory.totalBytes = memory.ullTotalPhys; s->memory.availableBytes = memory.ullAvailPhys;
        s->memoryStatus = UMI_STATUS_OK;
    } else s->memoryStatus = WinError();
    FILETIME idle, kernel, user;
    /* GetSystemTimes covers only the calling thread's primary processor group
     * on very large hosts. Different task workers could sample different groups;
     * reject that case instead of presenting a false machine-wide percentage. */
    if (GetActiveProcessorGroupCount() > 1) s->cpuStatus = UMI_STATUS_NOT_IMPLEMENTED;
    else if (GetSystemTimes(&idle, &kernel, &user) && TimeValue(kernel) >= TimeValue(idle)) {
        s->cpu.ticks[0] = TimeValue(user); s->cpu.ticks[2] = TimeValue(kernel) - TimeValue(idle);
        s->cpu.ticks[3] = TimeValue(idle); s->cpuStatus = UMI_STATUS_OK;
    } else s->cpuStatus = UMI_STATUS_UNAVAILABLE;
    HANDLE list = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (list == INVALID_HANDLE_VALUE) s->processStatus = WinError();
    else {
        PROCESSENTRY32W entry; memset(&entry, 0, sizeof entry); entry.dwSize = sizeof entry;
        BOOL more = Process32FirstW(list, &entry); s->processStatus = UMI_STATUS_OK;
        while (more) {
            ++s->processesSeen;
            if (s->processesSeen > UMI_DESKTOP_SYSTEM_SCAN_LIMIT) { s->processStatus = UMI_STATUS_CAPACITY_EXCEEDED; break; }
            UmiDesktopSystemProcess process = {0}; process.pid = entry.th32ProcessID; process.parentPid = entry.th32ParentProcessID;
            process.state = '?'; Name(entry.szExeFile, process.name, sizeof process.name);
            HANDLE child = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
            if (child) {
                FILETIME created, exited, k, u;
                if (GetProcessTimes(child, &created, &exited, &k, &u)) { process.startKnown = 1; process.startTicks = TimeValue(created); }
                CloseHandle(child);
            }
            /* Working-set access is intentionally absent: no VM_READ right is
             * requested merely to list an application. Unknown is not zero RSS. */
            Insert(s, process); more = Process32NextW(list, &entry);
        }
        if (!more && GetLastError() != ERROR_NO_MORE_FILES) s->processStatus = WinError();
        CloseHandle(list);
        if (s->processStatus == UMI_STATUS_OK && s->processesSeen > UMI_DESKTOP_SYSTEM_PROCESS_LIMIT) s->processStatus = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    PMIB_IF_TABLE2 table = NULL;
    NETIO_STATUS network = GetIfTable2(&table);
    if (network != NO_ERROR || !table) s->networkStatus = UMI_STATUS_UNAVAILABLE;
    else {
        s->networkStatus = UMI_STATUS_OK; s->interfacesSeen = table->NumEntries;
        for (ULONG i = 0; i < table->NumEntries && s->interfaceCount < UMI_DESKTOP_SYSTEM_INTERFACE_LIMIT; ++i) {
            UmiDesktopSystemInterface *item = &s->interfaces[s->interfaceCount++];
            /* The interface index is retained in the label to distinguish aliases. */
            char alias[96]; Name(table->Table[i].Alias, alias, sizeof alias);
            (void)snprintf(item->name, sizeof item->name, "%lu %s", (unsigned long)table->Table[i].InterfaceIndex, alias);
            item->receivedBytes = table->Table[i].InOctets; item->transmittedBytes = table->Table[i].OutOctets;
        }
        if (s->interfacesSeen > s->interfaceCount) s->networkStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        qsort(s->interfaces, s->interfaceCount, sizeof s->interfaces[0], CompareInterface); FreeMibTable(table);
    }
    const char *storage = options->storagePath ? options->storagePath : "C:\\";
    wchar_t wide[UMI_DESKTOP_SYSTEM_PATH_CAPACITY];
    (void)snprintf(s->storage.path, sizeof s->storage.path, "%s", storage);
    ULARGE_INTEGER available, total, freeBytes;
    if (!WidePath(storage, wide, (int)(sizeof wide / sizeof wide[0]))) s->storageStatus = UMI_STATUS_INVALID_ARGUMENT;
    else if (!GetDiskFreeSpaceExW(wide, &available, &total, &freeBytes)) s->storageStatus = WinError();
    else if (available.QuadPart > total.QuadPart) s->storageStatus = UMI_STATUS_INVALID_STATE;
    else { s->storage.totalBytes = total.QuadPart; s->storage.availableBytes = available.QuadPart; s->storageStatus = UMI_STATUS_OK; }
    UmiDesktopSystemReadBoot(options->bootReportPath, s);
}
