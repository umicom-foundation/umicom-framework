/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop_system/self_memory_native.c
 *
 * PURPOSE:
 *   Capture only the current process memory through native Windows or existing Linux identity readers with explicit metric availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#if defined(__linux__) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "self_memory_internal.h"
#include "internal.h"
#include "process_scan_internal.h"
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/resource.h>
#endif
UmiStatus UmiDesktopSelfMemoryReadNative(UmiSelfMemorySample *out)
{
#if defined(_WIN32)
    /* GetCurrentProcess is a borrowed pseudo-handle. It must not be closed or
     * replaced with a broad query of every running process. */
    HANDLE current=GetCurrentProcess();
    FILETIME created,exited,kernel,user;
    if(!GetProcessTimes(current,&created,&exited,&kernel,&user)) return UMI_STATUS_IO_ERROR;
    PROCESS_MEMORY_COUNTERS_EX counters={0};counters.cb=(DWORD)sizeof(counters);
    if(!GetProcessMemoryInfo(current,(PROCESS_MEMORY_COUNTERS *)&counters,(DWORD)sizeof(counters)))
        return UMI_STATUS_IO_ERROR;
    UmiSelfMemorySample sample={0};sample.provider=UMI_SELF_MEMORY_WINDOWS;
    sample.pid=(uint64_t)GetCurrentProcessId();
    sample.creation_ticks=((uint64_t)created.dwHighDateTime<<32U)|created.dwLowDateTime;
    sample.captured_milliseconds=UmiDesktopSystemClock();sample.known_metrics=UMI_SELF_MEMORY_ALL;
    sample.resident_bytes=(uint64_t)counters.WorkingSetSize;
    sample.peak_resident_bytes=(uint64_t)counters.PeakWorkingSetSize;
    sample.private_committed_bytes=(uint64_t)counters.PrivateUsage;
    *out=sample;return UMI_STATUS_OK;
#elif defined(__linux__)
    pid_t self=getpid();if(self<=0) return UMI_STATUS_IO_ERROR;
    UmiDesktopSystemProcess process;
    UmiStatus status=UmiDesktopProcessReadNative((uint64_t)self,&process);
    if(status!=UMI_STATUS_OK) return status;
    if(!process.startKnown || !process.residentKnown || process.pid!=(uint64_t)self)
        return UMI_STATUS_UNAVAILABLE;
    UmiSelfMemorySample sample={0};sample.provider=UMI_SELF_MEMORY_LINUX;
    sample.pid=process.pid;sample.creation_ticks=process.startTicks;
    sample.known_metrics=UMI_SELF_MEMORY_RESIDENT;sample.resident_bytes=process.residentBytes;
    struct rusage usage;
    if(getrusage(RUSAGE_SELF,&usage)==0 && usage.ru_maxrss>=0 &&
       (uint64_t)usage.ru_maxrss<=UINT64_MAX/1024U) {
        sample.peak_resident_bytes=(uint64_t)usage.ru_maxrss*1024U;
        sample.known_metrics|=UMI_SELF_MEMORY_PEAK_RESIDENT;
    }
    /* Linux does not provide the Windows private-commit metric here. Leave its
     * known bit absent rather than substituting virtual address space size. */
    sample.captured_milliseconds=UmiDesktopSystemClock();*out=sample;return UMI_STATUS_OK;
#else
    (void)out;return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
