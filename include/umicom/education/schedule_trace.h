/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/schedule_trace.h
 *
 * PURPOSE:
 *   Inspect a bounded classroom scheduling trace without scheduling a thread,
 *   invoking a kernel, or borrowing any application's task memory.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_SCHEDULE_TRACE_H
#define UMICOM_EDUCATION_SCHEDULE_TRACE_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_SCHEDULE_HEADER_BYTES 16U
#define UMI_EDUCATION_SCHEDULE_RECORD_BYTES 24U
#define UMI_EDUCATION_SCHEDULE_MAX_RECORDS 4096U
#define UMI_EDUCATION_SCHEDULE_MAX_TASKS 32U
#define UMI_EDUCATION_SCHEDULE_MAX_FILE_BYTES \
    (UMI_EDUCATION_SCHEDULE_HEADER_BYTES + \
     UMI_EDUCATION_SCHEDULE_RECORD_BYTES * UMI_EDUCATION_SCHEDULE_MAX_RECORDS)

typedef enum UmiEducationScheduleEvent {
    UMI_EDUCATION_SCHEDULE_ADMIT = 1,
    UMI_EDUCATION_SCHEDULE_DISPATCH = 2,
    UMI_EDUCATION_SCHEDULE_YIELD = 3,
    UMI_EDUCATION_SCHEDULE_WAIT = 4,
    UMI_EDUCATION_SCHEDULE_WAKE = 5,
    UMI_EDUCATION_SCHEDULE_EXIT = 6,
    UMI_EDUCATION_SCHEDULE_CANCEL = 7
} UmiEducationScheduleEvent;

/* Teaching format, NOT the Umicom Kernel ABI or a recorded kernel event stream.
 * Header: 8 bytes "UMISCH1\0", LE16 version=1, LE16 flags=0,
 *         LE32 record count (maximum 4096).
 * Each 24-byte record: LE32 ordinal (1..N), U8 event, U8 flags=0,
 *         LE16 reserved=0, LE32 task identity (nonzero), LE32 detail,
 *         LE64 logical tick (monotonic nondecreasing).
 * DETAIL=0 for ADMIT, DISPATCH, YIELD, EXIT. For WAIT, WAKE and CANCEL,
 * DETAIL identifies one nonzero service/wait key; it must match for wake or
 * cancellation. The teaching trace records no user buffer or payload.
 * Identifiers cannot be reused in one trace. A maximum of 32 tasks may be
 * admitted, including those subsequently exited or cancelled.
 * The ready queue is FIFO: DISPATCH must choose its oldest ready task.
 * Only RUNNING tasks may YIELD, WAIT or EXIT; a WAITING task can WAKE/CANCEL.
 * An externally generated WAKE is not proof of kernel identity or permission.
 */
typedef struct UmiEducationScheduleReport {
    uint32_t recordCount;
    uint32_t admitted;
    uint32_t dispatched;
    uint32_t yielded;
    uint32_t waited;
    uint32_t awakened;
    uint32_t exited;
    uint32_t cancelled;
    uint32_t readyPeak;
    uint32_t waitingPeak;
    uint32_t readyAtEnd;
    uint32_t waitingAtEnd;
    uint32_t liveAtEnd;
    uint32_t runningAtEnd;
    uint64_t lastTick;
} UmiEducationScheduleReport;

/* Reads a complete v1 trace. No allocation, I/O, thread operation or system
 * call. All state is private to this invocation. Invalid input leaves report
 * unchanged. The caller keeps input and output storage disjoint and valid. */
UmiStatus UmiEducationScheduleTraceInspect(const void *bytes, size_t length,
                                            UmiEducationScheduleReport *report);

/* Reads an ordinary, bounded file. Does not modify that file or the report
 * on failure. Explicitly selected local files are not a security boundary. */
UmiStatus UmiEducationScheduleTraceInspectFile(const char *path,
                                                UmiEducationScheduleReport *report);
#ifdef __cplusplus
}
#endif
#endif
