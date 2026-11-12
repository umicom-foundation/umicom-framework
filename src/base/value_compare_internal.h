/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/value_compare_internal.h
 *
 * PURPOSE:
 *   Bind existing domain codecs to allocation-free canonical comparison using caller-owned scratch storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_VALUE_COMPARE_INTERNAL_H
#define UMICOM_VALUE_COMPARE_INTERNAL_H
#include "storage_internal.h"
#include "umicom/base/value_archive.h"
#include <string.h>

/* Every binding uses its existing checked codec. Measurement validates both
 * values before either is encoded; no separate field layout or hash-only
 * equality can drift from persistence. Keep all resource ownership with the
 * caller so native preview loops can reuse one bounded allocation. */
#define UMI_DEFINE_VALUE_COMPARE(Function, Record, Encode) \
UmiStatus Function(const Record *left, const Record *right, \
    void *workspace, size_t capacity, UmiValueArchiveComparison *out_comparison) \
{ \
    if (left == NULL || right == NULL || out_comparison == NULL || \
        (workspace == NULL && capacity != 0U)) return UMI_STATUS_INVALID_ARGUMENT; \
    if (UmiStorageOverlaps(left, sizeof(*left), out_comparison, sizeof(*out_comparison)) || \
        UmiStorageOverlaps(right, sizeof(*right), out_comparison, sizeof(*out_comparison)) || \
        UmiStorageOverlaps(left, sizeof(*left), workspace, capacity) || \
        UmiStorageOverlaps(right, sizeof(*right), workspace, capacity) || \
        UmiStorageOverlaps(workspace, capacity, out_comparison, sizeof(*out_comparison))) \
        return UMI_STATUS_INVALID_ARGUMENT; \
    UmiValueArchiveComparison measured = {0}; \
    UmiStatus status = Encode(left, NULL, 0U, &measured.left_archive_size); \
    if (status != UMI_STATUS_OK) return status; \
    status = Encode(right, NULL, 0U, &measured.right_archive_size); \
    if (status != UMI_STATUS_OK) return status; \
    if (measured.left_archive_size < UMI_VALUE_ARCHIVE_HEADER_SIZE || \
        measured.right_archive_size < UMI_VALUE_ARCHIVE_HEADER_SIZE || \
        measured.left_archive_size > UMI_VALUE_ARCHIVE_BYTE_LIMIT || \
        measured.right_archive_size > UMI_VALUE_ARCHIVE_BYTE_LIMIT) \
        return UMI_STATUS_INVALID_STATE; \
    if (measured.right_archive_size > SIZE_MAX - measured.left_archive_size) \
        return UMI_STATUS_CAPACITY_EXCEEDED; \
    measured.required_workspace = measured.left_archive_size + measured.right_archive_size; \
    measured.relation = UMI_VALUE_ARCHIVE_NOT_COMPARED; \
    if (workspace == NULL) { *out_comparison = measured; return UMI_STATUS_OK; } \
    if (capacity < measured.required_workspace) { \
        *out_comparison = measured; return UMI_STATUS_CAPACITY_EXCEEDED; \
    } \
    unsigned char *bytes = workspace; \
    size_t written = 0U; \
    status = Encode(left, bytes, measured.left_archive_size, &written); \
    if (status != UMI_STATUS_OK) return status; \
    if (written != measured.left_archive_size) return UMI_STATUS_INVALID_STATE; \
    status = Encode(right, bytes + measured.left_archive_size, \
        measured.right_archive_size, &written); \
    if (status != UMI_STATUS_OK) return status; \
    if (written != measured.right_archive_size) return UMI_STATUS_INVALID_STATE; \
    measured.relation = measured.left_archive_size == measured.right_archive_size && \
        memcmp(bytes, bytes + measured.left_archive_size, measured.left_archive_size) == 0 \
        ? UMI_VALUE_ARCHIVE_EQUAL : UMI_VALUE_ARCHIVE_DIFFERENT; \
    *out_comparison = measured; \
    return UMI_STATUS_OK; \
}
#endif
