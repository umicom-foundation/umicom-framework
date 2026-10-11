/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/timer_trace.h
 *
 * PURPOSE:
 *   Inspect an explicitly versioned, bounded classroom timer/event trace.
 *   Teach monotonic time, deadline overflow, signal/timeout precedence and
 *   cancellation without controlling live tasks, timers or Kernel interrupts.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_TIMER_TRACE_H
#define UMICOM_EDUCATION_TIMER_TRACE_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_TIMER_HEADER_BYTES 16U
#define UMI_EDUCATION_TIMER_RECORD_BYTES 32U
#define UMI_EDUCATION_TIMER_MAX_RECORDS 4096U
#define UMI_EDUCATION_TIMER_MAX_WAITS 32U
#define UMI_EDUCATION_TIMER_MAX_DELAY 10000000ULL
#define UMI_EDUCATION_TIMER_MAX_FILE_BYTES \
    (UMI_EDUCATION_TIMER_HEADER_BYTES + UMI_EDUCATION_TIMER_RECORD_BYTES * \
     UMI_EDUCATION_TIMER_MAX_RECORDS)

typedef enum UmiEducationTimerEvent {
    UMI_EDUCATION_TIMER_WAIT = 1,
    UMI_EDUCATION_TIMER_SIGNAL = 2,
    UMI_EDUCATION_TIMER_CANCEL = 3,
    UMI_EDUCATION_TIMER_ADVANCE = 4
} UmiEducationTimerEvent;

/* UM I T M R 1 \0 — exact eight-byte magic "UMITMR1\0".
 * 16-byte header: magic[8], LE16 version=1, LE16 flags=0,
 *                 LE32 count (at most 4096).
 * 32-byte records: LE32 ordinal (one-based), U8 event, U8 flags=0,
 *                  LE16 reserved=0, LE32 identity, LE32 event key,
 *                  LE64 logical tick, LE64 value.
 *
 * All action records use the clock most recently established by ADVANCE,
 * beginning at tick zero. ADVANCE must strictly increase the logical tick;
 * its identity, key and value are zero. WAIT uses a new nonzero identity/key;
 * value=0 means no deadline, otherwise value is an interval in logical ticks
 * (1..10,000,000). A finite deadline is checked for unsigned overflow.
 * SIGNAL and CANCEL require matching pending identity/key and value=0;
 * a signal/cancel at or after its finite deadline is refused. ADVANCE settles
 * every newly due finite wait as TIMED_OUT before any later signal. No identity
 * can be reused in the same trace: a stale notification cannot name a new wait.
 * This format records simulated decisions, not native Kernel events, message
 * payloads, system calls, hardware clock rates or credentials.
 */
typedef struct UmiEducationTimerReport {
    uint32_t recordCount;
    uint32_t waits;
    uint32_t signalled;
    uint32_t timedOut;
    uint32_t cancelled;
    uint32_t advances;
    uint32_t peakPending;
    uint32_t pendingAtEnd;
    uint32_t distinctIdentities;
    uint64_t lastTick;
    uint64_t nextFiniteDeadline; /* Zero when no pending finite deadline. */
} UmiEducationTimerReport;

/* No allocation, I/O or global state. Complete bounded parse is atomic with
 * respect to report: invalid input leaves its original bytes unchanged.
 * Input and output must be disjoint; the caller retains input ownership. */
UmiStatus UmiEducationTimerTraceInspect(const void *bytes, size_t length,
                                         UmiEducationTimerReport *report);

/* Read an ordinary local file at an explicit path, with a hard size limit.
 * This does not constitute a sandbox or reject arbitrary caller-selected
 * symlinks; never use this as a security-sensitive file-access policy. */
UmiStatus UmiEducationTimerTraceInspectFile(const char *path,
                                             UmiEducationTimerReport *report);
#ifdef __cplusplus
}
#endif
#endif
