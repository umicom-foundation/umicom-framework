/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * An installed-SDK example of editing while a captured save is outstanding.
 * The console runs storage inline for clarity. GUI hosts dispatch the copied
 * request to their managed worker and return completion to the owning thread. */
#include "umicom/data/record_draft.h"
#include <stdio.h>
#include <string.h>

static UmiStatus store_request(UmiDataServer *server, UmiRecordDraft *draft,
                               const UmiRecordSaveRequest *request)
{
    UmiRecordRevision saved;
    UmiStatus status = umi_record_save_request_commit(server, request, &saved);
    return umi_record_draft_finish_save(draft, request->request_id, status,
                                        status == UMI_STATUS_OK ? &saved : NULL);
}

static UmiStatus edit_example(UmiDataServer *server, const char *entity)
{
    UmiRecordDraft *draft = NULL;
    UmiStatus status = umi_record_draft_create(&draft);
    if (status != UMI_STATUS_OK) return status;
    UmiRecordDraftAttribution author = {.source = "synthetic-example",
        .actor = "fictional-reviewer", .reason = "Save an example review",
        .recorded_at_nanoseconds = 100U};
    UmiRecordSaveRequest request;
    status = umi_record_draft_begin(draft, entity);
    const char first[] = "Fictional administrative review: pending.";
    const char later[] = "Fictional administrative review: checked; no action initiated.";
    if (status == UMI_STATUS_OK) status = umi_record_draft_set_content(draft, first, sizeof first - 1U);
    if (status == UMI_STATUS_OK) status = umi_record_draft_prepare_save(draft, &author, &request);
    /* The second edit happens after capture, before the first save completes. */
    if (status == UMI_STATUS_OK) status = umi_record_draft_set_content(draft, later, sizeof later - 1U);
    if (status == UMI_STATUS_OK) status = store_request(server, draft, &request);
    UmiRecordDraftState state;
    if (status == UMI_STATUS_OK) status = umi_record_draft_state(draft, &state);
    if (status == UMI_STATUS_OK && (!state.dirty || state.saving || state.expected_revision != 1U)) {
        status = UMI_STATUS_INVALID_STATE;
    }
    author.recorded_at_nanoseconds = 200U;
    if (status == UMI_STATUS_OK) status = umi_record_draft_prepare_save(draft, &author, &request);
    if (status == UMI_STATUS_OK) status = store_request(server, draft, &request);
    if (status == UMI_STATUS_OK) status = umi_record_draft_state(draft, &state);
    if (status == UMI_STATUS_OK && (state.dirty || state.saving || state.expected_revision != 2U)) {
        status = UMI_STATUS_INVALID_STATE;
    }
    UmiRecordDocument document;
    if (status == UMI_STATUS_OK) status = umi_record_document_latest(server, entity, &document);
    if (status == UMI_STATUS_OK && (document.length != sizeof later - 1U ||
        memcmp(document.content, later, sizeof later - 1U) != 0)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = umi_record_document_load(server, entity, 1U, &document);
    if (status == UMI_STATUS_OK && (document.length != sizeof first - 1U ||
        memcmp(document.content, first, sizeof first - 1U) != 0)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        printf("%s: later edit retained; both captured revisions readable\n", entity);
    }
    umi_record_draft_destroy(draft);
    return status;
}

int main(void)
{
    UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK) status = edit_example(server, "synthetic-care-review");
    if (status == UMI_STATUS_OK) status = edit_example(server, "synthetic-bank-review");
    umi_data_server_destroy(server);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Record draft example failed with status %d\n", (int)status);
        return 1;
    }
    return 0;
}
