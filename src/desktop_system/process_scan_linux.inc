/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/process_scan_linux.inc
 * PURPOSE: Reuse bounded proc-stat parsing for process catalogues and creation identity checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static UmiStatus ProcessReadAt(const char *root, uint64_t pid, uint64_t page_size,
                               UmiDesktopSystemProcess *out)
{
    char suffix[64], text[8192];
    size_t bytes = 0U;
    int count = snprintf(suffix, sizeof suffix, "%llu/stat", (unsigned long long)pid);
    if (count < 0 || (size_t)count >= sizeof suffix)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = ReadAt(root, suffix, text, sizeof text, &bytes);
    UmiDesktopSystemProcess value;
    if (status == UMI_STATUS_OK)
        status = UmiDesktopSystemParseProcess(text, bytes, page_size, &value);
    if (status == UMI_STATUS_OK && value.pid != pid)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        *out = value;
    return status;
}
UmiStatus UmiDesktopProcessReadNative(uint64_t pid, UmiDesktopSystemProcess *out)
{
    if (out == NULL || pid == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
        return UMI_STATUS_UNAVAILABLE;
    UmiDesktopSystemProcess current;
    UmiStatus status = ProcessReadAt("/proc", pid, (uint64_t)page_size, &current);
    if (status == UMI_STATUS_OK && (current.state == 'Z' || current.state == 'X'))
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK)
        *out = current;
    return status;
}
UmiStatus UmiDesktopProcessScanNative(const UmiDesktopProcessCaptureOptions *options,
                                      UmiDesktopProcessVisitor visitor, void *context,
                                      UmiDesktopProcessReport *report)
{
    const char *root = options->linux_proc_root != NULL ? options->linux_proc_root : "/proc";
    report->fixture = strcmp(root, "/proc") != 0;
    DIR *directory = opendir(root);
    if (directory == NULL)
        return FileError();
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
    {
        closedir(directory);
        return UMI_STATUS_UNAVAILABLE;
    }
    UmiStatus status = UMI_STATUS_OK;
    for (;;)
    {
        if (options->cancelled != NULL && options->cancelled(options->context))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        errno = 0;
        struct dirent *entry = readdir(directory);
        if (entry == NULL)
        {
            if (errno != 0)
                status = UMI_STATUS_IO_ERROR;
            break;
        }
        uint64_t pid;
        if (!NumericPid(entry->d_name, &pid))
            continue;
        ++report->seen;
        if (report->seen > UMI_DESKTOP_SYSTEM_SCAN_LIMIT)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        UmiDesktopSystemProcess process;
        if (ProcessReadAt(root, pid, (uint64_t)page_size, &process) != UMI_STATUS_OK)
        {
            ++report->unreadable;
            continue;
        }
        status = visitor(context, &process);
        if (status != UMI_STATUS_OK)
            break;
    }
    if (closedir(directory) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    return status;
}
static UmiStatus ProcessMonitorKeep(void *context, const UmiDesktopSystemProcess *process)
{
    InsertProcess(context, process);
    return UMI_STATUS_OK;
}
