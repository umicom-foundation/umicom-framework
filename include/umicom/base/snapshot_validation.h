/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/base/snapshot_validation.h
 * PURPOSE: Validate bounded text records and describe rejected batch input.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_SNAPSHOT_VALIDATION_H
#define UMICOM_BASE_SNAPSHOT_VALIDATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiSnapshotIssue {
    UMI_SNAPSHOT_VALID = 0,
    UMI_SNAPSHOT_NULL_RECORD = 1,
    UMI_SNAPSHOT_INVALID_SCHEMA = 2,
    UMI_SNAPSHOT_UNTERMINATED_TEXT = 3,
    UMI_SNAPSHOT_EMPTY_REQUIRED_TEXT = 4,
    /* Scoped publication cannot replace another document's records. */
    UMI_SNAPSHOT_SCOPE_MISMATCH = 6,
    UMI_SNAPSHOT_DUPLICATE_ID = 5
} UmiSnapshotIssue;

/** Describe a fixed-size char array inside a C record. offset and capacity
 * are byte counts, normally obtained with offsetof and sizeof. required is
 * either zero (empty text allowed) or one (at least one nonzero byte required).
 * name is a borrowed diagnostic label; do not put user data or secrets here. */
typedef struct UmiSnapshotTextField {
    const char *name;
    size_t offset;
    size_t capacity;
    int required;
} UmiSnapshotTextField;

/** A validation result contains no copied input text. field borrows the schema
 * label; typed Framework validators use static labels. field_index is SIZE_MAX
 * when no individual field applies. Success clears all details except that
 * sentinel. On failure, capacity describes the rejected field's byte capacity. */
typedef struct UmiSnapshotValidation {
    UmiSnapshotIssue issue;
    const char *field;
    size_t field_index;
    size_t capacity;
} UmiSnapshotValidation;

/** Validate every schema range before reading any record field. The caller
 * supplies a readable record of recordSize bytes and fieldCount descriptors.
 * Nonempty schemas require non-null fields, labels and positive capacities.
 * Unterminated arrays and empty required fields return INVALID_ARGUMENT.
 * Empty schemas are valid for a non-null, nonempty record. No allocation,
 * mutation, UTF-8 decoding, path access or domain-specific validation occurs.
 * outValidation is optional, otherwise it is written on every return and must
 * not overlap the record or descriptors. Keep inputs stable during the call. */
UmiStatus UmiSnapshotValidateTextFields(const void *record, size_t recordSize,
    const UmiSnapshotTextField *fields, size_t fieldCount,
    UmiSnapshotValidation *outValidation);

/** Return a static, readable explanation, including for an unknown issue. */
const char *UmiSnapshotIssueText(UmiSnapshotIssue issue);

/** Result of an atomic in-memory registry batch. applied is the number of
 * accepted input rows on success and zero on every failure. rejected_index
 * identifies the first rejected row, or SIZE_MAX for an operation-wide error
 * or success. validation describes bounded-text or duplicate-ID failures.
 * Typed batch APIs initialise this optional result on every return. */
typedef struct UmiSnapshotBatchResult {
    size_t applied;
    size_t rejected_index;
    UmiSnapshotValidation validation;
} UmiSnapshotBatchResult;

/** A capture describes one complete registry observation. Allocate room for
 * count records and retain revision when preparing an edit away from the owner
 * thread. Before publishing that edit, compare against this same registry's
 * revision; a number from another registry is not an authority to replace it. */
typedef struct UmiSnapshotCapture {
    size_t count;
    uint64_t revision;
} UmiSnapshotCapture;

/** An edit either inserts/replaces a complete record or removes its ID.
 * Removal reads only the fixed id array. No edit is an instruction to perform
 * external I/O; services decide separately how accepted state is persisted. */
typedef enum UmiSnapshotEditKind {
    UMI_SNAPSHOT_EDIT_UPSERT = 1,
    UMI_SNAPSHOT_EDIT_REMOVE = 2
} UmiSnapshotEditKind;

/** One bounded page from a registry observed at an exact revision. offset
 * is the requested starting row; copied records occupy the caller's output
 * array. next_offset is offset + copied, and has_more is true when further rows
 * remain. A zero-capacity read returns metadata without advancing the offset.
 * This is an in-memory observation, not a persistence format or a lock. */
typedef struct UmiSnapshotPage {
    uint64_t revision;
    size_t total_count;
    size_t offset;
    size_t copied;
    size_t next_offset;
    int has_more;
} UmiSnapshotPage;

#ifdef __cplusplus
}
#endif
#endif
