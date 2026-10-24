/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/record_draft.h
 * PURPOSE: Preserve editable record content while an immutable save is running.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_RECORD_DRAFT_H
#define UMICOM_DATA_RECORD_DRAFT_H
#include "umicom/data/record_document.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRecordDraft UmiRecordDraft;

/* Attribution is supplied for each save. Entity identity, predecessor revision
 * and content digest come from the draft, not from an editable form field.
 * Labels describe provenance; the application must authorize the operation. */
typedef struct UmiRecordDraftAttribution {
    char source[160];
    char actor[128];
    char reason[256];
    uint64_t observed_at_nanoseconds;
    uint64_t recorded_at_nanoseconds;
} UmiRecordDraftAttribution;

typedef struct UmiRecordDraftState {
    bool ready;
    bool dirty;
    bool saving;
    bool is_new;
    char entity_id[64];
    uint64_t expected_revision;
    uint64_t pending_request_id;
    size_t length;
} UmiRecordDraftState;

/* This owned value can be copied to a worker. Keep it unchanged until that
 * worker finishes. Use umi_record_save_request_commit to validate and commit
 * the captured request on that worker. Return its committed revision
 * and status to finish_save on the originating draft's owning thread.
 * request_id is scoped to the lifetime of that draft and is never reused there. */
typedef struct UmiRecordSaveRequest {
    uint64_t request_id;
    UmiRecordRevision proposed;
    size_t length;
    unsigned char content[UMI_RECORD_DOCUMENT_MAX_BYTES];
} UmiRecordSaveRequest;

/* Draft calls are serialized by the caller. No database access, callbacks or
 * background tasks run here. Workers use copied requests, never this handle.
 * Destruction does not cancel an outstanding database write. Complete workers
 * and disconnect their replies before destroying or replacing the handle.
 * Memory is ordinary process memory, not encrypted or securely erased. */
UmiStatus umi_record_draft_create(UmiRecordDraft **output);
void umi_record_draft_destroy(UmiRecordDraft *draft);

/* Begin creates an unsaved, possibly empty document; it does not check whether
 * the entity already exists. Open copies and validates a previously read
 * document. Neither operation silently replaces dirty content. Discard is an
 * explicit caller decision: restore a saved baseline, or close an unsaved new
 * document. Begin/open/discard return BUSY during an outstanding save.
 * All other errors leave the draft and output arguments unchanged. */
UmiStatus umi_record_draft_begin(UmiRecordDraft *draft, const char *entity_id);
UmiStatus umi_record_draft_open(UmiRecordDraft *draft, const UmiRecordDocument *document);
UmiStatus umi_record_draft_discard(UmiRecordDraft *draft);
UmiStatus umi_record_draft_state(const UmiRecordDraft *draft, UmiRecordDraftState *output);
UmiStatus umi_record_draft_set_content(UmiRecordDraft *draft, const void *content, size_t length);
/* Copies arbitrary bytes without appending a text terminator. NULL content is
 * allowed only for zero length. An insufficient capacity changes neither the
 * destination nor output_length. State.length can be used to size a buffer. */
UmiStatus umi_record_draft_copy_content(const UmiRecordDraft *draft, void *content,
                                       size_t capacity, size_t *output_length);

/* After an explicit comparison decision, adopt a reviewed stored baseline while
 * retaining every current draft byte. This does not merge or save content.
 * The expected_baseline must still equal the draft's current baseline revision;
 * entity identity must match and the reviewed revision must not go backwards.
 * An equal revision with different content or metadata is rejected as a store
 * inconsistency. All failures leave the draft unchanged. Pending saves are BUSY.
 * A subsequent writer can still invalidate the next save; storage checks again. */
UmiStatus umi_record_draft_adopt_reviewed(UmiRecordDraft *draft,
    const UmiRecordDocument *reviewed, uint64_t expected_baseline);

/* Requires an open dirty draft and valid attribution. A successful preparation
 * captures the exact content and reviewed revision. Editing remains available
 * while saving; a second preparation is BUSY. A caller that cannot dispatch
 * its request must finish it with a non-OK result so the draft can retry. */
UmiStatus umi_record_draft_prepare_save(UmiRecordDraft *draft,
    const UmiRecordDraftAttribution *attribution, UmiRecordSaveRequest *output);

/* Storage worker entry point: verifies the captured metadata, nonzero request
 * ID and content digest before calling atomic document append. It never accesses
 * a draft or changes request bytes. Keep server and request alive until return.
 * The output is unchanged on failure. Validation detects accidental edits to a
 * captured request; it is not authorization or protection against a forger. */
UmiStatus umi_record_save_request_commit(UmiDataServer *server,
    const UmiRecordSaveRequest *request, UmiRecordRevision *output);

/* Call exactly once when the worker has finished. OK requires a committed
 * revision matching the captured request. Wrong tokens and mismatched success
 * metadata leave the pending request intact. A valid non-OK completion clears
 * pending state, retains content and baseline, and returns that worker status.
 * Successful completion advances the baseline to the captured bytes and keeps
 * the current draft bytes, including edits made after preparation.
 *
 * On a stale save, copy the local draft for comparison, load the latest record,
 * and let the user decide how to reconcile it. On IO_ERROR, verify persistence
 * before retrying: failure can be ambiguous after a storage interruption.
 * Never report CANCELLED merely because a window closed while I/O still runs. */
UmiStatus umi_record_draft_finish_save(UmiRecordDraft *draft, uint64_t request_id,
                                      UmiStatus result, const UmiRecordRevision *saved);
#ifdef __cplusplus
}
#endif
#endif
