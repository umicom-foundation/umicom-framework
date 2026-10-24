/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_draft.c
 * PURPOSE: Own record drafts independently of storage workers and UI widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/record_draft.h"
#include "umicom/data/record_draft_snapshot.h"
#include "umicom/base/sha256.h"
#include "include/record_fields.h"
#include <stdlib.h>

/* Keeping the baseline, editable bytes and pending snapshot separate prevents
 * a completed write from clearing newer user edits. The data SDK owns this
 * state machine so native and web hosts use the same revision discipline. */
struct UmiRecordDraft {
    bool ready;
    bool saving;
    uint64_t next_request_id;
    UmiRecordDocument baseline;
    size_t length;
    unsigned char content[UMI_RECORD_DOCUMENT_MAX_BYTES];
    UmiRecordSaveRequest pending;
};

static bool is_dirty(const UmiRecordDraft *draft)
{
    return draft->ready && (draft->baseline.record.revision == 0U ||
        draft->length != draft->baseline.length ||
        memcmp(draft->content, draft->baseline.content, draft->length) != 0);
}

static UmiStatus can_replace(const UmiRecordDraft *draft)
{
    if (draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (draft->saving) {
        return UMI_STATUS_BUSY;
    }
    return is_dirty(draft) ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}

static UmiStatus document_valid(const UmiRecordDocument *document)
{
    if (document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (document->length > UMI_RECORD_DOCUMENT_MAX_BYTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
    UmiStatus status = umi_record_revision_format(&document->record, canonical, sizeof canonical);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    char digest[65];
    status = UmiSha256Buffer(document->content, document->length, digest);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return strcmp(digest, document->record.amendment.content_sha256) == 0
        ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

UmiStatus umi_record_draft_create(UmiRecordDraft **output)
{
    if (output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiRecordDraft *draft = calloc(1U, sizeof *draft);
    if (draft == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    draft->next_request_id = 1U;
    *output = draft;
    return UMI_STATUS_OK;
}

void umi_record_draft_destroy(UmiRecordDraft *draft)
{
    free(draft);
}

UmiStatus umi_record_draft_begin(UmiRecordDraft *draft, const char *entity_id)
{
    if (entity_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = can_replace(draft);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    UmiRecordDocument candidate = {0};
    if (!umi_record_field_copy(candidate.record.amendment.entity_id,
                              sizeof candidate.record.amendment.entity_id, entity_id, false)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    draft->baseline = candidate;
    memset(draft->content, 0, sizeof draft->content);
    draft->length = 0U;
    draft->ready = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_open(UmiRecordDraft *draft, const UmiRecordDocument *document)
{
    UmiStatus status = can_replace(draft);
    if (status == UMI_STATUS_OK) {
        status = document_valid(document);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Only meaningful bytes are retained; unused caller buffer tails need not
     * be initialized and must not become part of a later exported snapshot. */
    memset(&draft->baseline, 0, sizeof draft->baseline);
    draft->baseline.record = document->record;
    draft->baseline.length = document->length;
    memcpy(draft->baseline.content, document->content, document->length);
    memset(draft->content, 0, sizeof draft->content);
    memcpy(draft->content, document->content, document->length);
    draft->length = document->length;
    draft->ready = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_discard(UmiRecordDraft *draft)
{
    if (draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (draft->saving) {
        return UMI_STATUS_BUSY;
    }
    if (!draft->ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    memset(draft->content, 0, sizeof draft->content);
    draft->length = draft->baseline.length;
    memcpy(draft->content, draft->baseline.content, draft->length);
    if (draft->baseline.record.revision == 0U) {
        memset(&draft->baseline, 0, sizeof draft->baseline);
        draft->ready = false;
    }
    /* Do not reset next_request_id: a delayed reply from a previously opened
     * document must not be mistaken for a later request on the same handle. */
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_state(const UmiRecordDraft *draft, UmiRecordDraftState *output)
{
    if (draft == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiRecordDraftState candidate = {0};
    candidate.ready = draft->ready;
    candidate.dirty = is_dirty(draft);
    candidate.saving = draft->saving;
    candidate.is_new = draft->ready && draft->baseline.record.revision == 0U;
    memcpy(candidate.entity_id, draft->baseline.record.amendment.entity_id, sizeof candidate.entity_id);
    candidate.expected_revision = draft->baseline.record.revision;
    candidate.pending_request_id = draft->saving ? draft->pending.request_id : 0U;
    candidate.length = draft->length;
    *output = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_set_content(UmiRecordDraft *draft, const void *content, size_t length)
{
    if (draft == NULL || (content == NULL && length != 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length > UMI_RECORD_DOCUMENT_MAX_BYTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (!draft->ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (length != 0U) {
        memcpy(draft->content, content, length);
    }
    memset(draft->content + length, 0, sizeof draft->content - length);
    draft->length = length;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_copy_content(const UmiRecordDraft *draft, void *content,
                                       size_t capacity, size_t *output_length)
{
    if (draft == NULL || output_length == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!draft->ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (capacity < draft->length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (content == NULL && draft->length != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (draft->length != 0U) {
        memcpy(content, draft->content, draft->length);
    }
    *output_length = draft->length;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_adopt_reviewed(UmiRecordDraft *draft,
    const UmiRecordDocument *reviewed, uint64_t expected_baseline)
{
    if (draft == NULL || reviewed == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (draft->saving) return UMI_STATUS_BUSY;
    if (!draft->ready || draft->baseline.record.revision != expected_baseline)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = document_valid(reviewed);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(reviewed->record.amendment.entity_id, draft->baseline.record.amendment.entity_id) != 0 ||
        reviewed->record.revision < draft->baseline.record.revision) return UMI_STATUS_INVALID_STATE;
    if (reviewed->record.revision == draft->baseline.record.revision) {
        char current[UMI_RECORD_REVISION_TEXT_CAPACITY], incoming[UMI_RECORD_REVISION_TEXT_CAPACITY];
        status = umi_record_revision_format(&draft->baseline.record, current, sizeof current);
        if (status == UMI_STATUS_OK) status = umi_record_revision_format(&reviewed->record, incoming, sizeof incoming);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(current, incoming) != 0 || draft->baseline.length != reviewed->length ||
            memcmp(draft->baseline.content, reviewed->content, reviewed->length) != 0)
            return UMI_STATUS_INVALID_STATE;
        return UMI_STATUS_OK;
    }
    /* Explicit review changes only the saved reference point. Applications
     * must not implement this by discard/open, which would lose the local text
     * if a later step failed. The next commit still checks the store revision. */
    memset(&draft->baseline, 0, sizeof draft->baseline);
    draft->baseline.record = reviewed->record;
    draft->baseline.length = reviewed->length;
    memcpy(draft->baseline.content, reviewed->content, reviewed->length);
    return UMI_STATUS_OK;
}

UmiStatus umi_record_draft_prepare_save(UmiRecordDraft *draft,
    const UmiRecordDraftAttribution *attribution, UmiRecordSaveRequest *output)
{
    if (draft == NULL || attribution == NULL || output == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (draft->saving) {
        return UMI_STATUS_BUSY;
    }
    if (!is_dirty(draft)) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (draft->baseline.record.revision == UINT64_MAX || draft->next_request_id == 0U) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiRecordSaveRequest candidate = {0};
    candidate.request_id = draft->next_request_id;
    candidate.proposed.previous_revision = draft->baseline.record.revision;
    candidate.proposed.revision = candidate.proposed.previous_revision + 1U;
    UmiRecordAmendment *m = &candidate.proposed.amendment;
    memcpy(m->entity_id, draft->baseline.record.amendment.entity_id, sizeof m->entity_id);
    memcpy(m->source, attribution->source, sizeof m->source);
    memcpy(m->actor, attribution->actor, sizeof m->actor);
    memcpy(m->reason, attribution->reason, sizeof m->reason);
    m->observed_at_nanoseconds = attribution->observed_at_nanoseconds;
    m->recorded_at_nanoseconds = attribution->recorded_at_nanoseconds;
    candidate.length = draft->length;
    memcpy(candidate.content, draft->content, candidate.length);
    UmiStatus status = UmiSha256Buffer(candidate.content, candidate.length, m->content_sha256);
    char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
    if (status == UMI_STATUS_OK) {
        status = umi_record_revision_format(&candidate.proposed, canonical, sizeof canonical);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    draft->pending = candidate;
    draft->saving = true;
    draft->next_request_id = candidate.request_id == UINT64_MAX ? 0U : candidate.request_id + 1U;
    *output = candidate;
    return UMI_STATUS_OK;
}

UmiStatus umi_record_save_request_commit(UmiDataServer *server,
    const UmiRecordSaveRequest *request, UmiRecordRevision *output)
{
    if (server == NULL || request == NULL || output == NULL || request->request_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (request->length > UMI_RECORD_DOCUMENT_MAX_BYTES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Validate before opening storage: document append intentionally computes
     * its own digest, whereas a prepared request must still match its capture. */
    char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
    UmiStatus status = umi_record_revision_format(&request->proposed, canonical, sizeof canonical);
    char digest[65];
    if (status == UMI_STATUS_OK) {
        status = UmiSha256Buffer(request->content, request->length, digest);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (strcmp(digest, request->proposed.amendment.content_sha256) != 0) {
        return UMI_STATUS_PARSE_ERROR;
    }
    return umi_record_document_append(server, &request->proposed.amendment,
        request->content, request->length, request->proposed.previous_revision, output);
}

UmiStatus umi_record_draft_finish_save(UmiRecordDraft *draft, uint64_t request_id,
                                      UmiStatus result, const UmiRecordRevision *saved)
{
    if (draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!draft->saving || request_id != draft->pending.request_id) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (result == UMI_STATUS_OK) {
        if (saved == NULL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        /* Compare the public canonical representation, never struct padding.
         * A wrong completion cannot grant a revision that was not requested. */
        char actual[UMI_RECORD_REVISION_TEXT_CAPACITY];
        char expected[UMI_RECORD_REVISION_TEXT_CAPACITY];
        UmiStatus status = umi_record_revision_format(saved, actual, sizeof actual);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = umi_record_revision_format(&draft->pending.proposed, expected, sizeof expected);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        if (strcmp(actual, expected) != 0) {
            return UMI_STATUS_INVALID_STATE;
        }
        memset(&draft->baseline, 0, sizeof draft->baseline);
        draft->baseline.record = *saved;
        draft->baseline.length = draft->pending.length;
        memcpy(draft->baseline.content, draft->pending.content, draft->pending.length);
    }
    /* Working bytes are deliberately untouched on success and failure. A write
     * can finish after further typing; only its captured bytes become baseline. */
    memset(&draft->pending, 0, sizeof draft->pending);
    draft->saving = false;
    return result;
}

/* Recovery extends the existing draft owner; keeping replacement here preserves
 * its dirty/pending guards and the lifetime of completion request identities. */
UmiStatus umi_record_draft_snapshot_validate(const UmiRecordDraftSnapshot *snapshot)
{
    if (snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (snapshot->length > UMI_RECORD_DOCUMENT_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (snapshot->baseline.record.revision != 0U) return document_valid(&snapshot->baseline);
    const UmiRecordAmendment *m = &snapshot->baseline.record.amendment;
    if (!umi_record_field_valid(m->entity_id, sizeof m->entity_id, false) ||
        snapshot->baseline.length != 0U || snapshot->baseline.record.previous_revision != 0U ||
        m->source[0] != '\0' || m->actor[0] != '\0' || m->reason[0] != '\0' ||
        m->content_sha256[0] != '\0' || m->observed_at_nanoseconds != 0U || m->recorded_at_nanoseconds != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus snapshot_copy_baseline(const UmiRecordDocument *input, UmiRecordDocument *output)
{
    memset(output, 0, sizeof *output);
    if (input->record.revision == 0U) {
        memcpy(output->record.amendment.entity_id, input->record.amendment.entity_id,
            strlen(input->record.amendment.entity_id) + 1U);
    } else {
        char canonical[UMI_RECORD_REVISION_TEXT_CAPACITY];
        UmiStatus status = umi_record_revision_format(&input->record, canonical, sizeof canonical);
        if (status == UMI_STATUS_OK) status = umi_record_revision_parse(canonical, strlen(canonical), &output->record);
        if (status != UMI_STATUS_OK) return status;
        output->length = input->length;
        memcpy(output->content, input->content, input->length);
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_record_draft_snapshot_capture(const UmiRecordDraft *draft, UmiRecordDraftSnapshot *output)
{
    if (draft == NULL || output == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (draft->saving) return UMI_STATUS_BUSY;
    if (!draft->ready) return UMI_STATUS_INVALID_STATE;
    UmiRecordDraftSnapshot *candidate = calloc(1U, sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = snapshot_copy_baseline(&draft->baseline, &candidate->baseline);
    if (status == UMI_STATUS_OK) {
        candidate->length = draft->length;
        memcpy(candidate->content, draft->content, draft->length);
        *output = *candidate;
    }
    free(candidate);
    return status;
}
UmiStatus umi_record_draft_snapshot_restore(UmiRecordDraft *draft, const UmiRecordDraftSnapshot *snapshot)
{
    UmiStatus status = can_replace(draft);
    if (status == UMI_STATUS_OK) status = umi_record_draft_snapshot_validate(snapshot);
    if (status != UMI_STATUS_OK) return status;
    UmiRecordDocument *baseline = calloc(1U, sizeof *baseline);
    if (baseline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = snapshot_copy_baseline(&snapshot->baseline, baseline);
    if (status == UMI_STATUS_OK) {
        draft->baseline = *baseline;
        memset(draft->content, 0, sizeof draft->content);
        memcpy(draft->content, snapshot->content, snapshot->length);
        draft->length = snapshot->length;
        draft->ready = true;
    }
    free(baseline);
    return status;
}

/* Reconciliation must publish content and baseline together. Work on an owned
 * copy so every failure preserves both the local draft and its revision token. */
UmiStatus umi_record_draft_reconcile_reviewed(UmiRecordDraft *draft,
    const UmiRecordDocument *reviewed, uint64_t expected_baseline,
    const void *expected_content, size_t expected_length,
    const void *replacement, size_t replacement_length)
{
    if (draft == NULL || reviewed == NULL || (expected_content == NULL && expected_length != 0U) ||
        (replacement == NULL && replacement_length != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (expected_length > UMI_RECORD_DOCUMENT_MAX_BYTES || replacement_length > UMI_RECORD_DOCUMENT_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (draft->saving) return UMI_STATUS_BUSY;
    if (!draft->ready || draft->length != expected_length ||
        (expected_length != 0U && memcmp(draft->content, expected_content, expected_length) != 0))
        return UMI_STATUS_INVALID_STATE;
    UmiRecordDraft *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *draft;
    UmiStatus status = umi_record_draft_adopt_reviewed(candidate, reviewed, expected_baseline);
    if (status == UMI_STATUS_OK) status = umi_record_draft_set_content(candidate, replacement, replacement_length);
    if (status == UMI_STATUS_OK) *draft = *candidate;
    free(candidate);
    return status;
}
