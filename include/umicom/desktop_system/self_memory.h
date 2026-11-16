/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_system/self_memory.h
 *
 * PURPOSE:
 *   Observe the calling process memory and compare identity-bound samples without interpreting growth as a leak.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_DESKTOP_SELF_MEMORY_H
#define UMICOM_DESKTOP_SELF_MEMORY_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SELF_MEMORY_RESIDENT 1U
#define UMI_SELF_MEMORY_PEAK_RESIDENT 2U
#define UMI_SELF_MEMORY_PRIVATE_COMMITTED 4U
#define UMI_SELF_MEMORY_ALL 7U
typedef enum UmiSelfMemoryProvider {
    UMI_SELF_MEMORY_WINDOWS=1,
    UMI_SELF_MEMORY_LINUX=2
} UmiSelfMemoryProvider;
typedef struct UmiSelfMemorySample {
    UmiSelfMemoryProvider provider;
    unsigned int known_metrics;
    uint64_t pid,creation_ticks,captured_milliseconds;
    uint64_t resident_bytes,peak_resident_bytes,private_committed_bytes;
} UmiSelfMemorySample;
/* Explicit read-only observation of THIS process, not its model-server children.
 * Windows: current/peak working set and private committed bytes. Linux: resident
 * bytes from the existing proc-stat reader and optional peak RSS from getrusage;
 * private committed remains unknown. Do not equate committed bytes with RSS.
 * Each metric is valid only when its known_metrics bit is set; unknown is zero
 * storage, not an observed zero. Peak is a lifetime high-water mark, not a reset
 * at an application's stage boundary. Linux observations are not atomic.
 *
 * Reuse native creation identity and the desktop monotonic clock. No external
 * processes, credentials, model weights, network or privileges are requested.
 * OS calls may block; use the application's existing worker for frequent polls.
 * Failure preserves output. No retained owner, heap-sized sample history or
 * automatic periodic work is created. Unsupported hosts return NOT_IMPLEMENTED.
 * A sample is not a leak diagnosis, memory reservation or device-memory report. */
UmiStatus UmiDesktopSelfMemoryCapture(UmiSelfMemorySample *out_sample);

typedef struct UmiMemoryMovement {
    uint64_t bytes;
    int direction; /* -1 decrease, 0 unchanged, +1 increase; no signed overflow. */
} UmiMemoryMovement;
typedef struct UmiSelfMemoryDifference {
    unsigned int common_metrics;
    uint64_t elapsed_milliseconds;
    UmiMemoryMovement resident,peak_resident,private_committed;
} UmiSelfMemoryDifference;
/* Compare samples from the same host, provider, PID and process creation.
 * The caller owns host/provenance and stage identity; this is not authentication.
 * Reject changed identities, reversed timestamps and decreasing lifetime peaks.
 * Compare only jointly known metrics. No common metric returns UNAVAILABLE.
 * A positive difference can be useful retained data or allocator behavior; it
 * does not prove a leak. Inputs remain borrowed and stable for this pure call.
 * All failures preserve output. No percentage rounding or signed byte overflow. */
UmiStatus UmiDesktopSelfMemoryCompare(const UmiSelfMemorySample *before,
    const UmiSelfMemorySample *after,UmiSelfMemoryDifference *out_difference);
#ifdef __cplusplus
}
#endif
#endif
