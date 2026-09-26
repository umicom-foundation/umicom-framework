/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/monitor.h
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESKTOP_SYSTEM_MONITOR_H
#define UMICOM_DESKTOP_SYSTEM_MONITOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/boot_report.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DESKTOP_SYSTEM_PROCESS_LIMIT 128U
#define UMI_DESKTOP_SYSTEM_SCAN_LIMIT 4096U
#define UMI_DESKTOP_SYSTEM_INTERFACE_LIMIT 32U
#define UMI_DESKTOP_SYSTEM_PATH_CAPACITY 2048U
#define UMI_DESKTOP_SYSTEM_NAME_CAPACITY 128U

typedef struct UmiDesktopSystemMemory {
    uint64_t totalBytes;
    uint64_t availableBytes;
} UmiDesktopSystemMemory;
/* Linux: user, nice, system, idle, iowait, irq, softirq, steal.
 * Windows uses user, zero, kernel-minus-idle, idle, and four zero fields.
 * Multi-processor-group Windows hosts are explicitly unsupported by the CPU
 * adapter; it must not compare different thread groups as one machine.
 * Counters from different machines/backends must never be compared. */
typedef struct UmiDesktopSystemCpu { uint64_t ticks[8]; } UmiDesktopSystemCpu;
typedef struct UmiDesktopSystemProcess {
    uint64_t pid;
    uint64_t parentPid;
    uint64_t startTicks;
    uint64_t residentBytes;
    int residentKnown;
    int startKnown;
    char state;
    char name[UMI_DESKTOP_SYSTEM_NAME_CAPACITY];
} UmiDesktopSystemProcess;
typedef struct UmiDesktopSystemInterface {
    char name[UMI_DESKTOP_SYSTEM_NAME_CAPACITY];
    uint64_t receivedBytes;
    uint64_t transmittedBytes;
} UmiDesktopSystemInterface;
typedef struct UmiDesktopSystemStorage {
    uint64_t totalBytes;
    uint64_t availableBytes;
    char path[UMI_DESKTOP_SYSTEM_PATH_CAPACITY];
} UmiDesktopSystemStorage;
typedef struct UmiDesktopSystemSnapshot {
    char source[UMI_DESKTOP_SYSTEM_PATH_CAPACITY];
    uint64_t capturedMilliseconds; /* monotonic in this process's host */
    uint64_t elapsedMilliseconds;
    int fixture;
    UmiStatus memoryStatus, cpuStatus, processStatus, networkStatus, storageStatus, bootStatus;
    UmiDesktopSystemMemory memory;
    UmiDesktopSystemCpu cpu;
    UmiDesktopSystemStorage storage;
    UmiBootReport boot;
    UmiDesktopSystemProcess processes[UMI_DESKTOP_SYSTEM_PROCESS_LIMIT];
    size_t processCount;
    size_t processesSeen;
    size_t processesUnreadable;
    UmiDesktopSystemInterface interfaces[UMI_DESKTOP_SYSTEM_INTERFACE_LIMIT];
    size_t interfaceCount;
    size_t interfacesSeen;
} UmiDesktopSystemSnapshot;
typedef struct UmiDesktopSystemOptions {
    /* NULL selects the local root volume. No database is opened. */
    const char *storagePath;
    /* NULL selects /run/umicom/boot.report on Linux. On Windows it is unavailable
     * unless an explicit exported report file is chosen. */
    const char *bootReportPath;
    /* NULL selects real /proc. An alternative is explicitly labelled FIXTURE.
     * Never set this in a production GTK host. Windows rejects this option. */
    const char *linuxProcRoot;
} UmiDesktopSystemOptions;
/* Read-only capture. Arguments and overall allocation errors are returned here;
 * each source has its OWN status. OK does not imply that every source succeeded.
 * The output is replaced only when a capture was made. No previous values are
 * retained after a source fails. Process enumeration is best-effort, not an
 * atomic system snapshot; inaccessible/exited records and caps remain visible.
 * May block in OS filesystem calls. Call from a worker for graphical clients.
 * No process command lines, environments, addresses or credentials are read. */
UmiStatus UmiDesktopSystemCapture(const UmiDesktopSystemOptions *options,
    UmiDesktopSystemSnapshot *outSnapshot);
/* Compute floor(busy / elapsed * 10000), excluding Linux idle+iowait from busy.
 * Rejects counter decreases, zero elapsed ticks and overflowing deltas. Output
 * remains unchanged on error. The first capture establishes a baseline only. */
UmiStatus UmiDesktopSystemCpuUsage(const UmiDesktopSystemCpu *before,
    const UmiDesktopSystemCpu *after, uint32_t *outBasisPoints);
/* A bounded JSON representation. Query with NULL/0 to obtain the required byte
 * count INCLUDING NUL. Capacity failure clears nonempty output, never emits a
 * partial JSON document. All external strings are escaped. Snapshots should
 * come from Capture, not be assembled from untrusted raw structure bytes. */
UmiStatus UmiDesktopSystemJson(const UmiDesktopSystemSnapshot *snapshot,
    char *outText, size_t capacity, size_t *outRequired);
#ifdef __cplusplus
}
#endif
#endif
