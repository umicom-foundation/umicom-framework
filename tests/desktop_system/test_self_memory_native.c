/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_self_memory_native.c
 *
 * PURPOSE:
 *   Verify native self-memory unit conversion and API failures through inert host boundaries.
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
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "../../src/desktop_system/internal.h"
#include "../../src/desktop_system/process_scan_internal.h"
#include "umicom/desktop_system/self_memory.h"
static const char *mode;
static unsigned identity_calls,memory_calls;
static uint64_t fake_clock(void) { return 900U; }
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>
static HANDLE fake_current(void) { return (HANDLE)(uintptr_t)7U; }
static DWORD fake_pid(void) { return 29U; }
static BOOL fake_times(HANDLE process,LPFILETIME created,LPFILETIME exited,LPFILETIME kernel,LPFILETIME user)
{
    assert(process==fake_current());++identity_calls;
    if(strcmp(mode,"identity-failure")==0) return FALSE;
    *created=(FILETIME){11U,2U};*exited=*kernel=*user=(FILETIME){0};return TRUE;
}
static BOOL fake_memory(HANDLE process,PPROCESS_MEMORY_COUNTERS counters,DWORD size)
{
    assert(process==fake_current() && size==sizeof(PROCESS_MEMORY_COUNTERS_EX));++memory_calls;
    if(strcmp(mode,"memory-failure")==0) return FALSE;
    PROCESS_MEMORY_COUNTERS_EX *extended=(PROCESS_MEMORY_COUNTERS_EX *)counters;
    assert(extended->cb==size);extended->WorkingSetSize=120U;extended->PeakWorkingSetSize=300U;extended->PrivateUsage=220U;
    return TRUE;
}
#define GetCurrentProcess fake_current
#define GetCurrentProcessId fake_pid
#define GetProcessTimes fake_times
#ifdef GetProcessMemoryInfo
#undef GetProcessMemoryInfo
#endif
#define GetProcessMemoryInfo fake_memory
#elif defined(__linux__)
#include <unistd.h>
#include <sys/resource.h>
static pid_t fake_pid(void) { return 29; }
static UmiStatus fake_identity(uint64_t pid,UmiDesktopSystemProcess *out)
{
    assert(pid==29U);++identity_calls;
    if(strcmp(mode,"identity-failure")==0) return UMI_STATUS_IO_ERROR;
    *out=(UmiDesktopSystemProcess){0};out->pid=29U;out->startTicks=11U;
    out->startKnown=1;out->residentKnown=1;out->residentBytes=120U;
    if(strcmp(mode,"unknown-resident")==0) out->residentKnown=0;
    return UMI_STATUS_OK;
}
static int fake_usage(int who,struct rusage *out)
{
    assert(who==RUSAGE_SELF);++memory_calls;memset(out,0,sizeof(*out));
    if(strcmp(mode,"peak-failure")==0) return -1;
    out->ru_maxrss=3;return 0;
}
#define getpid fake_pid
#define UmiDesktopProcessReadNative fake_identity
#define getrusage fake_usage
#endif
#define UmiDesktopSystemClock fake_clock
#define UmiDesktopSelfMemoryReadNative UmiTestSelfMemoryNative
#include "../../src/desktop_system/self_memory_native.c"
#undef UmiDesktopSelfMemoryReadNative
#undef UmiDesktopSystemClock
int main(int argc,char **argv)
{
    assert(argc==2);mode=argv[1];UmiSelfMemorySample out,saved;
    memset(&out,0x5a,sizeof(out));memcpy(&saved,&out,sizeof(saved));
    UmiStatus wanted=UMI_STATUS_OK;
    if(strcmp(mode,"identity-failure")==0) wanted=UMI_STATUS_IO_ERROR;
#if defined(_WIN32)
    else if(strcmp(mode,"memory-failure")==0) wanted=UMI_STATUS_IO_ERROR;
    else assert(strcmp(mode,"valid")==0);
#elif defined(__linux__)
    else if(strcmp(mode,"unknown-resident")==0) wanted=UMI_STATUS_UNAVAILABLE;
    else assert(strcmp(mode,"valid")==0 || strcmp(mode,"peak-failure")==0);
#endif
    assert(UmiTestSelfMemoryNative(&out)==wanted);assert(identity_calls==1U);
    assert(memory_calls==((strcmp(mode,"identity-failure")==0 || strcmp(mode,"unknown-resident")==0)?0U:1U));
    if(wanted!=UMI_STATUS_OK) { assert(memcmp(&out,&saved,sizeof(out))==0);return 0; }
    assert(out.pid==29U && out.captured_milliseconds==900U && out.resident_bytes==120U);
#if defined(_WIN32)
    assert(out.provider==UMI_SELF_MEMORY_WINDOWS && out.known_metrics==UMI_SELF_MEMORY_ALL);
    assert(out.creation_ticks==((UINT64_C(2)<<32U)|11U));
    assert(out.peak_resident_bytes==300U && out.private_committed_bytes==220U);
#else
    assert(out.provider==UMI_SELF_MEMORY_LINUX && out.private_committed_bytes==0U);
    assert(out.known_metrics==(strcmp(mode,"peak-failure")==0?UMI_SELF_MEMORY_RESIDENT:(UMI_SELF_MEMORY_RESIDENT|UMI_SELF_MEMORY_PEAK_RESIDENT)));
    assert(out.peak_resident_bytes==(strcmp(mode,"peak-failure")==0?0U:3072U));
#endif
    return 0;
}
