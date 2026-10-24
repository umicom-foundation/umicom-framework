/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * These tests connect detached save requests to real Data Server operations.
 * Each run owns an isolated store; no private record data or network is used. */
#include "umicom/data/record_draft.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

static int exercise(UmiDataServer *server, const char *name)
{
    UmiRecordAmendment metadata = {.entity_id = "example", .source = "test-import",
        .actor = "test-reviewer", .reason = "Original", .recorded_at_nanoseconds = 10U};
    UmiRecordDraftAttribution author = {.source = "test-import", .actor = "test-reviewer",
        .reason = "Correction", .recorded_at_nanoseconds = 20U};
    UmiRecordRevision saved;
    UmiRecordDocument loaded;
    UmiRecordSaveRequest request;
    UmiRecordDraft *draft = NULL;
    OK(umi_record_draft_create(&draft));
    OK(umi_record_document_append(server, &metadata, "original", 8U, 0U, &saved));
    OK(umi_record_document_latest(server, "example", &loaded));
    OK(umi_record_draft_open(draft, &loaded));
    OK(umi_record_draft_set_content(draft, "local", 5U));
    OK(umi_record_draft_prepare_save(draft, &author, &request));
    if (strcmp(name, "stale") == 0) {
        OK(umi_record_document_append(server, &metadata, "another author", 14U, 1U, &saved));
        UmiStatus status = umi_record_save_request_commit(server, &request, &saved);
        CHECK(status == UMI_STATUS_INVALID_STATE);
        CHECK(umi_record_draft_finish_save(draft, request.request_id, status, NULL) == status);
        UmiRecordDraftState state; OK(umi_record_draft_state(draft, &state));
        CHECK(state.dirty && !state.saving && state.expected_revision == 1U);
        char local[16]; size_t length;
        OK(umi_record_draft_copy_content(draft, local, sizeof local, &length));
        CHECK(length == 5U && memcmp(local, "local", 5U) == 0);
        OK(umi_record_document_latest(server, "example", &loaded));
        CHECK(loaded.record.revision == 2U && loaded.length == 14U);
        CHECK(memcmp(loaded.content, "another author", 14U) == 0);
        CHECK(umi_record_draft_open(draft, &loaded) == UMI_STATUS_INVALID_STATE);
        /* The caller preserves local bytes for comparison, then explicitly
         * discards and reloads. No automatic merge overwrites either author. */
        OK(umi_record_draft_discard(draft));
        OK(umi_record_draft_open(draft, &loaded));
        OK(umi_record_draft_set_content(draft, local, length));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.proposed.previous_revision == 2U);
        OK(umi_record_save_request_commit(server, &request, &saved));
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &saved));
        OK(umi_record_document_load(server, "example", 2U, &loaded));
        CHECK(loaded.length == 14U && memcmp(loaded.content, "another author", 14U) == 0);
    } else if (strcmp(name, "snapshot") == 0) {
        OK(umi_record_draft_set_content(draft, "continued typing", 16U));
        OK(umi_record_save_request_commit(server, &request, &saved));
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &saved));
        UmiRecordDraftState state; OK(umi_record_draft_state(draft, &state));
        CHECK(state.dirty && !state.saving && state.expected_revision == 2U);
        OK(umi_record_document_latest(server, "example", &loaded));
        CHECK(loaded.length == 5U && memcmp(loaded.content, "local", 5U) == 0);
        char local[32]; size_t length;
        OK(umi_record_draft_copy_content(draft, local, sizeof local, &length));
        CHECK(length == 16U && memcmp(local, "continued typing", 16U) == 0);
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        OK(umi_record_save_request_commit(server, &request, &saved));
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &saved));
        OK(umi_record_draft_state(draft, &state));
        CHECK(!state.dirty && !state.saving && state.expected_revision == 3U);
    } else if (strcmp(name, "busy") == 0) {
        OK(umi_data_server_begin(server));
        UmiStatus status = umi_record_save_request_commit(server, &request, &saved);
        CHECK(status == UMI_STATUS_BUSY);
        CHECK(umi_record_draft_finish_save(draft, request.request_id, status, NULL) == status);
        OK(umi_data_server_rollback(server));
        OK(umi_record_draft_prepare_save(draft, &author, &request));
        CHECK(request.proposed.previous_revision == 1U);
        OK(umi_record_save_request_commit(server, &request, &saved));
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &saved));
    } else if (strcmp(name, "request-integrity") == 0) {
        UmiRecordSaveRequest corrupt = request;
        memset(&saved, 0xa5, sizeof saved);
        unsigned char before[sizeof saved]; memcpy(before, &saved, sizeof saved);
        corrupt.content[0] = 'X';
        CHECK(umi_record_save_request_commit(server, &corrupt, &saved) == UMI_STATUS_PARSE_ERROR);
        corrupt = request; corrupt.request_id = 0U;
        CHECK(umi_record_save_request_commit(server, &corrupt, &saved) == UMI_STATUS_INVALID_ARGUMENT);
        corrupt = request; corrupt.length = UMI_RECORD_DOCUMENT_MAX_BYTES + 1U;
        CHECK(umi_record_save_request_commit(server, &corrupt, &saved) == UMI_STATUS_CAPACITY_EXCEEDED);
        corrupt = request; corrupt.proposed.revision++;
        CHECK(umi_record_save_request_commit(server, &corrupt, &saved) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_save_request_commit(NULL, &request, &saved) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_save_request_commit(server, NULL, &saved) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_record_save_request_commit(server, &request, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, &saved, sizeof saved) == 0);
        OK(umi_record_document_latest(server, "example", &loaded));
        CHECK(loaded.record.revision == 1U && loaded.length == 8U);
        CHECK(memcmp(loaded.content, "original", 8U) == 0);
        /* A rejected corrupted copy leaves the valid original request usable. */
        OK(umi_record_save_request_commit(server, &request, &saved));
        OK(umi_record_draft_finish_save(draft, request.request_id, UMI_STATUS_OK, &saved));
    } else {
        fprintf(stderr, "Unknown draft storage regression: %s\n", name);
        umi_record_draft_destroy(draft);
        return 1;
    }
    uint64_t count = 0U; OK(umi_record_document_verify(server, "example", &count));
    CHECK(count == (strcmp(name, "busy") == 0 || strcmp(name, "request-integrity") == 0 ? 2U : 3U));
    umi_record_draft_destroy(draft);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    UmiDataServer *server = NULL;
    UmiStatus status;
    if (strcmp(argv[1], "memory") == 0) status = umi_data_server_create_memory(&server);
    else if (strcmp(argv[1], "sqlite") == 0) status = umi_data_server_create_sqlite(":memory:", &server);
    else return 2;
    if (status != UMI_STATUS_OK) { fprintf(stderr, "Cannot open test store: %d\n", (int)status); return 1; }
    int result = exercise(server, argv[2]);
    umi_data_server_destroy(server);
    return result;
}
