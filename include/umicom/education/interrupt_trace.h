/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/interrupt_trace.h
 *
 * PURPOSE:
 *   Review a deliberately small, versioned teaching trace of interrupt-source
 *   ownership, acknowledgement, cancellation and explicitly reviewed recovery.
 *   This does not read real privileged state or control a hardware interrupt.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_INTERRUPT_TRACE_H
#define UMICOM_EDUCATION_INTERRUPT_TRACE_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_INTERRUPT_HEADER_BYTES 16U
#define UMI_EDUCATION_INTERRUPT_RECORD_BYTES 32U
#define UMI_EDUCATION_INTERRUPT_MAX_RECORDS 4096U
#define UMI_EDUCATION_INTERRUPT_MAX_SOURCES 8U
#define UMI_EDUCATION_INTERRUPT_MAX_FILE_BYTES \
    (UMI_EDUCATION_INTERRUPT_HEADER_BYTES + UMI_EDUCATION_INTERRUPT_RECORD_BYTES * \
     UMI_EDUCATION_INTERRUPT_MAX_RECORDS)

/* Teaching wire format, NOT a Umicom Kernel binary ABI or a platform trace:
 * 16 bytes: ASCII "UMIIRQ1\0" (8), LE16 version=1, LE16 flags=0, LE32 count.
 * 32 bytes per record:
 *   offset  0: LE32 one-based ordinal, contiguous
 *   offset  4: U8 event code below
 *   offset  5: U8 reserved=0
 *   offset  6: LE16 source (1..8)
 *   offset  8: LE32 owner identity, nonzero
 *   offset 12: LE32 generation, nonzero and greater on every fresh CLAIM
 *   offset 16: LE64 logical tick, never decreases
 *   offset 24: LE64 detail, zero except for FAULT (1..255 diagnostic)
 *
 * State progression:
 * FREE --CLAIM--> LEASED --ARM--> ARMED --ENTER--> SERVICING
 *   SERVICING --ACK--> ACKED --RETURN--> LEASED
 *   SERVICING/ACKED --FAULT--> FAULTED --RECOVER--> LEASED
 *   ARMED --CANCEL--> LEASED; LEASED --RELEASE--> FREE
 *
 * Recovery is an educational record stating that a trusted owner has reviewed
 * and resolved a fictional fault. It must never be treated as evidence of
 * actual machine-state restoration. A cancellation does not acknowledge an
 * interrupt already in service; such a case requires separate fault review.
 * Generation counters prevent stale-owner reuse of a source slot.
 */
typedef enum UmiEducationInterruptEvent {
    UMI_EDUCATION_INTERRUPT_CLAIM = 1,
    UMI_EDUCATION_INTERRUPT_ARM = 2,
    UMI_EDUCATION_INTERRUPT_ENTER = 3,
    UMI_EDUCATION_INTERRUPT_ACK = 4,
    UMI_EDUCATION_INTERRUPT_RETURN = 5,
    UMI_EDUCATION_INTERRUPT_CANCEL = 6,
    UMI_EDUCATION_INTERRUPT_FAULT = 7,
    UMI_EDUCATION_INTERRUPT_RECOVER = 8,
    UMI_EDUCATION_INTERRUPT_RELEASE = 9
} UmiEducationInterruptEvent;

typedef struct UmiEducationInterruptReport {
    uint32_t recordCount;
    uint32_t claims;
    uint32_t arms;
    uint32_t entries;
    uint32_t acknowledgements;
    uint32_t returns;
    uint32_t cancellations;
    uint32_t faults;
    uint32_t recoveries;
    uint32_t releases;
    uint32_t ownedAtEnd;
    uint32_t activeAtEnd;
    uint32_t faultedAtEnd;
    uint32_t peakOwned;
    uint64_t lastTick;
} UmiEducationInterruptReport;

/* Complete in-memory validation publishes the report only on full success.
 * The caller owns the input. No allocation, global state, sockets or OS calls.
 * Non-overlapping input/output storage is required for a reliable result. */
UmiStatus UmiEducationInterruptTraceInspect(const void *bytes, size_t length,
                                             UmiEducationInterruptReport *report);

/* Read a caller-specified local file with the fixed maximum-size policy.
 * File selection is NOT sandboxed or restricted by this teaching interface. */
UmiStatus UmiEducationInterruptTraceInspectFile(const char *path,
                                                 UmiEducationInterruptReport *report);
#ifdef __cplusplus
}
#endif
#endif
