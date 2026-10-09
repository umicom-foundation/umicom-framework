/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/process_scan_windows.inc
 * PURPOSE: Enumerate local Windows process metadata with explicit cancellation and handle ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiDesktopProcessReadNative(uint64_t pid, UmiDesktopSystemProcess *out)
{
    if (out == NULL || pid == 0U || pid > UINT32_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)pid);
    if (process == NULL)
        return GetLastError() == ERROR_INVALID_PARAMETER ? UMI_STATUS_NOT_FOUND : WinError();
    UmiDesktopSystemProcess value = {0};
    value.pid = pid;
    value.state = '?';
    FILETIME created, exited, kernel, user;
    UmiStatus status = UMI_STATUS_OK;
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user))
        status = WinError();
    else if (TimeValue(exited) != 0U)
        status = UMI_STATUS_NOT_FOUND;
    else
    {
        value.startKnown = 1;
        value.startTicks = TimeValue(created);
    }
    if (!CloseHandle(process) && status == UMI_STATUS_OK)
        status = WinError();
    if (status == UMI_STATUS_OK)
        *out = value;
    return status;
}
UmiStatus UmiDesktopProcessScanNative(const UmiDesktopProcessCaptureOptions *options,
                                      UmiDesktopProcessVisitor visitor, void *context,
                                      UmiDesktopProcessReport *report)
{
    if (options->linux_proc_root != NULL)
        return UMI_STATUS_NOT_IMPLEMENTED;
    HANDLE list = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (list == INVALID_HANDLE_VALUE)
        return WinError();
    PROCESSENTRY32W entry;
    memset(&entry, 0, sizeof entry);
    entry.dwSize = sizeof entry;
    BOOL more = Process32FirstW(list, &entry);
    UmiStatus status = UMI_STATUS_OK;
    while (more)
    {
        if (options->cancelled != NULL && options->cancelled(options->context))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        ++report->seen;
        if (report->seen > UMI_DESKTOP_SYSTEM_SCAN_LIMIT)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        UmiDesktopSystemProcess process = {0};
        process.pid = entry.th32ProcessID;
        process.parentPid = entry.th32ParentProcessID;
        process.state = '?';
        Name(entry.szExeFile, process.name, sizeof process.name);
        /* Creation time needs only limited query access. A protected process
         * remains a visible row with unknown identity rather than guessed time. */
        UmiDesktopSystemProcess identity;
        if (process.pid != 0U &&
            UmiDesktopProcessReadNative(process.pid, &identity) == UMI_STATUS_OK)
        {
            process.startKnown = identity.startKnown;
            process.startTicks = identity.startTicks;
        }
        else
            ++report->unreadable;
        status = visitor(context, &process);
        if (status != UMI_STATUS_OK)
            break;
        more = Process32NextW(list, &entry);
    }
    if (status == UMI_STATUS_OK && !more && GetLastError() != ERROR_NO_MORE_FILES)
        status = WinError();
    if (!CloseHandle(list) && status == UMI_STATUS_OK)
        status = WinError();
    return status;
}
static UmiStatus ProcessMonitorKeep(void *context, const UmiDesktopSystemProcess *process)
{
    Insert(context, *process);
    return UMI_STATUS_OK;
}
static void ProcessMonitorCapture(UmiDesktopSystemSnapshot *snapshot)
{
    UmiDesktopProcessCaptureOptions options = {0};
    UmiDesktopProcessReport report = {0};
    snapshot->processStatus =
        UmiDesktopProcessScanNative(&options, ProcessMonitorKeep, snapshot, &report);
    snapshot->processesSeen = report.seen;
    snapshot->processesUnreadable = report.unreadable;
    if (snapshot->processStatus == UMI_STATUS_OK && report.seen > UMI_DESKTOP_SYSTEM_PROCESS_LIMIT)
        snapshot->processStatus = UMI_STATUS_CAPACITY_EXCEEDED;
}
