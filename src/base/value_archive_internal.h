/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/value_archive_internal.h
 * PURPOSE: Share byte-order, bounds and publication rules across typed state codecs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VALUE_ARCHIVE_INTERNAL_H
#define UMICOM_VALUE_ARCHIVE_INTERNAL_H
#include "umicom/base/value_archive.h"
#include <stdbool.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

/* A sticky status stops all later work after the first refused field. A null
 * writer buffer measures exactly the same representation without storing it. */
typedef struct UmiArchiveWriter {
    unsigned char *bytes;
    size_t capacity;
    size_t offset;
    UmiStatus status;
} UmiArchiveWriter;
typedef struct UmiArchiveReader {
    const unsigned char *bytes;
    size_t size;
    size_t offset;
    UmiStatus status;
} UmiArchiveReader;

static inline void UmiArchiveStoreInteger(unsigned char *bytes, uint64_t value, size_t width)
{
    for (size_t index = 0U; index < width; ++index) {
        bytes[index] = (unsigned char)(value & UINT64_C(255));
        value >>= 8U;
    }
}
static inline uint64_t UmiArchiveLoadInteger(const unsigned char *bytes, size_t width)
{
    uint64_t value = 0U;
    for (size_t index = 0U; index < width; ++index)
        value |= (uint64_t)bytes[index] << (index * 8U);
    return value;
}

/* Include the envelope in the checksum, treating its own four bytes as zero.
 * This detects a damaged length or schema as well as damaged field contents.
 * It is intentionally not a security boundary; typed validation is separate. */
static inline uint32_t UmiArchiveChecksum(const unsigned char *bytes, size_t size)
{
    uint32_t crc = UINT32_MAX;
    for (size_t index = 0U; index < size; ++index) {
        unsigned char byte = index >= 24U && index < 28U ? 0U : bytes[index];
        crc ^= (uint32_t)byte;
        for (unsigned bit = 0U; bit < 8U; ++bit)
            crc = (crc >> 1U) ^ ((crc & 1U) != 0U ? UINT32_C(0xedb88320) : 0U);
    }
    return ~crc;
}

static inline void UmiArchiveFinish(unsigned char *bytes, size_t size, uint64_t schema)
{
    memcpy(bytes, "UMIVALUE", 8U);
    UmiArchiveStoreInteger(bytes + 8U, schema, 8U);
    UmiArchiveStoreInteger(bytes + 16U, (uint64_t)(size - UMI_VALUE_ARCHIVE_HEADER_SIZE), 8U);
    memset(bytes + 24U, 0, 8U);
    UmiArchiveStoreInteger(bytes + 24U, UmiArchiveChecksum(bytes, size), 4U);
}

static inline void UmiArchiveWriteBytes(UmiArchiveWriter *writer, const void *bytes, size_t size)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (writer->offset > writer->capacity || size > writer->capacity - writer->offset) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    if (writer->bytes != NULL && size != 0U)
        memcpy(writer->bytes + writer->offset, bytes, size);
    writer->offset += size;
}
static inline void UmiArchiveWriteUnsigned(UmiArchiveWriter *writer, uint64_t value)
{
    unsigned char bytes[8];
    UmiArchiveStoreInteger(bytes, value, sizeof(bytes));
    UmiArchiveWriteBytes(writer, bytes, sizeof(bytes));
}
static inline void UmiArchiveWriteSigned(UmiArchiveWriter *writer, int64_t value)
{
    /* Conversion to unsigned defines the two's-complement wire bits even on
     * a host whose signed integer object representation differs. */
    UmiArchiveWriteUnsigned(writer, (uint64_t)value);
}
static inline void UmiArchiveWriteText(UmiArchiveWriter *writer, const char *text, size_t capacity)
{
    if (writer->status != UMI_STATUS_OK) return;
    const char *end = (const char *)memchr(text, '\0', capacity);
    if (end == NULL) { writer->status = UMI_STATUS_INVALID_ARGUMENT; return; }
    size_t length = (size_t)(end - text);
    UmiArchiveWriteUnsigned(writer, (uint64_t)length);
    UmiArchiveWriteBytes(writer, text, length);
}
static inline int UmiArchiveBinaryDouble(void)
{
    return CHAR_BIT == 8 && sizeof(double) == sizeof(uint64_t) &&
        DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024 && FLT_RADIX == 2;
}
static inline void UmiArchiveWriteDouble(UmiArchiveWriter *writer, double value)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (!UmiArchiveBinaryDouble()) { writer->status = UMI_STATUS_UNAVAILABLE; return; }
    if (!isfinite(value)) { writer->status = UMI_STATUS_INVALID_ARGUMENT; return; }
    uint64_t bits = 0U;
    memcpy(&bits, &value, sizeof(bits));
    UmiArchiveWriteUnsigned(writer, bits);
}

