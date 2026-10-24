/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_document.c
 * PURPOSE: Retain content and amendment metadata in one Data Server transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/record_document.h"
#include "umicom/base/sha256.h"
#include "include/record_fields.h"
#include "include/record_history_internal.h"
#include <stdio.h>

/* Bounded chunks keep binary content portable across the string-based memory
 * and SQLite backends. The service owns encoding; applications keep their own
 * record schema and never construct persistence keys or hexadecimal payloads. */
#define CONTENT_CHUNK_BYTES 1024U
#define CONTENT_CHUNK_TEXT_CAPACITY (CONTENT_CHUNK_BYTES * 2U + 1U)
#define CONTENT_MANIFEST_CAPACITY 64U

static UmiStatus content_key(const char *entity, uint64_t revision,
                             size_t chunk, char *output, size_t capacity)
{
    if (revision == 0U || chunk > UMI_RECORD_DOCUMENT_MAX_BYTES / CONTENT_CHUNK_BYTES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    char history[192];
    UmiStatus status = umi_record_history_key(entity, revision, history, sizeof history);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* History supplies the collision-free encoded entity and fixed-width
     * revision. Chunk zero is the manifest, with content in chunks 1 through N. */
    const size_t prefix = sizeof "repo.record-history.r." - 1U;
    int written = snprintf(output, capacity, "repo.record-content.%s.%02zu",
                           history + prefix, chunk);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus abort_document(UmiDataServer *server, UmiStatus original)
{
    return umi_data_server_rollback(server) == UMI_STATUS_OK
        ? original : UMI_STATUS_IO_ERROR;
}

static UmiStatus set_unoccupied(UmiDataServer *server, const char *key, const char *value)
{
    char occupied[2];
    UmiStatus status = umi_data_server_get(server, key, occupied, sizeof occupied);
    if (status != UMI_STATUS_NOT_FOUND) {
        return status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED
            ? UMI_STATUS_INVALID_STATE : status;
    }
    return umi_data_server_set(server, key, value);
}

static UmiStatus write_content(UmiDataServer *server, const UmiRecordDocument *document)
{
    char key[192], text[CONTENT_CHUNK_TEXT_CAPACITY];
    const char *entity = document->record.amendment.entity_id;
    uint64_t revision = document->record.revision;
    size_t chunks = (document->length + CONTENT_CHUNK_BYTES - 1U) / CONTENT_CHUNK_BYTES;
    UmiStatus status = content_key(entity, revision, 0U, key, sizeof key);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    int written = snprintf(text, sizeof text, "UMID1\t%zu\t%zu", document->length, chunks);
    if (written < 0 || (size_t)written >= sizeof text) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = set_unoccupied(server, key, text);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    static const char hex[] = "0123456789abcdef";
    for (size_t chunk = 0U; chunk < chunks; ++chunk) {
        size_t offset = chunk * CONTENT_CHUNK_BYTES;
        size_t bytes = document->length - offset;
        if (bytes > CONTENT_CHUNK_BYTES) {
            bytes = CONTENT_CHUNK_BYTES;
        }
        for (size_t i = 0U; i < bytes; ++i) {
            unsigned char byte = document->content[offset + i];
            text[i * 2U] = hex[byte >> 4U];
            text[i * 2U + 1U] = hex[byte & 15U];
        }
        text[bytes * 2U] = '\0';
        status = content_key(entity, revision, chunk + 1U, key, sizeof key);
        if (status == UMI_STATUS_OK) {
            status = set_unoccupied(server, key, text);
        }
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

static int hex_digit(char byte)
{
    if (byte >= '0' && byte <= '9') {
        return byte - '0';
    }
    if (byte >= 'a' && byte <= 'f') {
        return byte - 'a' + 10;
    }
    return -1;
}

/* The caller owns a transaction; no public read is composed here because a
 * nested transaction would incorrectly reject the document operation itself. */
static UmiStatus read_content(UmiDataServer *server, const char *entity,
                              uint64_t revision, UmiRecordDocument *candidate)
{
    UmiStatus status = umi_record_history_load(server, entity, revision, &candidate->record);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    char key[192], manifest[CONTENT_MANIFEST_CAPACITY];
    status = content_key(entity, revision, 0U, key, sizeof key);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    memset(manifest, 0xff, sizeof manifest);
    status = umi_data_server_get(server, key, manifest, sizeof manifest);
    if (status != UMI_STATUS_OK) {
        return status == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : status;
    }
    char *fields[3];
    uint64_t length, count;
    if (!umi_record_fields_split(manifest, sizeof manifest, fields, 3U) ||
        strcmp(fields[0], "UMID1") != 0 ||
        !umi_record_field_u64(fields[1], &length) ||
        !umi_record_field_u64(fields[2], &count) ||
        length > UMI_RECORD_DOCUMENT_MAX_BYTES ||
        count != (length + CONTENT_CHUNK_BYTES - 1U) / CONTENT_CHUNK_BYTES ||
        (fields[1][0] == '0' && fields[1][1] != '\0') ||
        (fields[2][0] == '0' && fields[2][1] != '\0')) {
        return UMI_STATUS_PARSE_ERROR;
    }
    candidate->length = (size_t)length;
    /* Zero unused bytes so snapshots never disclose uninitialised stack data. */
    memset(candidate->content, 0, sizeof candidate->content);
    for (size_t chunk = 0U; chunk < (size_t)count; ++chunk) {
        size_t offset = chunk * CONTENT_CHUNK_BYTES;
        size_t bytes = candidate->length - offset;
        if (bytes > CONTENT_CHUNK_BYTES) {
            bytes = CONTENT_CHUNK_BYTES;
        }
        char text[CONTENT_CHUNK_TEXT_CAPACITY];
        memset(text, 0xff, sizeof text);
        status = content_key(entity, revision, chunk + 1U, key, sizeof key);
        if (status == UMI_STATUS_OK) {
            status = umi_data_server_get(server, key, text, sizeof text);
        }
        if (status != UMI_STATUS_OK) {
            return status == UMI_STATUS_CAPACITY_EXCEEDED ? UMI_STATUS_PARSE_ERROR : status;
        }
        const char *end = memchr(text, '\0', sizeof text);
        if (end == NULL || (size_t)(end - text) != bytes * 2U) {
            return UMI_STATUS_PARSE_ERROR;
        }
        for (size_t i = 0U; i < bytes; ++i) {
            int high = hex_digit(text[i * 2U]), low = hex_digit(text[i * 2U + 1U]);
            if (high < 0 || low < 0) {
                return UMI_STATUS_PARSE_ERROR;
            }
            candidate->content[offset + i] = (unsigned char)((unsigned)high * 16U + (unsigned)low);
        }
    }
    char digest[UMI_SHA256_HEX_CAPACITY];
    status = UmiSha256Buffer(candidate->content, candidate->length, digest);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return strcmp(digest, candidate->record.amendment.content_sha256) == 0
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus umi_record_document_append(UmiDataServer *server,
                                    const UmiRecordAmendment *metadata,
                                    const void *content, size_t length,
                                    uint64_t expected_revision,
                                    UmiRecordRevision *output)
{
    if (server == NULL || metadata == NULL || output == NULL ||
        (content == NULL && length != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length > UMI_RECORD_DOCUMENT_MAX_BYTES || expected_revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiRecordDocument candidate = {0};
    candidate.record.amendment = *metadata;
    candidate.record.revision = expected_revision + 1U;
    candidate.record.previous_revision = expected_revision;
    candidate.length = length;
    if (length != 0U) {
        memcpy(candidate.content, content, length);
    }
    UmiStatus status = UmiSha256Buffer(candidate.content, length,
                                      candidate.record.amendment.content_sha256);
    char checked[UMI_RECORD_REVISION_TEXT_CAPACITY];
    if (status == UMI_STATUS_OK) {
        status = umi_record_revision_format(&candidate.record, checked, sizeof checked);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UmiRecordRevision staged;
    status = umi_record_history_stage(server, &candidate.record.amendment,
                                      expected_revision, &staged);
    if (status == UMI_STATUS_OK && expected_revision != 0U) {
        UmiRecordDocument previous;
        status = read_content(server, candidate.record.amendment.entity_id,
                              expected_revision, &previous);
    }
    if (status == UMI_STATUS_OK) {
        status = write_content(server, &candidate);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_data_server_commit(server);
    }
    if (status != UMI_STATUS_OK) {
        return abort_document(server, status);
    }
    *output = staged;
    return UMI_STATUS_OK;
}

static UmiStatus load_document(UmiDataServer *server, const char *entity_id,
                               uint64_t revision, bool latest,
                               UmiRecordDocument *output)
{
    if (server == NULL || output == NULL || entity_id == NULL || (!latest && revision == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UmiRecordDocument candidate;
    if (latest) {
        UmiRecordRevision head;
        status = umi_record_history_latest(server, entity_id, &head);
        if (status == UMI_STATUS_OK) {
            revision = head.revision;
        }
    }
    if (status == UMI_STATUS_OK) {
        status = read_content(server, entity_id, revision, &candidate);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_data_server_commit(server);
    }
    if (status != UMI_STATUS_OK) {
        return abort_document(server, status);
    }
    *output = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_document_load(UmiDataServer *server, const char *entity_id,
                                  uint64_t revision, UmiRecordDocument *output)
{
    return load_document(server, entity_id, revision, false, output);
}

UmiStatus umi_record_document_latest(UmiDataServer *server, const char *entity_id,
                                    UmiRecordDocument *output)
{
    return load_document(server, entity_id, 0U, true, output);
}

UmiStatus umi_record_document_verify(UmiDataServer *server, const char *entity_id,
                                    uint64_t *out_revisions)
{
    if (server == NULL || entity_id == NULL || out_revisions == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UmiRecordRevision head;
    status = umi_record_history_latest(server, entity_id, &head);
    if (status != UMI_STATUS_OK) {
        return abort_document(server, status);
    }
    uint64_t previous_time = 0U;
    for (uint64_t revision = 1U;; ++revision) {
        UmiRecordDocument candidate;
        status = read_content(server, entity_id, revision, &candidate);
        if (status != UMI_STATUS_OK) {
            return abort_document(server, status);
        }
        if (candidate.record.amendment.recorded_at_nanoseconds < previous_time) {
            return abort_document(server, UMI_STATUS_PARSE_ERROR);
        }
        previous_time = candidate.record.amendment.recorded_at_nanoseconds;
        if (revision == head.revision) {
            break;
        }
    }
    status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK) {
        return abort_document(server, status);
    }
    *out_revisions = head.revision;
    return UMI_STATUS_OK;
}
