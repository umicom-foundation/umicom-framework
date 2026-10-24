/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Own editable saved-query drafts without tying them to GTK or a database. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_SEARCH_SESSION_H
#define UMICOM_DATA_STRUCTURED_FORM_SEARCH_SESSION_H
#include "umicom/data/record_draft_snapshot.h"
#include "umicom/data/structured_form_search_preset.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormSearchSession UmiStructuredFormSearchSession;
typedef struct UmiStructuredFormSearchSessionView {
  uint64_t generation, edit_revision;
  UmiRecordDraftState record;
  UmiStructuredFormSearchPreset preset;
} UmiStructuredFormSearchSessionView;
/* The schema is borrowed immutable data; the target namespace is copied.
 * It is a fixed query scope, not authorization. All handle calls are serialized
 * by one owner. Workers receive copied UmiRecordSaveRequest values only.
 * No I/O, callbacks or background work run on the session itself. */
UmiStatus umi_structured_form_search_session_create(const UmiStructuredFormSchema *schema,
                                                    const char *entity_prefix,
                                                    UmiStructuredFormSearchSession **out);
void umi_structured_form_search_session_destroy(UmiStructuredFormSearchSession *session);
/* Begin/open/restore refuse dirty or pending work. Entity IDs use form.search.
 * Begin creates an unsaved definition but performs no collision check.
 * Open validates document integrity and exact schema/scope before replacing.
 * Each successful replacement advances generation and starts edit revision 1.
 * Output and state remain unchanged on failure. */
UmiStatus umi_structured_form_search_session_begin(UmiStructuredFormSearchSession *session,
                                                   const char *entity_id,
                                                   const UmiStructuredFormSearchPreset *preset);
UmiStatus umi_structured_form_search_session_open(UmiStructuredFormSearchSession *session,
                                                  const UmiRecordDocument *document);
UmiStatus umi_structured_form_search_session_snapshot(const UmiStructuredFormSearchSession *session,
                                                      UmiStructuredFormSearchSessionView *out);
/* Both tokens bind an edit to the exact draft context. Accepted edits advance
 * edit_revision, even when canonical content is unchanged. Edits during saving
 * remain available; the immutable pending request never changes. */
UmiStatus umi_structured_form_search_session_edit(UmiStructuredFormSearchSession *session,
                                                  uint64_t generation, uint64_t edit_revision,
                                                  const UmiStructuredFormSearchPreset *preset);
UmiStatus umi_structured_form_search_session_discard(UmiStructuredFormSearchSession *session,
                                                     uint64_t generation, uint64_t edit_revision);
/* Prepare captures one dirty definition. Commit using record_save_request_commit
 * on the storage worker, then finish once on the owner. If dispatch fails,
 * finish with that failure status so the draft can retry. Destruction does not
 * cancel a running write; complete workers before destroying the handle. */
UmiStatus umi_structured_form_search_session_prepare_save(
    UmiStructuredFormSearchSession *session, uint64_t generation, uint64_t edit_revision,
    const UmiRecordDraftAttribution *attribution, UmiRecordSaveRequest *out);
UmiStatus umi_structured_form_search_session_finish_save(UmiStructuredFormSearchSession *session,
                                                         uint64_t request_id, UmiStatus result,
                                                         const UmiRecordRevision *saved);
/* Recovery copies both baseline and draft. Capture is BUSY while saving.
 * Restore validates both definitions against the fixed schema and namespace.
 * Before restoring against a database, call record_draft_snapshot_check_store.
 * A later save still checks the stored head. Plaintext recovery is not encrypted,
 * does not authorize access and cannot prove the identity of a database. */
UmiStatus umi_structured_form_search_session_capture(const UmiStructuredFormSearchSession *session,
                                                     UmiRecordDraftSnapshot *out);
UmiStatus umi_structured_form_search_session_restore(UmiStructuredFormSearchSession *session,
                                                     const UmiRecordDraftSnapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
