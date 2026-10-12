/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/device_io_trace.h
 * PURPOSE:
 *   Inspect an intentionally bounded teaching trace of device requests,
 *   cancellation, completion acknowledgement and explicit release.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_DEVICE_IO_TRACE_H
#define UMICOM_EDUCATION_DEVICE_IO_TRACE_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_DEVICE_IO_HEADER_BYTES 24U
#define UMI_EDUCATION_DEVICE_IO_RECORD_BYTES 40U
#define UMI_EDUCATION_DEVICE_IO_MAX_RECORDS 4096U
#define UMI_EDUCATION_DEVICE_IO_MAX_DEVICES 8U
#define UMI_EDUCATION_DEVICE_IO_MAX_REQUESTS 8U
#define UMI_EDUCATION_DEVICE_IO_COMPLETION_CAPACITY 4U
#define UMI_EDUCATION_DEVICE_IO_MAX_TRANSFER_BYTES 4096U
#define UMI_EDUCATION_DEVICE_IO_MAX_FILE_BYTES \
    (UMI_EDUCATION_DEVICE_IO_HEADER_BYTES + \
     UMI_EDUCATION_DEVICE_IO_RECORD_BYTES * UMI_EDUCATION_DEVICE_IO_MAX_RECORDS)

/* The file is a classroom record, NOT the Umicom Kernel ABI, a block-driver
 * protocol or a privileged hardware trace. Every integer is little-endian;
 * C structure padding is never part of the file.
 *
 * Header: 24 bytes
 *   0..7   ASCII magic "UMIDIO1\0"
 *   8      u16 version (1)
 *   10     u16 reserved (0)
 *   12     u32 record count (0..4096)
 *   16     u32 flags (0)
 *   20     u32 reserved (0)
 *
 * Record: 40 bytes
 *   0      u32 contiguous ordinal, starting at 1
 *   4      u8 event (enum below)
 *   5      u8 result: for COMPLETE only, 0 success, 1 failed, 2 cancelled
 *   6      u16 reserved (0)
 *   8      u32 device slot (1..8)
 *   12     u32 owner identity (nonzero)
 *   16     u32 device generation (nonzero and increasing on reattach)
 *   20     u32 request identity (only SUBMIT, COMPLETE, CANCEL_REQUEST, REAP)
 *   24     u32 transfer bytes: requested by SUBMIT, returned by COMPLETE;
 *                   otherwise zero. Maximum requested size: 4096.
 *   28     u64 monotonically nondecreasing logical tick
 *   36     u32 reserved (0)
 *
 * SUBMIT identities increase globally and are never reused in one file.
 * CANCEL_REQUEST records intent, NOT completed cancellation. A cancelled
 * operation still requires a COMPLETE(result=2) and a REAP. It may complete
 * successfully or fail despite an earlier cancellation request. REAP consumes
 * the oldest *completed* request (completion FIFO), not necessarily the oldest
 * submitted request. Completion records are bounded independently from active
 * requests. QUIESCE refuses new submissions but permits in-flight completions.
 * DETACH requires QUIESCE and all outstanding requests reaped.
 * The inspector never issues I/O, reads a device or touches kernel state.
 */
typedef enum UmiEducationDeviceIoEvent {
    UMI_EDUCATION_DEVICE_IO_ATTACH = 1,
    UMI_EDUCATION_DEVICE_IO_SUBMIT = 2,
    UMI_EDUCATION_DEVICE_IO_COMPLETE = 3,
    UMI_EDUCATION_DEVICE_IO_CANCEL_REQUEST = 4,
    UMI_EDUCATION_DEVICE_IO_REAP = 5,
    UMI_EDUCATION_DEVICE_IO_QUIESCE = 6,
    UMI_EDUCATION_DEVICE_IO_RESUME = 7,
    UMI_EDUCATION_DEVICE_IO_DETACH = 8
} UmiEducationDeviceIoEvent;

typedef enum UmiEducationDeviceIoOutcome {
    UMI_EDUCATION_DEVICE_IO_SUCCESS = 0,
    UMI_EDUCATION_DEVICE_IO_FAILURE = 1,
    UMI_EDUCATION_DEVICE_IO_CANCELLED = 2
} UmiEducationDeviceIoOutcome;

typedef struct UmiEducationDeviceIoReport {
    uint32_t recordCount;
    uint32_t attachments;
    uint32_t submissions;
    uint32_t completions;
    uint32_t cancellationRequests;
    uint32_t reaped;
    uint32_t quiesced;
    uint32_t resumed;
    uint32_t detached;
    uint32_t succeeded;
    uint32_t failed;
    uint32_t cancellationAcknowledgements;
    uint32_t peakOutstanding;
    uint32_t peakReadyCompletions;
    uint32_t outstandingAtEnd;
    uint32_t readyCompletionsAtEnd;
    uint32_t liveDevicesAtEnd;
    uint64_t bytesRequested;
    uint64_t bytesCompleted;
    uint64_t lastTick;
} UmiEducationDeviceIoReport;

/* The caller owns both buffers. On any error, the report is byte-for-byte
 * unchanged. A valid partial trace may still contain live requests: readers
 * must consult outstandingAtEnd and readyCompletionsAtEnd. */
UmiStatus UmiEducationDeviceIoTraceInspect(const void *bytes, size_t length,
                                            UmiEducationDeviceIoReport *report);
/* Read one explicitly chosen bounded local file. Failure, including fclose,
 * never publishes a partially inspected result. This is not path sandboxing. */
UmiStatus UmiEducationDeviceIoTraceInspectFile(const char *path,
                                                UmiEducationDeviceIoReport *report);
#ifdef __cplusplus
}
#endif
#endif
