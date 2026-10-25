/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_draft_snapshot.c
 *
 * PURPOSE:
 *   Recovery regression cases verify state ownership and malformed binary inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Recovery regression cases verify state ownership and malformed binary inputs. */
#include "umicom/data/record_draft_snapshot.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static void resign(unsigned char *bytes, size_t length)
{
    char digest[65];
    if (length >= 64U && UmiSha256Buffer(bytes, length - 64U, digest) == UMI_STATUS_OK)
        memcpy(bytes + length - 64U, digest, 64U);
}
static int exercise(const char *name)
{
    int result = 0;
    UmiDataServer *store = NULL, *other = NULL;
    UmiRecordDraft *draft = NULL, *restored = NULL;
    UmiRecordDraftSnapshot *snapshot = calloc(1U, sizeof *snapshot), *decoded = calloc(1U, sizeof *decoded), *guard = calloc(1U, sizeof *guard);
    UmiRecordDocument *document = calloc(1U, sizeof *document);
    UmiRecordSaveRequest *request = calloc(1U, sizeof *request);
    unsigned char *bytes = NULL, *extra = NULL;
    size_t length = 0U;
    UmiRecordRevision saved;
    UmiRecordDraftState state;
    UmiRecordAmendment author = {.entity_id = "fixture.review", .source = "fixture", .actor = "reviewer", .reason = "Synthetic baseline", .recorded_at_nanoseconds = 1U};
    UmiRecordDraftAttribution attribution = {.source = "fixture", .actor = "reviewer", .reason = "Recovered work", .recorded_at_nanoseconds = 3U};
    CHECK(snapshot && decoded && guard && document && request);
    OK(umi_data_server_create_memory(&store));
    OK(umi_record_document_append(store, &author, "base", 4U, 0U, &saved));
    OK(umi_record_document_latest(store, author.entity_id, document));
    OK(umi_record_draft_create(&draft)); OK(umi_record_draft_create(&restored));
    OK(umi_record_draft_open(draft, document)); OK(umi_record_draft_set_content(draft, "local", 5U));
    if (strcmp(name, "new") == 0 || strcmp(name, "empty") == 0 || strcmp(name, "new-metadata") == 0) {
        OK(umi_record_draft_discard(draft)); OK(umi_record_draft_begin(draft, "fixture.new"));
        if (strcmp(name, "empty") != 0) OK(umi_record_draft_set_content(draft, "new text", 8U));
    }
    if (strcmp(name, "binary") == 0) { const unsigned char raw[] = {0U, 255U, 13U, 10U, 0U}; OK(umi_record_draft_set_content(draft, raw, sizeof raw)); }
    if (strcmp(name, "limit") == 0 || strcmp(name, "max-baseline") == 0) {
        memset(snapshot->content, 0xa7, sizeof snapshot->content);
        if (strcmp(name, "max-baseline") == 0) {
            OK(umi_record_draft_discard(draft));
            OK(umi_record_document_append(store, &author, snapshot->content, sizeof snapshot->content, 1U, &saved));
            OK(umi_record_document_latest(store, author.entity_id, document));
            OK(umi_record_draft_open(draft, document));
            snapshot->content[0] = 0U;
        }
        OK(umi_record_draft_set_content(draft, snapshot->content, sizeof snapshot->content));
    }
    OK(umi_record_draft_snapshot_capture(draft, snapshot));
    OK(umi_record_draft_state(draft, &state)); CHECK(state.dirty && !state.saving);
    OK(umi_record_draft_snapshot_encode(snapshot, &bytes, &length));
    OK(umi_record_draft_snapshot_decode(bytes, length, decoded));
    CHECK(decoded->length == snapshot->length && memcmp(decoded->content, snapshot->content, snapshot->length) == 0);
    CHECK(decoded->baseline.record.revision == snapshot->baseline.record.revision);
    for (size_t i = decoded->length; i < sizeof decoded->content; ++i) CHECK(decoded->content[i] == 0U);
    OK(umi_record_draft_snapshot_restore(restored, decoded));
    OK(umi_record_draft_state(restored, &state)); CHECK(state.dirty && state.expected_revision == snapshot->baseline.record.revision);
    if (strcmp(name, "saved") == 0 || strcmp(name, "new") == 0 || strcmp(name, "empty") == 0 || strcmp(name, "binary") == 0 || strcmp(name, "limit") == 0 || strcmp(name, "max-baseline") == 0) {
        OK(umi_record_draft_prepare_save(restored, &attribution, request));
        CHECK(request->length == snapshot->length && memcmp(request->content, snapshot->content, snapshot->length) == 0);
    } else if (strcmp(name, "integrity") == 0) {
        *guard = *decoded; bytes[length - 65U] ^= 1U;
        CHECK(umi_record_draft_snapshot_decode(bytes, length, decoded) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
    } else if (strcmp(name, "truncated") == 0) {
        *guard = *decoded;
        for (size_t n = 0U; n < length; ++n) {
            CHECK(umi_record_draft_snapshot_decode(bytes, n, decoded) != UMI_STATUS_OK);
            CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
        }
    } else if (strcmp(name, "trailing") == 0) {
        extra = calloc(length + 1U, 1U); CHECK(extra); memcpy(extra, bytes, length);
        CHECK(umi_record_draft_snapshot_decode(extra, length + 1U, decoded) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "framing") == 0) {
        const size_t offsets[] = {6U, 8U, 9U, 10U, 11U, 12U, 16U, 20U};
        *guard = *decoded;
        for (size_t i = 0U; i < sizeof offsets / sizeof offsets[0]; ++i) {
            unsigned char old = bytes[offsets[i]]; bytes[offsets[i]] = 255U; resign(bytes, length);
            CHECK(umi_record_draft_snapshot_decode(bytes, length, decoded) != UMI_STATUS_OK);
            CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
            bytes[offsets[i]] = old; resign(bytes, length);
        }
    } else if (strcmp(name, "canonical") == 0) {
        size_t metadata_length = ((size_t)bytes[12] << 24U) | ((size_t)bytes[13] << 16U) | ((size_t)bytes[14] << 8U) | bytes[15];
        char metadata[UMI_RECORD_REVISION_TEXT_CAPACITY] = {0};
        memcpy(metadata, bytes + 24U, metadata_length);
        char *revision = strstr(metadata, "\t1\t0\t"); CHECK(revision != NULL);
        size_t offset = 24U + (size_t)(revision - metadata) + 1U;
        extra = calloc(length + 1U, 1U); CHECK(extra);
        memcpy(extra, bytes, offset); extra[offset] = '0'; memcpy(extra + offset + 1U, bytes + offset, length - offset);
        ++metadata_length;
        for (size_t i = 0U; i < 4U; ++i) extra[12U + i] = (unsigned char)(metadata_length >> ((3U - i) * 8U));
        resign(extra, length + 1U); *guard = *decoded;
        CHECK(umi_record_draft_snapshot_decode(extra, length + 1U, decoded) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
    } else if (strcmp(name, "metadata") == 0) {
        *guard = *decoded; bytes[24] = '\0'; resign(bytes, length);
        CHECK(umi_record_draft_snapshot_decode(bytes, length, decoded) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
        snapshot->baseline.content[0] ^= 1U;
        CHECK(umi_record_draft_snapshot_validate(snapshot) == UMI_STATUS_PARSE_ERROR);
    } else if (strcmp(name, "new-metadata") == 0) {
        snapshot->baseline.record.amendment.source[0] = 'x';
        CHECK(umi_record_draft_snapshot_validate(snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_draft_snapshot_restore(restored, snapshot) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "dirty") == 0) {
        OK(umi_record_draft_set_content(restored, "keep", 4U));
        CHECK(umi_record_draft_snapshot_restore(restored, snapshot) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_snapshot_capture(restored, decoded)); CHECK(decoded->length == 4U && memcmp(decoded->content, "keep", 4U) == 0);
    } else if (strcmp(name, "pending") == 0) {
        OK(umi_record_draft_prepare_save(draft, &attribution, request)); *guard = *decoded;
        CHECK(umi_record_draft_snapshot_capture(draft, decoded) == UMI_STATUS_BUSY);
        CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
        CHECK(umi_record_draft_snapshot_restore(draft, snapshot) == UMI_STATUS_BUSY);
    } else if (strcmp(name, "capture-state") == 0) {
        umi_record_draft_destroy(restored); restored = NULL; OK(umi_record_draft_create(&restored)); *guard = *decoded;
        CHECK(umi_record_draft_snapshot_capture(restored, decoded) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(decoded, guard, sizeof *guard) == 0);
        size_t untouched_length = 123U; unsigned char *untouched_bytes = bytes;
        snapshot->length = UMI_RECORD_DOCUMENT_MAX_BYTES + 1U;
        CHECK(umi_record_draft_snapshot_encode(snapshot, &untouched_bytes, &untouched_length) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(untouched_bytes == bytes && untouched_length == 123U);
        CHECK(umi_record_draft_snapshot_restore(restored, snapshot) == UMI_STATUS_CAPACITY_EXCEEDED);
        OK(umi_record_draft_state(restored, &state)); CHECK(!state.ready);
    } else if (strcmp(name, "token") == 0) {
        OK(umi_record_draft_prepare_save(restored, &attribution, request)); uint64_t first = request->request_id;
        CHECK(umi_record_draft_finish_save(restored, first, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_IO_ERROR);
        OK(umi_record_draft_discard(restored)); OK(umi_record_draft_snapshot_restore(restored, snapshot));
        OK(umi_record_draft_prepare_save(restored, &attribution, request)); CHECK(request->request_id > first);
        CHECK(umi_record_draft_finish_save(restored, first, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_state(restored, &state)); CHECK(state.saving);
    } else if (strcmp(name, "store-match") == 0) {
        OK(umi_record_draft_snapshot_check_store(store, snapshot));
        OK(umi_data_server_begin(store)); CHECK(umi_record_draft_snapshot_check_store(store, snapshot) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(store)); OK(umi_data_server_rollback(store));
    } else if (strcmp(name, "store-mismatch") == 0) {
        OK(umi_data_server_create_memory(&other)); author.actor[0] = 'X';
        OK(umi_record_document_append(other, &author, "base", 4U, 0U, &saved));
        CHECK(umi_record_draft_snapshot_check_store(other, snapshot) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "stale-save") == 0) {
        author.recorded_at_nanoseconds = 2U;
        OK(umi_record_document_append(store, &author, "external", 8U, 1U, &saved));
        OK(umi_record_draft_snapshot_check_store(store, snapshot));
        OK(umi_record_draft_prepare_save(restored, &attribution, request));
        CHECK(umi_record_save_request_commit(store, request, &saved) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_draft_finish_save(restored, request->request_id, UMI_STATUS_INVALID_STATE, NULL) == UMI_STATUS_INVALID_STATE);
        OK(umi_record_draft_state(restored, &state)); CHECK(state.dirty && state.expected_revision == 1U);
    } else if (strcmp(name, "missing") == 0) {
        OK(umi_data_server_create_memory(&other)); CHECK(umi_record_draft_snapshot_check_store(other, snapshot) == UMI_STATUS_NOT_FOUND);
    } else CHECK(0 && "Unknown recovery case");
cleanup:
    free(extra); umi_record_draft_snapshot_bytes_free(bytes);
    free(snapshot); free(decoded); free(guard); free(document); free(request);
    umi_record_draft_destroy(draft); umi_record_draft_destroy(restored);
    umi_data_server_destroy(store); umi_data_server_destroy(other);
    return result;
}
int main(int argc, char **argv) { return argc == 2 ? exercise(argv[1]) : 2; }
