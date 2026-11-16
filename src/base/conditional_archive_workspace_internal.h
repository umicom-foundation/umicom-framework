/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/conditional_archive_workspace_internal.h
 *
 * PURPOSE:
 *   Restore reviewed inline values using caller-owned comparison and candidate storage, without hidden allocations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONDITIONAL_ARCHIVE_WORKSPACE_INTERNAL_H
#define UMICOM_CONDITIONAL_ARCHIVE_WORKSPACE_INTERNAL_H
#include "value_archive_internal.h"

/* All writable scratch spans must be independent of live/borrowed input.
 * Expected and destination may be exactly the same object, but a partially
 * overlapping pair is not a snapshot. Address-distance checks never form an
 * overflowing end pointer. Callers still own accessibility and alignment. */
static inline bool UmiConditionalArchiveStorageConflict(
    const void *bytes, size_t byte_count, const void *expected, void *value,
    size_t record_size, void *workspace, void *comparison, size_t capacity)
{
    return (expected != value &&
            UmiArchiveStorageOverlaps(expected, record_size, value, record_size)) ||
        UmiArchiveStorageOverlaps(bytes, byte_count, expected, record_size) ||
        UmiArchiveStorageOverlaps(bytes, byte_count, value, record_size) ||
        UmiArchiveStorageOverlaps(workspace, record_size, expected, record_size) ||
        UmiArchiveStorageOverlaps(workspace, record_size, value, record_size) ||
        UmiArchiveStorageOverlaps(workspace, record_size, bytes, byte_count) ||
        UmiArchiveStorageOverlaps(comparison, capacity, workspace, record_size) ||
        UmiArchiveStorageOverlaps(comparison, capacity, expected, record_size) ||
        UmiArchiveStorageOverlaps(comparison, capacity, value, record_size) ||
        UmiArchiveStorageOverlaps(comparison, capacity, bytes, byte_count);
}

/* Bind existing domain encoders/readers; no second format or copied validator.
 * The caller can reserve/reuse one aligned Record and two encoded snapshots.
 * Comparison is byte-exact canonical state, not a hash, struct padding, lock
 * or ABA/history check. Only the established staged decoder publishes value. */
#define UMI_DEFINE_CONDITIONAL_ARCHIVE_WORKSPACE(Function, Encode, DecodeWorkspace, Record, Schema, Bound) \
UmiStatus Function(const void *bytes, size_t byte_count, const Record *expected, \
    Record *workspace, void *comparison, size_t comparison_capacity, Record *value) \
{ \
    if (bytes == NULL || expected == NULL || workspace == NULL || \
        comparison == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (byte_count > Bound()) return UMI_STATUS_PARSE_ERROR; \
    if (UmiConditionalArchiveStorageConflict(bytes, byte_count, expected, value, \
            sizeof(Record), workspace, comparison, comparison_capacity)) \
        return UMI_STATUS_INVALID_ARGUMENT; \
    UmiValueArchiveInfo info; \
    UmiStatus status = umi_value_archive_inspect(bytes, byte_count, &info); \
    if (status != UMI_STATUS_OK) return status; \
    if (info.schema != Schema()) return UMI_STATUS_UNAVAILABLE; \
    size_t expected_size = 0U, current_size = 0U; \
    status = Encode(expected, NULL, 0U, &expected_size); \
    if (status != UMI_STATUS_OK) return status; \
    status = Encode(value, NULL, 0U, &current_size); \
    if (status != UMI_STATUS_OK) return status; \
    if (expected_size != current_size) return UMI_STATUS_INVALID_STATE; \
    if (current_size > SIZE_MAX / 2U || comparison_capacity < current_size * 2U) \
        return UMI_STATUS_CAPACITY_EXCEEDED; \
    unsigned char *scratch = (unsigned char *)comparison; \
    size_t written = 0U; \
    status = Encode(expected, scratch, current_size, &written); \
    if (status == UMI_STATUS_OK && written != current_size) return UMI_STATUS_INVALID_STATE; \
    if (status != UMI_STATUS_OK) return status; \
    status = Encode(value, scratch + current_size, current_size, &written); \
    if (status == UMI_STATUS_OK && written != current_size) return UMI_STATUS_INVALID_STATE; \
    if (status != UMI_STATUS_OK) return status; \
    if (memcmp(scratch, scratch + current_size, current_size) != 0) \
        return UMI_STATUS_INVALID_STATE; \
    return DecodeWorkspace(bytes, byte_count, workspace, value); \
}
#endif