static inline const unsigned char *UmiArchiveReadBytes(UmiArchiveReader *reader, size_t size)
{
    if (reader->status != UMI_STATUS_OK) return NULL;
    if (reader->offset > reader->size || size > reader->size - reader->offset) {
        reader->status = UMI_STATUS_PARSE_ERROR;
        return NULL;
    }
    const unsigned char *bytes = reader->bytes + reader->offset;
    reader->offset += size;
    return bytes;
}
static inline uint64_t UmiArchiveReadUnsigned(UmiArchiveReader *reader, uint64_t maximum)
{
    const unsigned char *bytes = UmiArchiveReadBytes(reader, 8U);
    if (bytes == NULL) return 0U;
    uint64_t value = UmiArchiveLoadInteger(bytes, 8U);
    if (value > maximum) { reader->status = UMI_STATUS_PARSE_ERROR; return 0U; }
    return value;
}
static inline int64_t UmiArchiveReadSigned(UmiArchiveReader *reader, int64_t minimum, int64_t maximum)
{
    uint64_t bits = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    int64_t value = bits <= INT64_MAX ? (int64_t)bits : -1 - (int64_t)(UINT64_MAX - bits);
    if (value < minimum || value > maximum) { reader->status = UMI_STATUS_PARSE_ERROR; return 0; }
    return value;
}
static inline void UmiArchiveReadText(UmiArchiveReader *reader, char *text, size_t capacity)
{
    uint64_t count = UmiArchiveReadUnsigned(reader, (uint64_t)(capacity - 1U));
    if (reader->status != UMI_STATUS_OK) return;
    size_t length = (size_t)count;
    const unsigned char *bytes = UmiArchiveReadBytes(reader, length);
    if (bytes == NULL) return;
    if (memchr(bytes, '\0', length) != NULL) { reader->status = UMI_STATUS_PARSE_ERROR; return; }
    memset(text, 0, capacity);
    memcpy(text, bytes, length);
}
static inline double UmiArchiveReadDouble(UmiArchiveReader *reader)
{
    if (reader->status != UMI_STATUS_OK) return 0.0;
    if (!UmiArchiveBinaryDouble()) { reader->status = UMI_STATUS_UNAVAILABLE; return 0.0; }
    uint64_t bits = UmiArchiveReadUnsigned(reader, UINT64_MAX);
    double value = 0.0;
    memcpy(&value, &bits, sizeof(bits));
    if (!isfinite(value)) { reader->status = UMI_STATUS_PARSE_ERROR; return 0.0; }
    return value;
}

/* Domain bindings enumerate actual fields. Never memcpy an entire struct to
 * disk: padding, unused text and host integer layout are not a file format.
 * Preflight completes before encode touches the caller's buffer. Decode only
 * publishes the private candidate after every byte and domain rule passes. */
#define UMI_DEFINE_VALUE_ARCHIVE(Encode, Decode, Record, Schema, Bound, Write, Read, Validate) \
UmiStatus Encode(const Record *value, void *bytes, size_t capacity, size_t *out_size) \
{ \
    if (value == NULL || out_size == NULL || (bytes == NULL && capacity != 0U)) \
        return UMI_STATUS_INVALID_ARGUMENT; \
    if (CHAR_BIT != 8) return UMI_STATUS_UNAVAILABLE; \
    UmiStatus status = Validate(value); \
    if (status != UMI_STATUS_OK) return status; \
    UmiArchiveWriter measure = {NULL, UMI_VALUE_ARCHIVE_BYTE_LIMIT, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK}; \
    Write(&measure, value); \
    if (measure.status != UMI_STATUS_OK) return measure.status; \
    *out_size = measure.offset; \
    if (bytes == NULL) return UMI_STATUS_OK; \
    if (capacity < measure.offset) return UMI_STATUS_CAPACITY_EXCEEDED; \
    UmiArchiveWriter writer = {(unsigned char *)bytes, capacity, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK}; \
    Write(&writer, value); \
    if (writer.status != UMI_STATUS_OK) return writer.status; \
    UmiArchiveFinish(writer.bytes, writer.offset, Schema()); \
    return UMI_STATUS_OK; \
} \
UmiStatus Decode(const void *bytes, size_t byte_count, Record *value) \
{ \
    if (bytes == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    if (byte_count > Bound()) return UMI_STATUS_PARSE_ERROR; \
    UmiValueArchiveInfo info; \
    UmiStatus status = umi_value_archive_inspect(bytes, byte_count, &info); \
    if (status != UMI_STATUS_OK) return status; \
    if (info.schema != Schema()) return UMI_STATUS_UNAVAILABLE; \
    Record staged; \
    memset(&staged, 0, sizeof(staged)); \
    UmiArchiveReader reader = {(const unsigned char *)bytes, byte_count, UMI_VALUE_ARCHIVE_HEADER_SIZE, UMI_STATUS_OK}; \
    Read(&reader, &staged); \
    if (reader.status != UMI_STATUS_OK) return reader.status; \
    if (reader.offset != reader.size) return UMI_STATUS_PARSE_ERROR; \
    status = Validate(&staged); \
    if (status != UMI_STATUS_OK) return status; \
    *value = staged; \
    return UMI_STATUS_OK; \
}
#endif
