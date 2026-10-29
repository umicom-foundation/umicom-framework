/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_observation_archive.c
 *
 * PURPOSE:
 *   Encode and reopen bounded source baselines without treating history as current.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/source_observation_archive.h"
#include "source_observation_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SourceWriter { char *bytes; size_t length; UmiStatus status; } SourceWriter;
typedef struct SourceField { const char *bytes; size_t length; } SourceField;
typedef struct SourceReader { const char *next, *end; } SourceReader;

static bool DigestValid(const char *text)
{
    if (text == NULL) return false;
    for (size_t i = 0U; i < 64U; ++i)
        if (!((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'a' && text[i] <= 'f')))
            return false;
    return text[64] == '\0';
}
static void Append(SourceWriter *writer, const char *bytes, size_t length)
{
    if (writer->status != UMI_STATUS_OK) return;
    if (length > UMI_RELEASE_SOURCE_ARCHIVE_LIMIT - 1U - writer->length) {
        writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return;
    }
    if (writer->bytes != NULL) memcpy(writer->bytes + writer->length, bytes, length);
    writer->length += length;
}
static void Text(SourceWriter *writer, const char *text) { Append(writer, text, strlen(text)); }
static void Number(SourceWriter *writer, size_t value)
{
    char text[32];
    int count = snprintf(text, sizeof(text), "%zu", value);
    if (count < 0 || (size_t)count >= sizeof(text)) { writer->status = UMI_STATUS_CAPACITY_EXCEEDED; return; }
    Append(writer, text, (size_t)count);
}
static void Encode(const UmiReleaseSourceObservation *observation, SourceWriter *writer)
{
    static const char hex[] = "0123456789abcdef";
    Text(writer, "UMICOM-SOURCE-OBSERVATION\t1\ninventory\t");
    Text(writer, observation->inventoryDigest);
    Text(writer, "\nfingerprint\t"); Text(writer, observation->fingerprint);
    Text(writer, "\nfiles\t"); Number(writer, observation->count); Text(writer, "\n");
    for (size_t i = 0U; writer->status == UMI_STATUS_OK && i < observation->count; ++i) {
        const UmiReleaseSourceItem *item = &observation->items[i];
        Text(writer, "file\t");
        for (const unsigned char *p = (const unsigned char *)item->path; *p != 0U; ++p) {
            char encoded[] = {hex[*p >> 4U], hex[*p & 15U]};
            Append(writer, encoded, sizeof(encoded));
        }
        Text(writer, "\t"); Number(writer, item->bytes);
        Text(writer, "\t"); Text(writer, item->sha256); Text(writer, "\n");
    }
    Text(writer, "end\n");
}
UmiStatus UmiReleaseSourceObservationEncode(const UmiReleaseSourceObservation *observation,
    char **outBytes, size_t *outLength, char outSha256[65])
{
    if (observation == NULL || outBytes == NULL || *outBytes != NULL ||
        outLength == NULL || outSha256 == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Measure before allocation or publication. The immutable observation is
     * read twice; both passes share one encoder to keep its framing identical. */
    SourceWriter size = {0};
    Encode(observation, &size);
    if (size.status != UMI_STATUS_OK) return size.status;
    char *bytes = malloc(size.length + 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    SourceWriter writer = {.bytes = bytes};
    Encode(observation, &writer);
    UmiStatus status = writer.status;
    if (status == UMI_STATUS_OK && writer.length != size.length) status = UMI_STATUS_INVALID_STATE;
    char digest[65];
    if (status == UMI_STATUS_OK) {
        bytes[writer.length] = '\0';
        status = UmiSha256Buffer(bytes, writer.length, digest);
    }
    if (status == UMI_STATUS_OK) {
        *outBytes = bytes; *outLength = writer.length; memcpy(outSha256, digest, sizeof(digest));
    } else free(bytes);
    return status;
}
void UmiReleaseSourceObservationFreeEncoded(char *bytes) { free(bytes); }

static bool Is(SourceField field, const char *text)
{
    return field.length == strlen(text) && memcmp(field.bytes, text, field.length) == 0;
}
static UmiStatus Row(SourceReader *reader, const char *name, SourceField fields[4], size_t expected)
{
    const char *start = reader->next;
    size_t count = 0U;
    for (const char *cursor = start; cursor < reader->end; ++cursor) {
        if (*cursor != '\t' && *cursor != '\n') continue;
        if (count == 4U) return UMI_STATUS_PARSE_ERROR;
        fields[count++] = (SourceField){start, (size_t)(cursor - start)};
        if (*cursor == '\n') {
            reader->next = cursor + 1;
            return count == expected && Is(fields[0], name) ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
        }
        start = cursor + 1;
    }
    return UMI_STATUS_PARSE_ERROR;
}
static UmiStatus ReadNumber(SourceField field, size_t maximum, size_t *output)
{
    if (field.length == 0U || (field.length > 1U && field.bytes[0] == '0')) return UMI_STATUS_PARSE_ERROR;
    size_t value = 0U;
    for (size_t i = 0U; i < field.length; ++i) {
        unsigned char c = (unsigned char)field.bytes[i];
        if (c < '0' || c > '9') return UMI_STATUS_PARSE_ERROR;
        size_t digit = (size_t)(c - '0');
        if (digit > maximum || value > (maximum - digit) / 10U) return UMI_STATUS_PARSE_ERROR;
        value = value * 10U + digit;
    }
    *output = value; return UMI_STATUS_OK;
}
static UmiStatus ReadDigest(SourceField field, char output[65])
{
    if (field.length != 64U) return UMI_STATUS_PARSE_ERROR;
    char digest[65];
    memcpy(digest, field.bytes, 64U); digest[64] = '\0';
    if (!DigestValid(digest)) return UMI_STATUS_PARSE_ERROR;
    memcpy(output, digest, sizeof(digest)); return UMI_STATUS_OK;
}
static bool SamePath(SourceField field, const char *path)
{
    static const char hex[] = "0123456789abcdef";
    size_t length = strlen(path);
    if (field.length != length * 2U) return false;
    for (size_t i = 0U; i < length; ++i) {
        unsigned char byte = (unsigned char)path[i];
        if (field.bytes[2U * i] != hex[byte >> 4U] || field.bytes[2U * i + 1U] != hex[byte & 15U])
            return false;
    }
    return true;
}
UmiStatus UmiReleaseSourceObservationDecode(const UmiReleaseInventory *inventory,
    const char *bytes, size_t length, const char *expectedSha256,
    UmiReleaseSourceObservation **outObservation)
{
    if (inventory == NULL || bytes == NULL || outObservation == NULL || *outObservation != NULL ||
        !DigestValid(expectedSha256)) return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(UmiReleaseInventoryProducer(inventory), "cmake") != 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= UMI_RELEASE_SOURCE_ARCHIVE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || memchr(bytes, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    char digest[65], inventoryDigest[65], recordedFingerprint[65];
    UmiStatus status = UmiSha256Buffer(bytes, length, digest);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(digest, expectedSha256) != 0) return UMI_STATUS_INVALID_STATE;
    status = UmiReleaseInventoryFingerprint(inventory, inventoryDigest);
    if (status != UMI_STATUS_OK) return status;
    SourceReader reader = {.next = bytes, .end = bytes + length};
    SourceField fields[4];
    status = Row(&reader, "UMICOM-SOURCE-OBSERVATION", fields, 2U);
    if (status != UMI_STATUS_OK || !Is(fields[1], "1")) return UMI_STATUS_PARSE_ERROR;
    status = Row(&reader, "inventory", fields, 2U);
    if (status == UMI_STATUS_OK) status = ReadDigest(fields[1], digest);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(digest, inventoryDigest) != 0) return UMI_STATUS_INVALID_STATE;
    status = Row(&reader, "fingerprint", fields, 2U);
    if (status == UMI_STATUS_OK) status = ReadDigest(fields[1], recordedFingerprint);
    if (status == UMI_STATUS_OK) status = Row(&reader, "files", fields, 2U);
    size_t count = 0U;
    if (status == UMI_STATUS_OK) status = ReadNumber(fields[1], UMI_RELEASE_INVENTORY_RECORD_LIMIT, &count);
    if (status != UMI_STATUS_OK) return status;

    UmiReleaseSourceObservation *result = calloc(1U, sizeof(*result));
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    const char *problem = NULL;
    /* Collect from independently supplied metadata, not from untrusted path
     * fields. This allocates canonical identities but performs no source I/O. */
    status = UmiSourceObservationCollect(inventory, result, &problem);
    (void)problem;
    if (status == UMI_STATUS_OK && count != result->count) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) memcpy(result->inventoryDigest, inventoryDigest, sizeof(inventoryDigest));
    size_t total = 0U;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < result->count; ++i) {
        status = Row(&reader, "file", fields, 4U);
        UmiReleaseSourceItem *item = &result->items[i];
        if (status == UMI_STATUS_OK && !SamePath(fields[1], item->path)) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) status = ReadNumber(fields[2], UMI_RELEASE_SOURCE_FILE_LIMIT, &item->bytes);
        if (status == UMI_STATUS_OK && item->bytes > UMI_RELEASE_SOURCE_TOTAL_LIMIT - total)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK) {
            total += item->bytes;
            status = ReadDigest(fields[3], item->sha256);
        }
    }
    if (status == UMI_STATUS_OK) status = Row(&reader, "end", fields, 1U);
    if (status == UMI_STATUS_OK && reader.next != reader.end) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiSourceObservationFingerprint(result);
    if (status == UMI_STATUS_OK && strcmp(result->fingerprint, recordedFingerprint) != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) *outObservation = result;
    else UmiReleaseSourceObservationDestroy(result);
    return status;
}
