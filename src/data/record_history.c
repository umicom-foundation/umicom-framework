/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_history.c
 * PURPOSE: Preserve immutable amendment rows and a checked current revision.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/record_history.h"
#include "include/record_fields.h"
#include <stdio.h>

/* Keep shared record ownership in Data Server. Applications supply metadata
 * and reviewed revisions; they do not implement their own persistence keys. */
static bool amendment_valid(const UmiRecordAmendment *record)
{
    if (!umi_record_field_valid(record->entity_id, sizeof record->entity_id, false) ||
        !umi_record_field_valid(record->source, sizeof record->source, false) ||
        !umi_record_field_valid(record->actor, sizeof record->actor, false) ||
        !umi_record_field_valid(record->reason, sizeof record->reason, false) ||
        !umi_record_field_valid(record->content_sha256, sizeof record->content_sha256, false) ||
        record->recorded_at_nanoseconds == 0U || strlen(record->content_sha256) != 64U) {
        return false;
    }
    for (size_t i = 0U; i < 64U; ++i) {
        char c = record->content_sha256[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) {
            return false;
        }
    }
    return true;
}

UmiStatus umi_record_revision_format(const UmiRecordRevision *record,
                                    char *output, size_t capacity)
{
    if (record == NULL || output == NULL || !amendment_valid(&record->amendment) ||
        record->previous_revision == UINT64_MAX ||
        record->revision != record->previous_revision + 1U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    const UmiRecordAmendment *a = &record->amendment;
    char text[UMI_RECORD_REVISION_TEXT_CAPACITY];
    int length = snprintf(text, sizeof text, "UMIR1\t%s\t%llu\t%llu\t%s\t%s\t%llu\t%llu\t%s\t%s",
        a->entity_id, (unsigned long long)record->revision,
        (unsigned long long)record->previous_revision, a->source, a->actor,
        (unsigned long long)a->observed_at_nanoseconds,
        (unsigned long long)a->recorded_at_nanoseconds, a->reason, a->content_sha256);
    if (length < 0 || (size_t)length >= sizeof text || (size_t)length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(output, text, (size_t)length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_record_revision_parse(const char *text, size_t length,
                                   UmiRecordRevision *output)
{
    if (text == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length == 0U || length >= UMI_RECORD_REVISION_TEXT_CAPACITY ||
        memchr(text, '\0', length) != NULL) {
        return UMI_STATUS_PARSE_ERROR;
    }
    char local[UMI_RECORD_REVISION_TEXT_CAPACITY];
    memcpy(local, text, length);
    local[length] = '\0';
    char *fields[10];
    UmiRecordRevision candidate = {0};
    UmiRecordAmendment *a = &candidate.amendment;
    if (!umi_record_fields_split(local, sizeof local, fields, 10U) ||
        strcmp(fields[0], "UMIR1") != 0 ||
        !umi_record_field_copy(a->entity_id, sizeof a->entity_id, fields[1], false) ||
        !umi_record_field_u64(fields[2], &candidate.revision) ||
        !umi_record_field_u64(fields[3], &candidate.previous_revision) ||
        !umi_record_field_copy(a->source, sizeof a->source, fields[4], false) ||
        !umi_record_field_copy(a->actor, sizeof a->actor, fields[5], false) ||
        !umi_record_field_u64(fields[6], &a->observed_at_nanoseconds) ||
        !umi_record_field_u64(fields[7], &a->recorded_at_nanoseconds) ||
        !umi_record_field_copy(a->reason, sizeof a->reason, fields[8], false) ||
        !umi_record_field_copy(a->content_sha256, sizeof a->content_sha256, fields[9], false) ||
        !amendment_valid(a) || candidate.previous_revision == UINT64_MAX ||
        candidate.revision != candidate.previous_revision + 1U) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *output = candidate;
    return UMI_STATUS_OK;
}

/* Encode every identifier byte, including punctuation, to prevent key aliases.
 * 63 bytes become 126 hex digits; the complete key fits the memory backend's
 * 192-byte key contract, including prefix, revision and terminator. */
static UmiStatus history_key(const char *entity, uint64_t revision, char *key, size_t capacity)
{
    if (entity == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    static const char hex[] = "0123456789abcdef";
    char encoded[127];
    size_t length = 0U;
    while (length < 64U && entity[length] != '\0') {
        unsigned char byte = (unsigned char)entity[length];
        if (length == 63U || byte < 32U || byte == 127U) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        encoded[length * 2U] = hex[byte >> 4U];
        encoded[length * 2U + 1U] = hex[byte & 15U];
        ++length;
    }
    if (length == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    encoded[length * 2U] = '\0';
    int written = revision == 0U
        ? snprintf(key, capacity, "repo.record-history.h.%s", encoded)
        : snprintf(key, capacity, "repo.record-history.r.%s.%020llu", encoded,
                   (unsigned long long)revision);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

static UmiStatus history_head(const UmiDataServer *server, const char *entity,
                              uint64_t *head)
{
    char key[192];
    UmiStatus status = history_key(entity, 0U, key, sizeof key);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    char text[32];
    memset(text, 0xff, sizeof text);
    status = umi_data_server_get(server, key, text, sizeof text);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    uint64_t candidate;
    if (memchr(text, '\0', sizeof text) == NULL ||
        !umi_record_field_u64(text, &candidate) || candidate == 0U) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *head = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_history_load(const UmiDataServer *server, const char *entity_id,
                                 uint64_t revision, UmiRecordRevision *output)
{
    if (server == NULL || output == NULL || revision == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    char key[192];
    UmiStatus status = history_key(entity_id, revision, key, sizeof key);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    char text[UMI_RECORD_REVISION_TEXT_CAPACITY];
    memset(text, 0xff, sizeof text);
    status = umi_data_server_get(server, key, text, sizeof text);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    const char *end = memchr(text, '\0', sizeof text);
    if (end == NULL) {
        return UMI_STATUS_PARSE_ERROR;
    }
    UmiRecordRevision candidate;
    status = umi_record_revision_parse(text, (size_t)(end - text), &candidate);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (candidate.revision != revision || strcmp(candidate.amendment.entity_id, entity_id) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    *output = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_history_latest(const UmiDataServer *server,
                                   const char *entity_id, UmiRecordRevision *output)
{
    if (server == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    uint64_t head;
    UmiStatus status = history_head(server, entity_id, &head);
    return status == UMI_STATUS_OK
        ? umi_record_history_load(server, entity_id, head, output) : status;
}

static UmiStatus history_abort(UmiDataServer *server, UmiStatus original)
{
    /* A failed rollback must not be reported as an ordinary stale-edit rejection.
     * Data Server retains its recovery state if the backend is still active. */
    UmiStatus rolled_back = umi_data_server_rollback(server);
    return rolled_back == UMI_STATUS_OK ? original : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_record_history_append(UmiDataServer *server,
                                   const UmiRecordAmendment *amendment,
                                   uint64_t expected_revision,
                                   UmiRecordRevision *output)
{
    if (server == NULL || amendment == NULL || output == NULL || !amendment_valid(amendment)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (expected_revision == UINT64_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Capture input before touching output, including when callers reuse an
     * existing revision's amendment as the next request. */
    UmiRecordRevision candidate = {*amendment, expected_revision + 1U, expected_revision};
    char text[UMI_RECORD_REVISION_TEXT_CAPACITY];
    UmiStatus status = umi_record_revision_format(&candidate, text, sizeof text);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    uint64_t head = 0U;
    status = history_head(server, amendment->entity_id, &head);
    if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
        return history_abort(server, status);
    }
    if (head != expected_revision) {
        return history_abort(server, UMI_STATUS_INVALID_STATE);
    }
    if (head != 0U) {
        UmiRecordRevision previous;
        status = umi_record_history_load(server, amendment->entity_id, head, &previous);
        if (status != UMI_STATUS_OK) {
            return history_abort(server, status);
        }
        if (amendment->recorded_at_nanoseconds < previous.amendment.recorded_at_nanoseconds) {
            return history_abort(server, UMI_STATUS_INVALID_ARGUMENT);
        }
    }
    char key[192];
    status = history_key(amendment->entity_id, candidate.revision, key, sizeof key);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    char occupied[2];
    status = umi_data_server_get(server, key, occupied, sizeof occupied);
    if (status != UMI_STATUS_NOT_FOUND) {
        return history_abort(server,
            status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED
                ? UMI_STATUS_INVALID_STATE : status);
    }
    status = umi_data_server_set(server, key, text);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    status = history_key(amendment->entity_id, 0U, key, sizeof key);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    char head_text[32];
    (void)snprintf(head_text, sizeof head_text, "%llu", (unsigned long long)candidate.revision);
    status = umi_data_server_set(server, key, head_text);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    *output = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_history_verify(UmiDataServer *server, const char *entity_id,
                                   uint64_t *out_revisions)
{
    if (server == NULL || out_revisions == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    uint64_t head;
    status = history_head(server, entity_id, &head);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    uint64_t previous_time = 0U;
    for (uint64_t revision = 1U;; ++revision) {
        UmiRecordRevision record;
        status = umi_record_history_load(server, entity_id, revision, &record);
        if (status != UMI_STATUS_OK) {
            return history_abort(server, status);
        }
        if (record.amendment.recorded_at_nanoseconds < previous_time) {
            return history_abort(server, UMI_STATUS_PARSE_ERROR);
        }
        previous_time = record.amendment.recorded_at_nanoseconds;
        if (revision == head) {
            break;
        }
    }
    status = umi_data_server_commit(server);
    if (status != UMI_STATUS_OK) {
        return history_abort(server, status);
    }
    *out_revisions = head;
    return UMI_STATUS_OK;
}
