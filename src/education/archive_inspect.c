/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/education/archive_inspect.c
 *
 * PURPOSE:
 *   Inspect an ordinary static library's ar member headers and portable name
 *   tables without extracting or executing untrusted object-file contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/education/archive_inspect.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define ARCHIVE_HEADER_BYTES 60U
#define ARCHIVE_MAX_SCANNED 100000U

typedef struct ArchiveReader {
    const unsigned char *bytes;
    FILE *file;
    uint64_t total;
} ArchiveReader;

/* Check subtraction before every seek/read, including a zero-byte request. */
static UmiStatus ArchiveRead(const ArchiveReader *reader, uint64_t offset,
                             void *destination, size_t count)
{
    if (reader == NULL || (destination == NULL && count != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (offset > reader->total || (uint64_t)count > reader->total - offset)
        return UMI_STATUS_PARSE_ERROR;
    if (count == 0U) return UMI_STATUS_OK;
    if (reader->file == NULL) {
        memcpy(destination, reader->bytes + (size_t)offset, count);
        return UMI_STATUS_OK;
    }
    if (offset > (uint64_t)LONG_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (fseek(reader->file, (long)offset, SEEK_SET) != 0)
        return UMI_STATUS_IO_ERROR;
    return fread(destination, 1U, count, reader->file) == count
           ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* ar's size and extended-name fields are decimal ASCII, never native integers.
 * Spaces may surround the number; sign characters and overflow are refused. */
static UmiStatus ArchiveDecimal(const unsigned char *field, size_t width,
                                uint64_t *outValue)
{
    size_t position = 0U;
    uint64_t result = 0U;
    int digits = 0;
    while (position < width && field[position] == ' ') ++position;
    while (position < width && field[position] >= '0' && field[position] <= '9') {
        unsigned digit = (unsigned)(field[position] - '0');
        if (result > (UINT64_MAX - (uint64_t)digit) / UINT64_C(10))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        result = result * UINT64_C(10) + (uint64_t)digit;
        ++position;
        digits = 1;
    }
    while (position < width && field[position] == ' ') ++position;
    if (!digits || position != width) return UMI_STATUS_PARSE_ERROR;
    *outValue = result;
    return UMI_STATUS_OK;
}

/* Names enter a console report. Reject control characters rather than allowing
 * an untrusted archive to write terminal escapes or simulated log entries. */
static UmiStatus ArchiveCopyName(const unsigned char *source, size_t length,
                                  char destination[UMI_EDUCATION_ARCHIVE_NAME_CAPACITY])
{
    if (length == 0U || length >= UMI_EDUCATION_ARCHIVE_NAME_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < length; ++i) {
        if (source[i] < 0x20U || source[i] == 0x7fU)
            return UMI_STATUS_PARSE_ERROR;
        destination[i] = (char)source[i];
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static int ArchiveObjectSuffix(const char *name)
{
    size_t n = strlen(name);
    if (n >= 2U && name[n - 2U] == '.' && (name[n - 1U] == 'o' || name[n - 1U] == 'O'))
        return 1;
    if (n >= 4U && name[n - 4U] == '.' &&
        (name[n - 3U] == 'o' || name[n - 3U] == 'O') &&
        (name[n - 2U] == 'b' || name[n - 2U] == 'B') &&
        (name[n - 1U] == 'j' || name[n - 1U] == 'J')) return 1;
    return 0;
}

/* A GNU archive stores long names in the // member; /number points to an
 * offset into that string table. Its usual terminator is slash + newline. */
static UmiStatus ArchiveGnuName(const ArchiveReader *reader, uint64_t table,
    uint64_t tableLength, uint64_t index,
    char outName[UMI_EDUCATION_ARCHIVE_NAME_CAPACITY])
{
    if (index >= tableLength) return UMI_STATUS_PARSE_ERROR;
    size_t used = 0U;
    for (uint64_t i = index; i < tableLength; ++i) {
        unsigned char value = 0U;
        UmiStatus status = ArchiveRead(reader, table + i, &value, 1U);
        if (status != UMI_STATUS_OK) return status;
        if (value == '\n') {
            if (used == 0U) return UMI_STATUS_PARSE_ERROR;
            outName[used] = '\0';
            return UMI_STATUS_OK;
        }
        if (value == '/') {
            unsigned char next = 0U;
            if (i + 1U < tableLength &&
                ArchiveRead(reader, table + i + 1U, &next, 1U) == UMI_STATUS_OK &&
                next == '\n') {
                if (used == 0U) return UMI_STATUS_PARSE_ERROR;
                outName[used] = '\0';
                return UMI_STATUS_OK;
            }
        }
        if (value < 0x20U || value == 0x7fU) return UMI_STATUS_PARSE_ERROR;
        if (used + 1U >= UMI_EDUCATION_ARCHIVE_NAME_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        outName[used++] = (char)value;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static UmiStatus ArchiveParse(const ArchiveReader *reader, UmiEducationArchiveReport *outReport)
{
    static const unsigned char MAGIC[8] = {'!', '<', 'a', 'r', 'c', 'h', '>', '\n'};
    static const unsigned char THIN[8] = {'!', '<', 't', 'h', 'i', 'n', '>', '\n'};
    unsigned char magic[8];
    UmiStatus status = ArchiveRead(reader, 0U, magic, sizeof(magic));
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(magic, THIN, sizeof(magic)) == 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (memcmp(magic, MAGIC, sizeof(magic)) != 0)
        return UMI_STATUS_PARSE_ERROR;

    UmiEducationArchiveReport result = {0};
    result.archiveBytes = reader->total;
    uint64_t offset = sizeof(magic), gnuTable = 0U, gnuLength = 0U;
    uint32_t scanned = 0U;
    while (offset < reader->total) {
        if (scanned++ >= ARCHIVE_MAX_SCANNED) return UMI_STATUS_CAPACITY_EXCEEDED;
        unsigned char header[ARCHIVE_HEADER_BYTES];
        status = ArchiveRead(reader, offset, header, sizeof(header));
        if (status != UMI_STATUS_OK) return status;
        if (header[58U] != '`' || header[59U] != '\n') return UMI_STATUS_PARSE_ERROR;
        uint64_t size = 0U;
        status = ArchiveDecimal(header + 48U, 10U, &size);
        if (status != UMI_STATUS_OK) return status;
        const uint64_t body = offset + ARCHIVE_HEADER_BYTES;
        if (size > reader->total - body) return UMI_STATUS_PARSE_ERROR;
        uint64_t next = body + size;
        if ((size & UINT64_C(1)) != 0U) {
            unsigned char pad = 0U;
            status = ArchiveRead(reader, next, &pad, 1U);
            if (status != UMI_STATUS_OK) return status;
            if (pad != '\n' && pad != '\0') return UMI_STATUS_PARSE_ERROR;
            ++next;
        }
        char name[UMI_EDUCATION_ARCHIVE_NAME_CAPACITY] = {0};
        unsigned char raw[17] = {0};
        memcpy(raw, header, 16U);
        size_t len = 16U;
        while (len > 0U && raw[len - 1U] == ' ') --len;
        if (len == 0U) return UMI_STATUS_PARSE_ERROR;
        raw[len] = 0U;
        int metadata = 0;
        uint64_t payloadSize = size;
        if (strcmp((const char *)raw, "//") == 0) {
            gnuTable = body;
            gnuLength = size;
            metadata = 1;
        } else if (strcmp((const char *)raw, "/") == 0 ||
                   strcmp((const char *)raw, "/SYM64/") == 0 ||
                   strncmp((const char *)raw, "__.SYMDEF", 9U) == 0) {
            metadata = 1;
        } else if (len > 3U && memcmp(raw, "#1/", 3U) == 0) {
            uint64_t extendedLength = 0U;
            status = ArchiveDecimal(raw + 3U, len - 3U, &extendedLength);
            if (status != UMI_STATUS_OK) return status;
            if (extendedLength > payloadSize ||
                extendedLength == 0U ||
                extendedLength >= UMI_EDUCATION_ARCHIVE_NAME_CAPACITY)
                return UMI_STATUS_PARSE_ERROR;
            unsigned char embedded[UMI_EDUCATION_ARCHIVE_NAME_CAPACITY];
            status = ArchiveRead(reader, body, embedded, (size_t)extendedLength);
            if (status != UMI_STATUS_OK) return status;
            /* BSD ar may pad its embedded name with NUL bytes to a boundary.
             * NUL is padding here, not a printable part of a member name.
             * Do not count that padding as object data. */
            size_t actualLength = (size_t)extendedLength;
            while (actualLength > 0U && embedded[actualLength - 1U] == 0U)
                --actualLength;
            status = ArchiveCopyName(embedded, actualLength, name);
            if (status != UMI_STATUS_OK) return status;
            if (strncmp(name, "__.SYMDEF", 9U) == 0) metadata = 1;
            payloadSize -= extendedLength;
        } else if (len > 1U && raw[0] == '/') {
            uint64_t tableIndex = 0U;
            status = ArchiveDecimal(raw + 1U, len - 1U, &tableIndex);
            if (status != UMI_STATUS_OK) return status;
            status = ArchiveGnuName(reader, gnuTable, gnuLength, tableIndex, name);
            if (status != UMI_STATUS_OK) return status;
        } else {
            /* Ordinary SysV/GNU ar names end with one slash. */
            if (raw[len - 1U] == '/') --len;
            status = ArchiveCopyName(raw, len, name);
            if (status != UMI_STATUS_OK) return status;
        }
        if (metadata) {
            if (result.metadataCount == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
            ++result.metadataCount;
        } else {
            if (result.memberCount == UINT32_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
            ++result.memberCount;
            int object = ArchiveObjectSuffix(name);
            if (object) ++result.objectCount;
            if (result.recordedCount < UMI_EDUCATION_ARCHIVE_RECORDED_MEMBERS) {
                UmiEducationArchiveMember *record = &result.members[result.recordedCount++];
                memcpy(record->name, name, strlen(name) + 1U);
                record->headerOffset = offset;
                record->bodyBytes = payloadSize;
                record->objectFile = object;
            }
        }
        offset = next;
    }
    *outReport = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiEducationArchiveInspectBytes(const unsigned char *bytes,
    size_t byteCount, UmiEducationArchiveReport *outReport)
{
    if (bytes == NULL || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ArchiveReader reader = {bytes, NULL, (uint64_t)byteCount};
    return ArchiveParse(&reader, outReport);
}

UmiStatus UmiEducationArchiveInspectFile(const char *path,
    UmiEducationArchiveReport *outReport)
{
    if (path == NULL || *path == '\0' || outReport == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    UmiStatus status = UMI_STATUS_IO_ERROR;
    UmiEducationArchiveReport candidate;
    if (fseek(file, 0L, SEEK_END) == 0) {
        long length = ftell(file);
        if (length >= 0L) {
            ArchiveReader reader = {NULL, file, (uint64_t)length};
            status = ArchiveParse(&reader, &candidate);
        }
    }
    if (fclose(file) != 0 && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) *outReport = candidate;
    return status;
}
