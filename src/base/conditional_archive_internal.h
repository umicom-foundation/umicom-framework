/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/conditional_archive_internal.h
 * PURPOSE: Guard typed archive restoration with the previously reviewed encoded state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONDITIONAL_ARCHIVE_INTERNAL_H
#define UMICOM_CONDITIONAL_ARCHIVE_INTERNAL_H
#include "value_archive_internal.h"
#include <stdlib.h>

/* Bind only an existing, validated inline-value codec. Compare the complete
 * canonical encodings, never a hash alone or struct padding. This is a
 * caller-serialized comparison and replacement, not a cross-thread primitive.
 * A snapshot that changes away and back cannot be distinguished from the same
 * encoded state; owners needing history tokens must also use their revision API.
 *
 * Keep decoding on the heap here: an imported record may contain large bounded
 * arrays. The existing decoder retains its original allocation contract. */
#define UMI_DEFINE_CONDITIONAL_ARCHIVE(Function, Encode, Record, Schema, Bound, Read, Validate) \
UmiStatus Function(const void *bytes, size_t byte_count, \
    const Record *expected, Record *value) \
{ \
    if (bytes == NULL || expected == NULL || value == NULL) \
        return UMI_STATUS_INVALID_ARGUMENT; \
    if (byte_count > Bound()) return UMI_STATUS_PARSE_ERROR; \
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
    if (current_size > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED; \
    unsigned char *comparison = malloc(current_size * 2U); \
    if (comparison == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    size_t written = 0U; \
    status = Encode(expected, comparison, current_size, &written); \
    if (status == UMI_STATUS_OK) \
        status = Encode(value, comparison + current_size, current_size, &written); \
    if (status == UMI_STATUS_OK && \
        memcmp(comparison, comparison + current_size, current_size) != 0) \
        status = UMI_STATUS_INVALID_STATE; \
    free(comparison); \
    if (status != UMI_STATUS_OK) return status; \
    Record *candidate = malloc(sizeof(*candidate)); \
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    memset(candidate, 0, sizeof(*candidate)); \
    UmiArchiveReader reader = {(const unsigned char *)bytes, byte_count, \
        UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK}; \
    Read(&reader, candidate); \
    status = reader.status; \
    if (status == UMI_STATUS_OK && reader.offset != reader.size) \
        status = UMI_STATUS_PARSE_ERROR; \
    if (status == UMI_STATUS_OK) status = Validate(candidate); \
    /* No live byte changes until both the comparison and complete decode pass. */ \
    if (status == UMI_STATUS_OK) memcpy(value, candidate, sizeof(*value)); \
    free(candidate); \
    return status; \
}
#endif
