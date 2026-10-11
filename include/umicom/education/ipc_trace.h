/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/ipc_trace.h
 * PURPOSE: Inspect a bounded educational IPC trace without creating channels,
 *          executing requests or interpreting untrusted bytes as pointers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_IPC_TRACE_H
#define UMICOM_EDUCATION_IPC_TRACE_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_IPC_TRACE_HEADER_BYTES 16U
#define UMI_EDUCATION_IPC_TRACE_RECORD_BYTES 16U
#define UMI_EDUCATION_IPC_TRACE_MAX_BYTES (1024U * 1024U)
#define UMI_EDUCATION_IPC_TRACE_MAX_RECORDS 4096U
#define UMI_EDUCATION_IPC_TRACE_MAX_PAYLOAD 256U
#define UMI_EDUCATION_IPC_TRACE_MAX_PENDING 64U

/* This is the v1 educational disk format, not Umicom Kernel's syscall ABI:
 * Header: magic "UMIIPC1\\0" (8 bytes), LE16 version=1, LE16 flags=0,
 *         LE32 number of records.
 * Record: LE32 ordinal (1..N), U8 kind (1=send, 2=receive), U8 flags=0,
 *         LE16 payload length, LE32 sender ID, LE32 recipient ID,
 *         followed by exactly payload length bytes.
 * Send copies 1..256 bytes into a model queue. Receive has zero payload and
 * consumes only the oldest matching sender/recipient pair. IDs must be nonzero
 * and different. At most 64 sends may remain outstanding. Unconsumed sends are
 * permitted at EOF. No I/O, IPC, threads or kernel calls occur in the parser.
 */
typedef struct UmiEducationIpcTraceReport {
    uint32_t recordCount;
    uint32_t sendCount;
    uint32_t receiveCount;
    uint32_t pendingCount;
    uint32_t highWater;
    uint64_t payloadBytes;
} UmiEducationIpcTraceReport;

/* Failure leaves report unchanged. Only a complete, well-formed trace succeeds.
 * No source/report aliasing is allowed. This function does not allocate memory.
 * It is a teaching inspector; it does not authenticate participants or prove
 * that actual Kernel IPC operations took place. */
UmiStatus UmiEducationIpcTraceInspect(const void *data, size_t byteCount,
                                      UmiEducationIpcTraceReport *report);

/* Read at most one MiB from an ordinary file and invoke the same bounded parser.
 * The selected input is read-only. Report remains unchanged on any failure. */
UmiStatus UmiEducationIpcTraceInspectFile(const char *path,
                                          UmiEducationIpcTraceReport *report);

#ifdef __cplusplus
}
#endif
#endif
