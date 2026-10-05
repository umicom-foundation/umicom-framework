/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/registry_archive_internal.h
 * PURPOSE: Restore complete typed collections through their existing publication authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_REGISTRY_ARCHIVE_INTERNAL_H
#define UMICOM_REGISTRY_ARCHIVE_INTERNAL_H
#include "value_archive_internal.h"
#include "umicom/base/snapshot_validation.h"
#include <stdlib.h>

/* A collection has its own envelope and embeds complete typed value archives.
 * Row lengths allow refusal before decoding; all rows are staged before the
 * existing replacement operation checks identities, normalizes fields and
 * assigns fresh local revisions. Saved counters never become owner authority.
 * Keep access serialized: this is an atomic owner publication, not a lock. */
#define UMI_DEFINE_REGISTRY_ARCHIVE(Encode, Restore, Registry, Record, Capacity, Schema, EncodeValue, DecodeValue, Replace) \
UmiStatus Encode(const Registry *registry, uint64_t expected_revision, \
    void *bytes, size_t capacity, size_t *out_size) \
{ \
    if (registry == NULL || out_size == NULL || (bytes == NULL && capacity != 0U)) \
        return UMI_STATUS_INVALID_ARGUMENT; \
    if (registry->revision != expected_revision || registry->count > (size_t)(Capacity)) \
        return UMI_STATUS_INVALID_STATE; \
    if (CHAR_BIT != 8) return UMI_STATUS_UNAVAILABLE; \
    size_t size = UMI_VALUE_ARCHIVE_HEADER_SIZE + 8U; \
    for (size_t index = 0U; index < registry->count; ++index) { \
        size_t row_size = 0U; \
        UmiStatus status = EncodeValue(&registry->items[index], NULL, 0U, &row_size); \
        if (status != UMI_STATUS_OK) return status; \
        if (size > UMI_VALUE_ARCHIVE_BYTE_LIMIT - 8U || \
            row_size > UMI_VALUE_ARCHIVE_BYTE_LIMIT - 8U - size) \
            return UMI_STATUS_CAPACITY_EXCEEDED; \
        size += 8U + row_size; \
    } \
    *out_size = size; \
    if (bytes == NULL) return UMI_STATUS_OK; \
    if (capacity < size) return UMI_STATUS_CAPACITY_EXCEEDED; \
    UmiArchiveWriter writer = {(unsigned char *)bytes, size, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK}; \
    UmiArchiveWriteUnsigned(&writer, (uint64_t)registry->count); \
    for (size_t index = 0U; index < registry->count; ++index) { \
        size_t row_size = 0U; \
        UmiStatus status = EncodeValue(&registry->items[index], NULL, 0U, &row_size); \
        if (status != UMI_STATUS_OK) return status; \
        UmiArchiveWriteUnsigned(&writer, (uint64_t)row_size); \
        status = EncodeValue(&registry->items[index], writer.bytes + writer.offset, \
            writer.capacity - writer.offset, &row_size); \
        if (status != UMI_STATUS_OK) return status; \
        writer.offset += row_size; \
    } \
    UmiArchiveFinish(writer.bytes, writer.offset, Schema() ^ UINT64_C(0xa48e739bb1620dc5)); \
    return UMI_STATUS_OK; \
} \
UmiStatus Restore(Registry *registry, uint64_t expected_revision, \
    const void *bytes, size_t byte_count, UmiSnapshotBatchResult *out_result) \
{ \
    UmiSnapshotBatchResult result = {0U, SIZE_MAX, {UMI_SNAPSHOT_VALID, NULL, SIZE_MAX, 0U}}; \
    if (out_result != NULL) *out_result = result; \
    if (registry == NULL || bytes == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (registry->revision != expected_revision || registry->count > (size_t)(Capacity)) \
        return UMI_STATUS_INVALID_STATE; \
    UmiValueArchiveInfo info; \
    UmiStatus status = umi_value_archive_inspect(bytes, byte_count, &info); \
    if (status != UMI_STATUS_OK) return status; \
    if (info.schema != (Schema() ^ UINT64_C(0xa48e739bb1620dc5))) return UMI_STATUS_UNAVAILABLE; \
    UmiArchiveReader reader = {(const unsigned char *)bytes, byte_count, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK}; \
    size_t count = (size_t)UmiArchiveReadUnsigned(&reader, (uint64_t)(Capacity)); \
    if (reader.status != UMI_STATUS_OK) return reader.status; \
    if (count > (reader.size - reader.offset) / (8U + UMI_VALUE_ARCHIVE_HEADER_SIZE)) \
        return UMI_STATUS_PARSE_ERROR; \
    if (count > UMI_VALUE_ARCHIVE_BYTE_LIMIT / sizeof(Record)) return UMI_STATUS_CAPACITY_EXCEEDED; \
    Record *items = count != 0U ? (Record *)malloc(count * sizeof(*items)) : NULL; \
    if (count != 0U && items == NULL) return UMI_STATUS_OUT_OF_MEMORY; \
    for (size_t index = 0U; index < count; ++index) { \
        size_t row_size = (size_t)UmiArchiveReadUnsigned(&reader, (uint64_t)byte_count); \
        const unsigned char *row = UmiArchiveReadBytes(&reader, row_size); \
        status = reader.status; \
        if (status == UMI_STATUS_OK) status = DecodeValue(row, row_size, &items[index]); \
        if (status != UMI_STATUS_OK) { \
            result.rejected_index = index; \
            if (out_result != NULL) *out_result = result; \
            free(items); \
            return status; \
        } \
    } \
    if (reader.offset != reader.size) { free(items); return UMI_STATUS_PARSE_ERROR; } \
    status = Replace(registry, expected_revision, items, count, out_result); \
    free(items); \
    return status; \
}
#endif
