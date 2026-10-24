/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/structured_form_session.h
 * PURPOSE: Own typed form edits and immutable record save requests in one session.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_STRUCTURED_FORM_SESSION_H
#define UMICOM_DATA_STRUCTURED_FORM_SESSION_H
#include "umicom/application/experience/structured_form_edit.h"
#include "umicom/data/record_draft_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormSession UmiStructuredFormSession;
typedef struct UmiStructuredFormSessionView {
  uint64_t generation;
  UmiRecordDraftState record;
  UmiStructuredForm form;
} UmiStructuredFormSessionView;
/* The schema is borrowed and immutable until destruction. Calls belong to one
 * serialized owner. Workers use copied UmiRecordSaveRequest values only.
 * No network, database, callbacks or background threads run on this handle. */
UmiStatus umi_structured_form_session_create(const UmiStructuredFormSchema *schema,
                                             UmiStructuredFormSession **out);
/* Complete outstanding workers and disconnect their replies before destroy.
 * Destroying the handle does not cancel a write already running elsewhere. */
void umi_structured_form_session_destroy(UmiStructuredFormSession *session);
/* Begin/open never replace unsaved changes or a pending save. Open verifies the
 * document's digest and matching form schema. The application obtains an exact
 * reviewed document using record_document_open_reviewed before calling open. */
UmiStatus umi_structured_form_session_begin(UmiStructuredFormSession *session,
                                            const char *entity_id);
UmiStatus umi_structured_form_session_open(UmiStructuredFormSession *session,
                                           const UmiRecordDocument *document);
UmiStatus umi_structured_form_session_snapshot(const UmiStructuredFormSession *session,
                                               UmiStructuredFormSessionView *out);
/* Generation binds a command to the selected record, including reopen/discard.
 * The edit also carries its form revision and previous answers. A stale context
 * can never become valid merely because another record uses the same schema. */
UmiStatus umi_structured_form_session_apply(UmiStructuredFormSession *session,
                                            uint64_t expected_generation,
                                            const UmiStructuredFormEdit *edit);
/* Explicit user decision. Restores the saved baseline, or closes a never-saved
 * new record. Advances generation, preserves worker-token monotonicity, and
 * refuses while saving. No database write is performed. */
UmiStatus umi_structured_form_session_discard(UmiStructuredFormSession *session,
                                              uint64_t expected_generation);
/* Preparation captures the current bytes; editing may continue. A second save
 * is BUSY. Call record_save_request_commit on the worker, then finish below on
 * this session's owner. Failure preserves the local draft for a reviewed retry.
 * submitting=1 checks required answers; draft saves use submitting=0. */
UmiStatus umi_structured_form_session_prepare_save(UmiStructuredFormSession *session,
                                                   uint64_t expected_generation,
                                                   const UmiRecordDraftAttribution *attribution,
                                                   int submitting, UmiRecordSaveRequest *out);
UmiStatus umi_structured_form_session_finish_save(UmiStructuredFormSession *session,
                                                  uint64_t request_id, UmiStatus result,
                                                  const UmiRecordRevision *saved);
/* Recovery snapshots use the existing framed, digest-checked draft codec. Check
 * the stored baseline with record_draft_snapshot_check_store before restoring
 * against a chosen database. These APIs do not silently reconcile newer heads. */
UmiStatus umi_structured_form_session_capture(const UmiStructuredFormSession *session,
                                              UmiRecordDraftSnapshot *out);
UmiStatus umi_structured_form_session_restore(UmiStructuredFormSession *session,
                                              const UmiRecordDraftSnapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
