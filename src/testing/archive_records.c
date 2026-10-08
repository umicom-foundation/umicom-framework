/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/archive_records.c
 * PURPOSE: Fit bounded diagnostic records into existing Data Server value limits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "archive_internal.h"
#include <stdio.h>
#include <string.h>
#define ARCHIVE_CHUNK_BYTES 4000U
#define ARCHIVE_CHUNK_LIMIT                                                                                  \
    ((UMI_TEST_ARCHIVE_WIRE_CAPACITY + ARCHIVE_CHUNK_BYTES - 1U) / ARCHIVE_CHUNK_BYTES)

/* These private helpers run within the archive's transaction. The parent key
 * records byte length and chunk count. Children are fixed-size except the last;
 * a missing, oversized or short chunk is corruption, never truncated evidence.
 * This representation leaves Data Server's 4 KiB value limit unchanged. */
static bool decimal(const char **cursor, size_t *out)
{
    const char *p = *cursor;
    size_t value = 0;
    if (*p < '0' || *p > '9')
        return false;
    while (*p >= '0' && *p <= '9')
    {
        unsigned digit = (unsigned)(*p - '0');
        if (value > (SIZE_MAX - digit) / 10U)
            return false;
        value = value * 10U + digit;
        ++p;
    }
    if (*p != '|')
        return false;
    *cursor = p + 1;
    *out = value;
    return true;
}
static UmiStatus shape(UmiDataServer *server, const char *key, size_t *length, size_t *chunks)
{
    char header[96];
    size_t format = 0;
    UmiStatus status = umi_data_server_get(server, key, header, sizeof(header));
    if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_CAPACITY_EXCEEDED)
        return UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK)
        return status;
    const char *cursor = header;
    if (!decimal(&cursor, &format) || format != 1U || !decimal(&cursor, length) ||
        !decimal(&cursor, chunks) || *cursor != '\0' || *length == 0 ||
        *length >= UMI_TEST_ARCHIVE_WIRE_CAPACITY || *chunks == 0 || *chunks > ARCHIVE_CHUNK_LIMIT ||
        *chunks != (*length + ARCHIVE_CHUNK_BYTES - 1U) / ARCHIVE_CHUNK_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
static void chunk_key(const char *key, size_t index, char out[192])
{
    (void)snprintf(out, 192U, "%s/part/%zu", key, index);
}
UmiStatus UmiTestArchiveValueWrite(UmiDataServer *server, const char *key, const char *wire)
{
    size_t length = strlen(wire);
    if (length == 0 || length >= UMI_TEST_ARCHIVE_WIRE_CAPACITY || strlen(key) >= 160U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t chunks = (length + ARCHIVE_CHUNK_BYTES - 1U) / ARCHIVE_CHUNK_BYTES;
    char child[192], part[ARCHIVE_CHUNK_BYTES + 1U];
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0; status == UMI_STATUS_OK && i < chunks; ++i)
    {
        size_t offset = i * ARCHIVE_CHUNK_BYTES;
        size_t count = length - offset;
        if (count > ARCHIVE_CHUNK_BYTES)
            count = ARCHIVE_CHUNK_BYTES;
        memcpy(part, wire + offset, count);
        part[count] = '\0';
        chunk_key(key, i, child);
        status = umi_data_server_set(server, child, part);
    }
    if (status == UMI_STATUS_OK)
    {
        char header[96];
        (void)snprintf(header, sizeof(header), "1|%zu|%zu|", length, chunks);
        status = umi_data_server_set(server, key, header);
    }
    return status;
}
UmiStatus UmiTestArchiveValueRead(UmiDataServer *server, const char *key, char *wire, size_t capacity)
{
    size_t length = 0, chunks = 0;
    UmiStatus status = shape(server, key, &length, &chunks);
    if (status != UMI_STATUS_OK)
        return status;
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char child[192], part[ARCHIVE_CHUNK_BYTES + 1U];
    for (size_t i = 0; i < chunks; ++i)
    {
        chunk_key(key, i, child);
        status = umi_data_server_get(server, child, part, sizeof(part));
        if (status == UMI_STATUS_NOT_FOUND || status == UMI_STATUS_CAPACITY_EXCEEDED)
            return UMI_STATUS_PARSE_ERROR;
        if (status != UMI_STATUS_OK)
            return status;
        size_t offset = i * ARCHIVE_CHUNK_BYTES;
        size_t expected = length - offset;
        if (expected > ARCHIVE_CHUNK_BYTES)
            expected = ARCHIVE_CHUNK_BYTES;
        if (strlen(part) != expected)
            return UMI_STATUS_PARSE_ERROR;
        memcpy(wire + offset, part, expected);
    }
    wire[length] = '\0';
    return UMI_STATUS_OK;
}
UmiStatus UmiTestArchiveValueDelete(UmiDataServer *server, const char *key)
{
    size_t length = 0, chunks = 0;
    UmiStatus status = shape(server, key, &length, &chunks);
    if (status != UMI_STATUS_OK)
        return status;
    char child[192];
    for (size_t i = 0; status == UMI_STATUS_OK && i < chunks; ++i)
    {
        chunk_key(key, i, child);
        status = umi_data_server_delete(server, child);
        if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
        status = umi_data_server_delete(server, key);
    return status;
}
