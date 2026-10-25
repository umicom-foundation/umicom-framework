/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_draft_snapshot.c
 *
 * PURPOSE:
 *   Serialize explicit fields, never a native structure. Canonical metadata and
 *   a digest bind the reviewed baseline to its retained unsaved content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Serialize explicit fields, never a native structure. Canonical metadata and
 * a digest bind the reviewed baseline to its retained unsaved content. */
#include "umicom/data/record_draft_snapshot.h"
#include "umicom/base/sha256.h"
#include <stdlib.h>
#include <string.h>

#define SNAPSHOT_HEADER_BYTES 24U
#define SNAPSHOT_DIGEST_BYTES 64U
static const unsigned char snapshot_magic[8] = {'U','M','I','R','D','S','1','\n'};
static void write_u32(unsigned char *bytes, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i) bytes[i] = (unsigned char)(value >> ((3U - i) * 8U));
}
static uint32_t read_u32(const unsigned char *bytes)
{
    uint32_t value = 0U;
    for (size_t i = 0U; i < 4U; ++i) value = (value << 8U) | bytes[i];
    return value;
}
static UmiStatus metadata_text(const UmiRecordDraftSnapshot *snapshot, char *text, size_t capacity)
{
    if (snapshot->baseline.record.revision != 0U)
        return umi_record_revision_format(&snapshot->baseline.record, text, capacity);
    size_t length = strlen(snapshot->baseline.record.amendment.entity_id);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(text, snapshot->baseline.record.amendment.entity_id, length + 1U);
    return UMI_STATUS_OK;
}
UmiStatus umi_record_draft_snapshot_encode(const UmiRecordDraftSnapshot *snapshot,
    unsigned char **output, size_t *output_length)
{
    if (output == NULL || output_length == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_record_draft_snapshot_validate(snapshot);
    if (status != UMI_STATUS_OK) return status;
    char metadata[UMI_RECORD_REVISION_TEXT_CAPACITY];
    status = metadata_text(snapshot, metadata, sizeof metadata);
    if (status != UMI_STATUS_OK) return status;
    size_t metadata_length = strlen(metadata);
    size_t signed_length = SNAPSHOT_HEADER_BYTES + metadata_length + snapshot->baseline.length + snapshot->length;
    unsigned char *bytes = calloc(signed_length + SNAPSHOT_DIGEST_BYTES, 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(bytes, snapshot_magic, sizeof snapshot_magic);
    bytes[8] = snapshot->baseline.record.revision == 0U ? 0U : 1U;
    write_u32(bytes + 12U, (uint32_t)metadata_length);
    write_u32(bytes + 16U, (uint32_t)snapshot->baseline.length);
    write_u32(bytes + 20U, (uint32_t)snapshot->length);
    memcpy(bytes + SNAPSHOT_HEADER_BYTES, metadata, metadata_length);
    memcpy(bytes + SNAPSHOT_HEADER_BYTES + metadata_length, snapshot->baseline.content, snapshot->baseline.length);
    memcpy(bytes + SNAPSHOT_HEADER_BYTES + metadata_length + snapshot->baseline.length, snapshot->content, snapshot->length);
    char digest[65];
    status = UmiSha256Buffer(bytes, signed_length, digest);
    if (status != UMI_STATUS_OK) { free(bytes); return status; }
    memcpy(bytes + signed_length, digest, SNAPSHOT_DIGEST_BYTES);
    *output = bytes;
    *output_length = signed_length + SNAPSHOT_DIGEST_BYTES;
    return UMI_STATUS_OK;
}
UmiStatus umi_record_draft_snapshot_decode(const void *input, size_t length, UmiRecordDraftSnapshot *output)
{
    if (input == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_RECORD_DRAFT_SNAPSHOT_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < SNAPSHOT_HEADER_BYTES + SNAPSHOT_DIGEST_BYTES) return UMI_STATUS_PARSE_ERROR;
    const unsigned char *bytes = input;
    if (memcmp(bytes, snapshot_magic, sizeof snapshot_magic) != 0 || bytes[8] > 1U ||
        bytes[9] != 0U || bytes[10] != 0U || bytes[11] != 0U) return UMI_STATUS_PARSE_ERROR;
    size_t metadata_length = read_u32(bytes + 12U), baseline_length = read_u32(bytes + 16U), draft_length = read_u32(bytes + 20U);
    if (metadata_length == 0U || metadata_length >= UMI_RECORD_REVISION_TEXT_CAPACITY ||
        baseline_length > UMI_RECORD_DOCUMENT_MAX_BYTES || draft_length > UMI_RECORD_DOCUMENT_MAX_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    /* Bounds are checked before summing, so malformed lengths cannot overflow
     * an offset or cause a read outside the supplied record. */
    size_t signed_length = SNAPSHOT_HEADER_BYTES + metadata_length + baseline_length + draft_length;
    if (length != signed_length + SNAPSHOT_DIGEST_BYTES) return UMI_STATUS_PARSE_ERROR;
    char digest[65];
    UmiStatus status = UmiSha256Buffer(bytes, signed_length, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(bytes + signed_length, digest, SNAPSHOT_DIGEST_BYTES) != 0 ||
        memchr(bytes + SNAPSHOT_HEADER_BYTES, '\0', metadata_length) != NULL) return UMI_STATUS_PARSE_ERROR;
    UmiRecordDraftSnapshot *candidate = calloc(1U, sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (bytes[8] == 0U) {
        if (metadata_length >= sizeof candidate->baseline.record.amendment.entity_id || baseline_length != 0U)
            status = UMI_STATUS_PARSE_ERROR;
        else memcpy(candidate->baseline.record.amendment.entity_id, bytes + SNAPSHOT_HEADER_BYTES, metadata_length);
    } else {
        status = umi_record_revision_parse((const char *)bytes + SNAPSHOT_HEADER_BYTES,
            metadata_length, &candidate->baseline.record);
    }
    candidate->baseline.length = baseline_length;
    candidate->length = draft_length;
    memcpy(candidate->baseline.content, bytes + SNAPSHOT_HEADER_BYTES + metadata_length, baseline_length);
    memcpy(candidate->content, bytes + SNAPSHOT_HEADER_BYTES + metadata_length + baseline_length, draft_length);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_validate(candidate);
    if (status == UMI_STATUS_OK) {
        char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
        status = metadata_text(candidate, canonical, sizeof canonical);
        if (status == UMI_STATUS_OK && (strlen(canonical) != metadata_length ||
            memcmp(canonical, bytes + SNAPSHOT_HEADER_BYTES, metadata_length) != 0)) status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK) *output = *candidate;
    free(candidate);
    return status;
}
void umi_record_draft_snapshot_bytes_free(void *bytes) { free(bytes); }

UmiStatus umi_record_draft_snapshot_check_store(UmiDataServer *server, const UmiRecordDraftSnapshot *snapshot)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_record_draft_snapshot_validate(snapshot);
    if (status != UMI_STATUS_OK || snapshot->baseline.record.revision == 0U) return status;
    UmiRecordDocument *stored = calloc(1U, sizeof *stored);
    if (stored == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_record_document_load(server, snapshot->baseline.record.amendment.entity_id,
        snapshot->baseline.record.revision, stored);
    if (status == UMI_STATUS_OK) {
        char actual[UMI_RECORD_REVISION_TEXT_CAPACITY], captured[UMI_RECORD_REVISION_TEXT_CAPACITY];
        status = umi_record_revision_format(&stored->record, actual, sizeof actual);
        if (status == UMI_STATUS_OK) status = umi_record_revision_format(&snapshot->baseline.record, captured, sizeof captured);
        if (status == UMI_STATUS_OK && (strcmp(actual, captured) != 0 || stored->length != snapshot->baseline.length ||
            memcmp(stored->content, snapshot->baseline.content, stored->length) != 0)) status = UMI_STATUS_INVALID_STATE;
    }
    free(stored);
    return status;
}
