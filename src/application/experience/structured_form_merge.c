/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Conflict resolution is a reviewed proposal. Keeping its input snapshots
 * separate from persistence prevents a UI choice becoming an implicit write. */
#include "umicom/application/experience/structured_form_merge.h"
#include "structured_form_internal.h"
#include <stdlib.h>
#include <string.h>
struct UmiStructuredFormMerge {
  const UmiStructuredFormSchema *schema;
  UmiStructuredForm baseline, local, incoming;
  uint64_t revision;
  UmiFormMergeDifference differences[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  UmiFormMergeChoice choices[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  UmiFormAnswer custom[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
};
static int answers_equal(const UmiStructuredForm *a, const UmiStructuredForm *b) {
  for (size_t i = 0; i < a->count; ++i)
    if (!umi_form_answer_equal(&a->answers[i], &b->answers[i]))
      return 0;
  return 1;
}
static int form_equal(const UmiStructuredForm *a, const UmiStructuredForm *b) {
  return a->revision == b->revision && a->saved_revision == b->saved_revision &&
         a->count == b->count && a->schema_version == b->schema_version &&
         strcmp(a->form_id, b->form_id) == 0 && answers_equal(a, b);
}
static const UmiFormAnswer *selected(const UmiStructuredFormMerge *review, size_t index) {
  switch (review->choices[index]) {
  case UMI_FORM_MERGE_KEEP_BASE:
    return &review->baseline.answers[index];
  case UMI_FORM_MERGE_KEEP_LOCAL:
    return &review->local.answers[index];
  case UMI_FORM_MERGE_TAKE_INCOMING:
    return &review->incoming.answers[index];
  case UMI_FORM_MERGE_CUSTOM:
    return &review->custom[index];
  case UMI_FORM_MERGE_AUTOMATIC:
    switch (review->differences[index]) {
    case UMI_FORM_MERGE_UNCHANGED:
      return &review->baseline.answers[index];
    case UMI_FORM_MERGE_LOCAL_ONLY:
      return &review->local.answers[index];
    case UMI_FORM_MERGE_INCOMING_ONLY:
    case UMI_FORM_MERGE_SAME_CHANGE:
      return &review->incoming.answers[index];
    case UMI_FORM_MERGE_CONFLICT:
      return NULL;
    }
  }
  return NULL;
}
UmiStatus umi_structured_form_merge_create(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *baseline,
                                           const UmiStructuredForm *local,
                                           const UmiStructuredForm *incoming,
                                           UmiStructuredFormMerge **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_draft_check(schema, baseline);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_draft_check(schema, local);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_draft_check(schema, incoming);
  if (status != UMI_STATUS_OK)
    return status;
  if (baseline->revision > local->revision || baseline->revision > incoming->revision ||
      (baseline->revision == local->revision && !answers_equal(baseline, local)) ||
      (baseline->revision == incoming->revision && !answers_equal(baseline, incoming)))
    return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormMerge *review = calloc(1, sizeof(*review));
  if (review == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  review->schema = schema;
  review->baseline = *baseline;
  review->local = *local;
  review->incoming = *incoming;
  review->revision = 1;
  for (size_t i = 0; i < baseline->count; ++i) {
    int local_change = !umi_form_answer_equal(&baseline->answers[i], &local->answers[i]);
    int incoming_change = !umi_form_answer_equal(&baseline->answers[i], &incoming->answers[i]);
    if (schema->base->fields[i].read_only && (local_change || incoming_change)) {
      free(review);
      return UMI_STATUS_PERMISSION_DENIED;
    }
    if (!local_change && !incoming_change)
      review->differences[i] = UMI_FORM_MERGE_UNCHANGED;
    else if (!incoming_change)
      review->differences[i] = UMI_FORM_MERGE_LOCAL_ONLY;
    else if (!local_change)
      review->differences[i] = UMI_FORM_MERGE_INCOMING_ONLY;
    else if (umi_form_answer_equal(&local->answers[i], &incoming->answers[i]))
      review->differences[i] = UMI_FORM_MERGE_SAME_CHANGE;
    else
      review->differences[i] = UMI_FORM_MERGE_CONFLICT;
  }
  *out = review;
  return UMI_STATUS_OK;
}
void umi_structured_form_merge_destroy(UmiStructuredFormMerge *review) { free(review); }
UmiStatus umi_structured_form_merge_state(const UmiStructuredFormMerge *review,
                                          UmiFormMergeState *out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiFormMergeState state = {review->revision,
                             review->baseline.revision,
                             review->local.revision,
                             review->incoming.revision,
                             review->baseline.count,
                             0,
                             0};
  for (size_t i = 0; i < state.field_count; ++i) {
    if (review->differences[i] == UMI_FORM_MERGE_CONFLICT)
      ++state.conflict_count;
    if (selected(review, i) == NULL)
      ++state.unresolved_count;
  }
  *out = state;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_merge_field(const UmiStructuredFormMerge *review, size_t index,
                                          UmiFormMergeField *out) {
  if (review == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= review->baseline.count)
    return UMI_STATUS_NOT_FOUND;
  UmiFormMergeField field = {0};
  const char *id = review->schema->base->fields[index].field_id;
  memcpy(field.field_id, id, strlen(id) + 1);
  field.difference = review->differences[index];
  field.choice = review->choices[index];
  field.baseline = review->baseline.answers[index];
  field.local = review->local.answers[index];
  field.incoming = review->incoming.answers[index];
  const UmiFormAnswer *value = selected(review, index);
  if (value != NULL) {
    field.resolved = 1;
    field.selected = *value;
  }
  *out = field;
  return UMI_STATUS_OK;
}
static UmiStatus selection_index(UmiStructuredFormMerge *review, uint64_t expected_revision,
                                 const char *field_id, size_t *index) {
  if (review == NULL || field_id == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (expected_revision != review->revision)
    return UMI_STATUS_INVALID_STATE;
  *index = umi_application_experience_form_schema_index(review->schema->base, field_id);
  if (*index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_merge_select(UmiStructuredFormMerge *review,
                                           uint64_t expected_revision, const char *field_id,
                                           UmiFormMergeChoice choice) {
  size_t index;
  UmiStatus status = selection_index(review, expected_revision, field_id, &index);
  if (status != UMI_STATUS_OK)
    return status;
  if (choice < UMI_FORM_MERGE_AUTOMATIC || choice >= UMI_FORM_MERGE_CUSTOM)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (review->choices[index] == choice)
    return UMI_STATUS_OK;
  if (review->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  review->choices[index] = choice;
  ++review->revision;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_merge_custom(UmiStructuredFormMerge *review,
                                           uint64_t expected_revision, const char *field_id,
                                           const UmiFormAnswer *answer) {
  size_t index;
  UmiStatus status = selection_index(review, expected_revision, field_id, &index);
  if (status != UMI_STATUS_OK)
    return status;
  if (review->schema->base->fields[index].read_only)
    return UMI_STATUS_PERMISSION_DENIED;
  status = umi_structured_form_answer_validate(review->schema, index, answer, 0);
  if (status != UMI_STATUS_OK)
    return status;
  if (review->choices[index] == UMI_FORM_MERGE_CUSTOM &&
      umi_form_answer_equal(&review->custom[index], answer))
    return UMI_STATUS_OK;
  if (review->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  review->custom[index] = *answer;
  review->choices[index] = UMI_FORM_MERGE_CUSTOM;
  ++review->revision;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_merge_check_inputs(const UmiStructuredFormMerge *review,
                                                 const UmiStructuredForm *baseline,
                                                 const UmiStructuredForm *local,
                                                 const UmiStructuredForm *incoming) {
  if (review == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_draft_check(review->schema, baseline);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_draft_check(review->schema, local);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_draft_check(review->schema, incoming);
  if (status != UMI_STATUS_OK)
    return status;
  return form_equal(&review->baseline, baseline) && form_equal(&review->local, local) &&
                 form_equal(&review->incoming, incoming)
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_STATE;
}
UmiStatus umi_structured_form_merge_finish(const UmiStructuredFormMerge *review,
                                           uint64_t expected_revision, int submitting,
                                           UmiStructuredForm *out) {
  if (review == NULL || out == NULL || (submitting != 0 && submitting != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (review->revision != expected_revision)
    return UMI_STATUS_INVALID_STATE;
  for (size_t i = 0; i < review->baseline.count; ++i)
    if (selected(review, i) == NULL)
      return UMI_STATUS_INVALID_STATE;
  UmiStructuredForm *form = malloc(sizeof(*form));
  UmiApplicationExperienceValidationReport *report = malloc(sizeof(*report));
  if (form == NULL || report == NULL) {
    free(form);
    free(report);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  *form = review->incoming;
  for (size_t i = 0; i < form->count; ++i)
    form->answers[i] = *selected(review, i);
  form->saved_revision = review->incoming.revision;
  UmiStatus status = UMI_STATUS_OK;
  if (!answers_equal(form, &review->incoming)) {
    uint64_t latest = review->local.revision > review->incoming.revision
                          ? review->local.revision
                          : review->incoming.revision;
    if (latest == UINT64_MAX)
      status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
      form->revision = latest + 1;
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_validate(review->schema, form, submitting, report);
  if (status == UMI_STATUS_OK && report->error_count != 0)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    *out = *form;
  free(form);
  free(report);
  return status;
}
