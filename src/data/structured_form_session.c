/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/structured_form_session.c
 * PURPOSE: Compose typed form ownership with the existing durable-draft protocol.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/structured_form_session.h"
#include "umicom/application/experience/structured_form_snapshot.h"
#include <stdlib.h>
#include <string.h>
/* The same owned session state is now shared with the conflict adapter through
 * a private header. Keep its original declaration for ownership review. */
#if 0
struct UmiStructuredFormSession {
  const UmiStructuredFormSchema *schema;
  UmiRecordDraft *draft;
  uint64_t generation;
  UmiStructuredForm current, baseline, pending;
};
#endif
#include "structured_form_session_internal.h"
static UmiStatus state(const UmiStructuredFormSession *session, UmiRecordDraftState *out) {
  return session == NULL ? UMI_STATUS_INVALID_ARGUMENT
                         : umi_record_draft_state(session->draft, out);
}
static UmiStatus replacement(const UmiStructuredFormSession *session) {
  UmiRecordDraftState current;
  UmiStatus status = state(session, &current);
  if (status != UMI_STATUS_OK)
    return status;
  if (current.saving)
    return UMI_STATUS_BUSY;
  if (current.dirty)
    return UMI_STATUS_INVALID_STATE;
  return session->generation == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
static UmiStatus reviewed(const UmiStructuredFormSession *session, uint64_t generation) {
  UmiRecordDraftState current;
  UmiStatus status = state(session, &current);
  if (status != UMI_STATUS_OK)
    return status;
  return current.ready && generation == session->generation ? UMI_STATUS_OK
                                                            : UMI_STATUS_INVALID_STATE;
}
UmiStatus umi_structured_form_session_create(const UmiStructuredFormSchema *schema,
                                             UmiStructuredFormSession **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormSession *candidate = calloc(1U, sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_record_draft_create(&candidate->draft);
  if (status != UMI_STATUS_OK) {
    free(candidate);
    return status;
  }
  candidate->schema = schema;
  *out = candidate;
  return UMI_STATUS_OK;
}
void umi_structured_form_session_destroy(UmiStructuredFormSession *session) {
  if (session != NULL) {
    umi_record_draft_destroy(session->draft);
    free(session);
  }
}
UmiStatus umi_structured_form_session_begin(UmiStructuredFormSession *session,
                                            const char *entity_id) {
  UmiStatus status = replacement(session);
  if (status != UMI_STATUS_OK)
    return status;
  if (entity_id == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < sizeof(((UmiRecordAmendment *)0)->entity_id) && entity_id[length] != '\0')
    ++length;
  if (length == 0U || length >= sizeof(((UmiRecordAmendment *)0)->entity_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredForm *form = malloc(sizeof(*form));
  UmiRecordDraftSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
  if (form == NULL || snapshot == NULL) {
    free(form);
    free(snapshot);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  status = umi_structured_form_init(session->schema, form);
  if (status == UMI_STATUS_OK) {
    /* New records remain unsaved even before the first answer is entered. */
    form->saved_revision = 0U;
    memcpy(snapshot->baseline.record.amendment.entity_id, entity_id, length + 1U);
    status = umi_structured_form_snapshot_encode(session->schema, form, snapshot->content,
                                                 sizeof(snapshot->content), &snapshot->length);
  }
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_restore(session->draft, snapshot);
  if (status == UMI_STATUS_OK) {
    session->current = *form;
    memset(&session->baseline, 0, sizeof(session->baseline));
    ++session->generation;
  }
  free(snapshot);
  free(form);
  return status;
}
UmiStatus umi_structured_form_session_open(UmiStructuredFormSession *session,
                                           const UmiRecordDocument *document) {
  UmiStatus status = replacement(session);
  if (status != UMI_STATUS_OK)
    return status;
  if (document == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (document->length > sizeof(document->content))
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStructuredForm *form = malloc(sizeof(*form));
  if (form == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_structured_form_snapshot_decode(session->schema, document->content, document->length,
                                               form);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_open(session->draft, document);
  if (status == UMI_STATUS_OK) {
    form->saved_revision = form->revision;
    session->current = session->baseline = *form;
    ++session->generation;
  }
  free(form);
  return status;
}
UmiStatus umi_structured_form_session_snapshot(const UmiStructuredFormSession *session,
                                               UmiStructuredFormSessionView *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDraftState current;
  UmiStatus status = state(session, &current);
  if (status != UMI_STATUS_OK)
    return status;
  /* Both components are already validated owned state; no failure after copy. */
  out->generation = session->generation;
  out->record = current;
  out->form = session->current;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_session_apply(UmiStructuredFormSession *session, uint64_t generation,
                                            const UmiStructuredFormEdit *edit) {
  UmiStatus status = reviewed(session, generation);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredForm *candidate = malloc(sizeof(*candidate));
  unsigned char *bytes = malloc(UMI_RECORD_DOCUMENT_MAX_BYTES);
  if (candidate == NULL || bytes == NULL) {
    free(candidate);
    free(bytes);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  *candidate = session->current;
  status = umi_structured_form_edit_apply(session->schema, candidate, edit);
  /* A no-op must not rewrite saved snapshot bookkeeping into a dirty record. */
  if (status == UMI_STATUS_OK && candidate->revision != session->current.revision) {
    size_t length;
    status = umi_structured_form_snapshot_encode(session->schema, candidate, bytes,
                                                 UMI_RECORD_DOCUMENT_MAX_BYTES, &length);
    if (status == UMI_STATUS_OK)
      status = umi_record_draft_set_content(session->draft, bytes, length);
    if (status == UMI_STATUS_OK)
      session->current = *candidate;
  }
  free(bytes);
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_session_discard(UmiStructuredFormSession *session,
                                              uint64_t generation) {
  UmiStatus status = reviewed(session, generation);
  if (status != UMI_STATUS_OK)
    return status;
  if (session->generation == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  status = umi_record_draft_discard(session->draft);
  if (status == UMI_STATUS_OK) {
    session->current = session->baseline;
    ++session->generation;
  }
  return status;
}
UmiStatus umi_structured_form_session_prepare_save(UmiStructuredFormSession *session,
                                                   uint64_t generation,
                                                   const UmiRecordDraftAttribution *attribution,
                                                   int submitting, UmiRecordSaveRequest *out) {
  UmiStatus status = reviewed(session, generation);
  if (status != UMI_STATUS_OK)
    return status;
  UmiApplicationExperienceValidationReport report;
  status = umi_structured_form_validate(session->schema, &session->current, submitting, &report);
  if (status == UMI_STATUS_OK && report.error_count != 0U)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_prepare_save(session->draft, attribution, out);
  if (status == UMI_STATUS_OK)
    session->pending = session->current;
  return status;
}
UmiStatus umi_structured_form_session_finish_save(UmiStructuredFormSession *session,
                                                  uint64_t request_id, UmiStatus result,
                                                  const UmiRecordRevision *saved) {
  if (session == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_draft_finish_save(session->draft, request_id, result, saved);
  if (status == UMI_STATUS_OK) {
    session->baseline = session->pending;
    session->baseline.saved_revision = session->baseline.revision;
    /* A newer draft stays dirty, but now knows which earlier revision reached
     * storage. A later response cannot mark that newer content as saved. */
    session->current.saved_revision = session->pending.revision;
    memset(&session->pending, 0, sizeof(session->pending));
  } else {
    UmiRecordDraftState current;
    if (umi_record_draft_state(session->draft, &current) == UMI_STATUS_OK && !current.saving)
      memset(&session->pending, 0, sizeof(session->pending));
  }
  return status;
}
UmiStatus umi_structured_form_session_capture(const UmiStructuredFormSession *session,
                                              UmiRecordDraftSnapshot *out) {
  if (session == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  return umi_record_draft_snapshot_capture(session->draft, out);
}
UmiStatus umi_structured_form_session_restore(UmiStructuredFormSession *session,
                                              const UmiRecordDraftSnapshot *snapshot) {
  UmiStatus status = replacement(session);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_record_draft_snapshot_validate(snapshot);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredForm *current = malloc(sizeof(*current)), *baseline = calloc(1U, sizeof(*baseline));
  if (current == NULL || baseline == NULL) {
    free(current);
    free(baseline);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  status = umi_structured_form_snapshot_decode(session->schema, snapshot->content, snapshot->length,
                                               current);
  if (status == UMI_STATUS_OK && snapshot->baseline.record.revision != 0U) {
    status = umi_structured_form_snapshot_decode(session->schema, snapshot->baseline.content,
                                                 snapshot->baseline.length, baseline);
    if (status == UMI_STATUS_OK && current->revision < baseline->revision)
      status = UMI_STATUS_INVALID_STATE;
    /* A valid capture never has different answers under the same revision.
     * Reject a forged or corrupted recovery pair before adopting either side. */
    if (status == UMI_STATUS_OK && current->revision == baseline->revision) {
      for (size_t i = 0U; i < current->count; ++i)
        if (!umi_form_answer_equal(&current->answers[i], &baseline->answers[i])) {
          status = UMI_STATUS_INVALID_STATE;
          break;
        }
    }
    if (status == UMI_STATUS_OK)
      baseline->saved_revision = baseline->revision;
  }
  if (status == UMI_STATUS_OK) {
    current->saved_revision = baseline->revision;
    status = umi_record_draft_snapshot_restore(session->draft, snapshot);
  }
  if (status == UMI_STATUS_OK) {
    session->current = *current;
    session->baseline = *baseline;
    ++session->generation;
  }
  free(baseline);
  free(current);
  return status;
}
