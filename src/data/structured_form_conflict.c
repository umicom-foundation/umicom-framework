/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Field decisions remain bound to both the storage baseline and selected draft.
 * Adoption is local; final storage concurrency checks remain in record save. */
#include "umicom/data/structured_form_conflict.h"
#include "structured_form_session_internal.h"
#include "umicom/application/experience/structured_form_snapshot.h"
#include <stdlib.h>
#include <string.h>
struct UmiStructuredFormConflict {
  const UmiStructuredFormSchema *schema;
  const UmiStructuredFormSession *owner;
  uint64_t generation;
  UmiStructuredFormMerge *merge;
  UmiRecordDraftSnapshot local;
  UmiRecordDocument incoming;
  UmiStructuredForm incoming_form;
};
static UmiStatus session_ready(const UmiStructuredFormSession *session, uint64_t generation,
                               UmiRecordDraftState *state) {
  if (session == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_draft_state(session->draft, state);
  if (status != UMI_STATUS_OK)
    return status;
  if (state->saving)
    return UMI_STATUS_BUSY;
  if (!state->ready || state->is_new || session->generation != generation)
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_session_review(const UmiStructuredFormSession *session,
                                             uint64_t expected_generation,
                                             const UmiRecordDocument *incoming,
                                             UmiStructuredFormConflict **out) {
  if (incoming == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDraftState state;
  UmiStatus status = session_ready(session, expected_generation, &state);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredFormConflict *review = calloc(1, sizeof(*review));
  if (review == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiRecordDraft *verified = NULL;
  status = umi_record_draft_create(&verified);
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_open(verified, incoming);
  umi_record_draft_destroy(verified);
  if (status == UMI_STATUS_OK &&
      (strcmp(state.entity_id, incoming->record.amendment.entity_id) != 0 ||
       incoming->record.revision < state.expected_revision))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_snapshot_decode(session->schema, incoming->content,
                                                 incoming->length, &review->incoming_form);
  if (status == UMI_STATUS_OK) {
    review->incoming_form.saved_revision = review->incoming_form.revision;
    status =
        umi_structured_form_merge_create(session->schema, &session->baseline, &session->current,
                                         &review->incoming_form, &review->merge);
  }
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_snapshot_capture(session->draft, &review->local);
  /* Equal storage revisions must mean the identical stored document. Treat a
   * changed payload at the same revision as inconsistency, not a new branch. */
  if (status == UMI_STATUS_OK && incoming->record.revision == state.expected_revision) {
    UmiRecordDraft *comparison = NULL;
    status = umi_record_draft_create(&comparison);
    if (status == UMI_STATUS_OK)
      status = umi_record_draft_open(comparison, &review->local.baseline);
    if (status == UMI_STATUS_OK)
      status = umi_record_draft_adopt_reviewed(comparison, incoming, state.expected_revision);
    umi_record_draft_destroy(comparison);
  }
  if (status == UMI_STATUS_OK) {
    review->schema = session->schema;
    review->owner = session;
    review->generation = session->generation;
    review->incoming = *incoming;
    *out = review;
  } else
    umi_structured_form_conflict_destroy(review);
  return status;
}
void umi_structured_form_conflict_destroy(UmiStructuredFormConflict *review) {
  if (review != NULL) {
    umi_structured_form_merge_destroy(review->merge);
    free(review);
  }
}
UmiStatus umi_structured_form_conflict_state(const UmiStructuredFormConflict *review,
                                             UmiFormMergeState *out) {
  return review == NULL ? UMI_STATUS_INVALID_ARGUMENT
                        : umi_structured_form_merge_state(review->merge, out);
}
UmiStatus umi_structured_form_conflict_field(const UmiStructuredFormConflict *review, size_t index,
                                             UmiFormMergeField *out) {
  return review == NULL ? UMI_STATUS_INVALID_ARGUMENT
                        : umi_structured_form_merge_field(review->merge, index, out);
}
UmiStatus umi_structured_form_conflict_select(UmiStructuredFormConflict *review,
                                              uint64_t expected_revision, const char *field_id,
                                              UmiFormMergeChoice choice) {
  return review == NULL
             ? UMI_STATUS_INVALID_ARGUMENT
             : umi_structured_form_merge_select(review->merge, expected_revision, field_id, choice);
}
UmiStatus umi_structured_form_conflict_custom(UmiStructuredFormConflict *review,
                                              uint64_t expected_revision, const char *field_id,
                                              const UmiFormAnswer *answer) {
  return review == NULL
             ? UMI_STATUS_INVALID_ARGUMENT
             : umi_structured_form_merge_custom(review->merge, expected_revision, field_id, answer);
}
UmiStatus umi_structured_form_conflict_preview(const UmiStructuredFormConflict *review,
                                               uint64_t expected_revision, int submitting,
                                               UmiStructuredForm *out) {
  return review == NULL
             ? UMI_STATUS_INVALID_ARGUMENT
             : umi_structured_form_merge_finish(review->merge, expected_revision, submitting, out);
}
static int answers_equal(const UmiStructuredForm *a, const UmiStructuredForm *b) {
  for (size_t i = 0; i < a->count; ++i)
    if (!umi_form_answer_equal(&a->answers[i], &b->answers[i]))
      return 0;
  return 1;
}
UmiStatus umi_structured_form_session_accept_review(UmiStructuredFormSession *session,
                                                    uint64_t expected_generation,
                                                    const UmiStructuredFormConflict *review,
                                                    uint64_t expected_review_revision,
                                                    int submitting) {
  if (review == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordDraftState state;
  UmiStatus status = session_ready(session, expected_generation, &state);
  if (status != UMI_STATUS_OK)
    return status;
  if (session != review->owner || session->schema != review->schema ||
      session->generation != review->generation)
    return UMI_STATUS_INVALID_STATE;
  if (session->generation == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  status = umi_structured_form_merge_check_inputs(review->merge, &session->baseline,
                                                  &session->current, &review->incoming_form);
  if (status != UMI_STATUS_OK)
    return status;
  UmiStructuredForm *candidate = malloc(sizeof(*candidate));
  unsigned char *bytes = malloc(UMI_RECORD_DOCUMENT_MAX_BYTES);
  if (candidate == NULL || bytes == NULL) {
    free(candidate);
    free(bytes);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  size_t length = 0;
  status = umi_structured_form_merge_finish(review->merge, expected_review_revision, submitting,
                                            candidate);
  if (status == UMI_STATUS_OK) {
    if (answers_equal(candidate, &review->incoming_form)) {
      /* Canonical saved bookkeeping may differ from historical encoded
       * bytes. Reuse the exact stored bytes when adopting incoming clean. */
      length = review->incoming.length;
      memcpy(bytes, review->incoming.content, length);
    } else
      status = umi_structured_form_snapshot_encode(session->schema, candidate, bytes,
                                                   UMI_RECORD_DOCUMENT_MAX_BYTES, &length);
  }
  if (status == UMI_STATUS_OK)
    status = umi_record_draft_reconcile_reviewed(
        session->draft, &review->incoming, review->local.baseline.record.revision,
        review->local.content, review->local.length, bytes, length);
  if (status == UMI_STATUS_OK) {
    session->baseline = review->incoming_form;
    session->current = *candidate;
    ++session->generation;
  }
  free(bytes);
  free(candidate);
  return status;
}
