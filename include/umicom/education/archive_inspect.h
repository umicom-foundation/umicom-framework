/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/education/archive_inspect.h
 *
 * PURPOSE:
 *   Inspect ordinary Unix ar static-library containers without extracting or
 *   executing their members. Teaching aid for C23 libraries and linkers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDUCATION_ARCHIVE_INSPECT_H
#define UMICOM_EDUCATION_ARCHIVE_INSPECT_H

#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDUCATION_ARCHIVE_NAME_CAPACITY 160U
#define UMI_EDUCATION_ARCHIVE_RECORDED_MEMBERS 24U

/* The report is caller-owned and contains no pointers into an input archive.
 * Member counts include all ordinary entries, while the fixed list retains
 * only the first 24 for display. Metadata such as the ar symbol index and the
 * GNU long-name table is reported separately, never interpreted as object code. */
typedef struct UmiEducationArchiveMember {
    char name[UMI_EDUCATION_ARCHIVE_NAME_CAPACITY];
    uint64_t headerOffset;
    uint64_t bodyBytes;
    int objectFile;
} UmiEducationArchiveMember;

typedef struct UmiEducationArchiveReport {
    uint64_t archiveBytes;
    uint32_t memberCount;
    uint32_t objectCount;
    uint32_t metadataCount;
    uint32_t recordedCount;
    UmiEducationArchiveMember members[UMI_EDUCATION_ARCHIVE_RECORDED_MEMBERS];
} UmiEducationArchiveReport;

/* Parse an entire in-memory ordinary ar archive. GNU and BSD long member names
 * are supported. Thin archives, which reference outside paths, are refused.
 * Header lengths, decimal values, pad bytes and name-table offsets are checked.
 * Failure leaves *outReport unchanged; no member bytes are executed or written.
 * A maximum of 100,000 member headers may be scanned in one invocation. */
UmiStatus UmiEducationArchiveInspectBytes(const unsigned char *bytes,
    size_t byteCount, UmiEducationArchiveReport *outReport);

/* Inspect a selected archive read-only with bounded seeks and header reads.
 * The entire archive is not loaded into memory. The standard C long-based
 * seek interface limits supported file sizes to LONG_MAX on the current host.
 * No recursive scanning, path interpretation, extraction or executable launch. */
UmiStatus UmiEducationArchiveInspectFile(const char *path,
    UmiEducationArchiveReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
