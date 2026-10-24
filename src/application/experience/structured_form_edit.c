/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_edit.c
 * PURPOSE: Publish form edits only after complete validation of captured state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/structured_form_edit.h"
#include "structured_form_internal.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus edit_check(const UmiStructuredFormSchema *schema,
                            const UmiStructuredFormEdit *edit) {
  if (edit == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (memchr(edit->form_id, '\0', sizeof(edit->form_id)) == NULL ||
      strcmp(edit->form_id, schema->base->form_id) != 0 ||
      edit->schema_version != schema->version || edit->base_revision == 0U ||
      edit->count > schema->rule_count)
    return UMI_STATUS_INVALID_STATE;
  for (size_t i = 0U; i < edit->count; ++i) {
    const UmiStructuredFormChange *change = &edit->changes[i];
    if (memchr(change->field_id, '\0', sizeof(change->field_id)) == NULL)
      return UMI_STATUS_INVALID_STATE;
    size_t index = umi_application_experience_form_schema_index(schema->base, change->field_id);
    if (index == SIZE_MAX)
      return UMI_STATUS_NOT_FOUND;
    if (schema->base->fields[index].read_only)
      return UMI_STATUS_PERMISSION_DENIED;
    if (umi_structured_form_answer_validate(schema, index, &change->before, 0) != UMI_STATUS_OK ||
        umi_structured_form_answer_validate(schema, index, &change->after, 0) != UMI_STATUS_OK ||
        umi_form_answer_equal(&change->before, &change->after))
      return UMI_STATUS_INVALID_STATE;
    for (size_t j = 0U; j < i; ++j)
      if (strcmp(change->field_id, edit->changes[j].field_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
  }
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_edit_capture(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *before,
                                           const UmiStructuredForm *after,
                                           UmiStructuredFormEdit *out) {
  UmiStatus status = umi_structured_form_draft_check(schema, before);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_structured_form_draft_check(schema, after);
  if (status != UMI_STATUS_OK)
    return status;
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (after->revision < before->revision)
    return UMI_STATUS_INVALID_STATE;
  UmiStructuredFormEdit *candidate = calloc(1U, sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(candidate->form_id, before->form_id, sizeof(candidate->form_id));
  candidate->schema_version = schema->version;
  candidate->base_revision = before->revision;
  for (size_t i = 0U; i < before->count; ++i) {
    if (umi_form_answer_equal(&before->answers[i], &after->answers[i]))
      continue;
    UmiStructuredFormChange *change = &candidate->changes[candidate->count++];
    memcpy(change->field_id, schema->base->fields[i].field_id,
           strlen(schema->base->fields[i].field_id) + 1U);
    change->before = before->answers[i];
    change->after = after->answers[i];
  }
  status = edit_check(schema, candidate);
  if (candidate->count != 0U && before->revision == after->revision)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    *out = *candidate;
  free(candidate);
  return status;
}
UmiStatus umi_structured_form_edit_apply(const UmiStructuredFormSchema *schema,
                                         UmiStructuredForm *form,
                                         const UmiStructuredFormEdit *edit) {
  UmiStatus status = umi_structured_form_draft_check(schema, form);
  if (status != UMI_STATUS_OK)
    return status;
  status = edit_check(schema, edit);
  if (status != UMI_STATUS_OK)
    return status;
  if (form->revision != edit->base_revision)
    return UMI_STATUS_INVALID_STATE;
  if (edit->count == 0U)
    return UMI_STATUS_OK;
  if (form->revision == UINT64_MAX)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  size_t indices[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  for (size_t i = 0U; i < edit->count; ++i) {
    indices[i] =
        umi_application_experience_form_schema_index(schema->base, edit->changes[i].field_id);
    if (!umi_form_answer_equal(&form->answers[indices[i]], &edit->changes[i].before))
      return UMI_STATUS_INVALID_STATE;
  }
  /* No fallible work follows this boundary. A bad later field cannot publish
   * half an edit, and view notification belongs to the caller after return. */
  for (size_t i = 0U; i < edit->count; ++i)
    form->answers[indices[i]] = edit->changes[i].after;
  ++form->revision;
  return UMI_STATUS_OK;
}
UmiStatus umi_structured_form_edit_inverse(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *form,
                                           const UmiStructuredFormEdit *applied,
                                           UmiStructuredFormEdit *out) {
  UmiStatus status = umi_structured_form_draft_check(schema, form);
  if (status != UMI_STATUS_OK)
    return status;
  status = edit_check(schema, applied);
  if (status != UMI_STATUS_OK)
    return status;
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (applied->base_revision == UINT64_MAX ||
      form->revision != applied->base_revision + (applied->count != 0U ? 1U : 0U))
    return UMI_STATUS_INVALID_STATE;
  for (size_t i = 0U; i < applied->count; ++i) {
    size_t index =
        umi_application_experience_form_schema_index(schema->base, applied->changes[i].field_id);
    if (!umi_form_answer_equal(&form->answers[index], &applied->changes[i].after))
      return UMI_STATUS_INVALID_STATE;
  }
  UmiStructuredFormEdit *candidate = malloc(sizeof(*candidate));
  if (candidate == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  *candidate = *applied;
  candidate->base_revision = form->revision;
  for (size_t i = 0U; i < applied->count; ++i) {
    candidate->changes[i].before = applied->changes[i].after;
    candidate->changes[i].after = applied->changes[i].before;
  }
  *out = *candidate;
  free(candidate);
  return UMI_STATUS_OK;
}
