/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/evidence_trace.h
 *
 * PURPOSE:
 *   Inspect a bounded, chained classroom trace of fictional driver lifetimes,
 *   failure observations and review checkpoints. No privileged devices, driver
 *   operations or actual recovery are performed by this interface.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_EVIDENCE_TRACE_H
#define UMICOM_EDUCATION_EVIDENCE_TRACE_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_EVIDENCE_HEADER_BYTES 24U
#define UMI_EDUCATION_EVIDENCE_RECORD_BYTES 80U
#define UMI_EDUCATION_EVIDENCE_METADATA_BYTES 48U
#define UMI_EDUCATION_EVIDENCE_HASH_BYTES 32U
#define UMI_EDUCATION_EVIDENCE_MAX_RECORDS 4096U
#define UMI_EDUCATION_EVIDENCE_MAX_DRIVERS 8U
#define UMI_EDUCATION_EVIDENCE_MAX_FILE_BYTES \
    (UMI_EDUCATION_EVIDENCE_HEADER_BYTES + \
     UMI_EDUCATION_EVIDENCE_RECORD_BYTES * UMI_EDUCATION_EVIDENCE_MAX_RECORDS)

/* This is a documented teaching format, not a production Kernel trace ABI.
 * All integer fields are little-endian and no C struct is written directly.
 * Header (24 bytes):
 *    0: 8-byte magic  'UMIEVD1\0'
 *    8: u16 version=1
 *   10: u16 reserved=0
 *   12: u32 recordCount <= 4096
 *   16: u32 flags=0
 *   20: u32 reserved=0
 * Record (80 bytes):
 *    0: u32 ordinal (starts at 1, contiguous)
 *    4: u8 kind (enumeration below)
 *    5: u8 severity (0 information, 1 warning, 2 error)
 *    6: u16 reserved=0
 *    8: u32 driver slot (1..8)
 *   12: u32 owner identity (nonzero)
 *   16: u32 generation (nonzero; increases whenever slot is reused)
 *   20: u32 diagnostic code (nonzero for OBSERVE/FAULT only)
 *   24: u64 logical tick (never decreases)
 *   32: u64 observation identifier (globally increasing for OBSERVE/FAULT)
 *   40: u64 checkpoint identifier (OBSERVE id, only for CHECKPOINT/RECOVER)
 *   48: SHA256(previous record digest || bytes[0..47]), where the initial
 *       previous digest consists of 32 zero bytes.
 *
 * SHA-256 detects accidental alterations to a file, but an attacker who can
 * rewrite the whole file can recompute the chain. This is NOT a digital
 * signature, authentication, provenance proof or durable write-ahead log.
 * CHECKPOINT records reference an earlier OBSERVE, not a persisted snapshot.
 * RECOVER records are claims of a reviewed decision, not proof of device repair.
 */
typedef enum UmiEducationEvidenceEvent {
    UMI_EDUCATION_EVIDENCE_ATTACH = 1,
    UMI_EDUCATION_EVIDENCE_READY = 2,
    UMI_EDUCATION_EVIDENCE_OBSERVE = 3,
    UMI_EDUCATION_EVIDENCE_CHECKPOINT = 4,
    UMI_EDUCATION_EVIDENCE_FAULT = 5,
    UMI_EDUCATION_EVIDENCE_RECOVER = 6,
    UMI_EDUCATION_EVIDENCE_QUIESCE = 7,
    UMI_EDUCATION_EVIDENCE_STOP = 8,
    UMI_EDUCATION_EVIDENCE_RELEASE = 9
} UmiEducationEvidenceEvent;

typedef struct UmiEducationEvidenceReport {
    uint32_t recordCount;
    uint32_t attached;
    uint32_t ready;
    uint32_t observations;
    uint32_t checkpoints;
    uint32_t faults;
    uint32_t recoveries;
    uint32_t quiesced;
    uint32_t stopped;
    uint32_t released;
    uint32_t liveAtEnd;
    uint32_t faultedAtEnd;
    uint32_t stoppedAtEnd;
    uint32_t peakLive;
    uint64_t lastTick;
    uint64_t lastObservationId;
    unsigned char lastDigest[UMI_EDUCATION_EVIDENCE_HASH_BYTES];
} UmiEducationEvidenceReport;

/* No heap allocation or OS calls. The borrowed input and caller output must
 * not alias. On error, *report remains completely unchanged. */
UmiStatus UmiEducationEvidenceTraceInspect(const void *bytes, size_t length,
                                            UmiEducationEvidenceReport *report);

/* Read at most MAX_FILE_BYTES from an explicit local file. The path is not
 * sandboxed; the caller decides which file may be opened. Results are published
 * only after successful reading, parsing AND closing. */
UmiStatus UmiEducationEvidenceTraceInspectFile(const char *path,
                                                UmiEducationEvidenceReport *report);
#ifdef __cplusplus
}
#endif
#endif
