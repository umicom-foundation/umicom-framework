/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_changeset.c
 *
 * PURPOSE:
 *   Compare form models and apply their typed differences forwards or in
 *   reverse while protecting the schema and revision boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/form_changeset.h"

#include <string.h>

#include "umicom/base/text.h"

/*
 * Initialise application experience form changeset from caller-provided values so later
 * operations receive a known state.
 */
/* Build changes in a local candidate after checking the borrowed schema;
 * an invalid field must not leave a partially published changeset. The prior implementation is retained for review. */
#if 0
UmiStatus
umi_application_experience_form_changeset_create(const UmiApplicationExperienceFormModel *before,
                                                 const UmiApplicationExperienceFormModel *after,
                                                 UmiApplicationExperienceFormChangeset *changeset) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (before == NULL || after == NULL || changeset == NULL || before->schema == NULL ||
      before->schema != after->schema)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(changeset, 0, sizeof(*changeset));
  changeset->base_revision = before->revision;
  changeset->target_revision = after->revision;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < before->schema->field_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (before->present[index] == after->present[index] &&
        (!before->present[index] ||
         umi_application_experience_value_equal(&before->values[index], &after->values[index])))
      continue;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (changeset->count >= UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    {
      UmiApplicationExperienceFieldChange *change = &changeset->changes[changeset->count++];
      UmiStatus status = umi_text_copy(change->field_id, sizeof(change->field_id),
                                       before->schema->fields[index].field_id);
      /* Preserve the original failure result so the caller can respond to the correct cause. */
      if (status != UMI_STATUS_OK)
        return status;
      change->before_present = before->present[index];
      change->after_present = after->present[index];
      change->before_value = before->values[index];
      change->after_value = after->values[index];
    }
  }
  return UMI_STATUS_OK;
}
#endif
UmiStatus
umi_application_experience_form_changeset_create(const UmiApplicationExperienceFormModel *before,
                                                 const UmiApplicationExperienceFormModel *after,
                                                 UmiApplicationExperienceFormChangeset *changeset) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (before == NULL || after == NULL || changeset == NULL || before->schema == NULL ||
      before->schema != after->schema)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus schema_status = umi_application_experience_form_schema_validate(before->schema);
  if (schema_status != UMI_STATUS_OK) return schema_status;
  if (after->revision < before->revision) return UMI_STATUS_INVALID_STATE;
  for (size_t i = 0U; i < before->schema->field_count; ++i) {
    if ((before->present[i] != 0 && before->present[i] != 1) ||
        (after->present[i] != 0 && after->present[i] != 1) ||
        (before->present[i] && umi_application_experience_value_validate(&before->values[i]) != UMI_STATUS_OK) ||
        (after->present[i] && umi_application_experience_value_validate(&after->values[i]) != UMI_STATUS_OK))
      return UMI_STATUS_INVALID_STATE;
  }
  UmiApplicationExperienceFormChangeset candidate;
  UmiApplicationExperienceFormChangeset *output = changeset;
  changeset = &candidate;
  (void)memset(changeset, 0, sizeof(*changeset));
  changeset->base_revision = before->revision;
  changeset->target_revision = after->revision;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < before->schema->field_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (before->present[index] == after->present[index] &&
        (!before->present[index] ||
         umi_application_experience_value_equal(&before->values[index], &after->values[index])))
      continue;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (changeset->count >= UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    {
      UmiApplicationExperienceFieldChange *change = &changeset->changes[changeset->count++];
      UmiStatus status = umi_text_copy(change->field_id, sizeof(change->field_id),
                                       before->schema->fields[index].field_id);
      /* Preserve the original failure result so the caller can respond to the correct cause. */
      if (status != UMI_STATUS_OK)
        return status;
      change->before_present = before->present[index];
      change->after_present = after->present[index];
      change->before_value = before->values[index];
      change->after_value = after->values[index];
    }
  }
  if (candidate.count != 0U && candidate.base_revision == candidate.target_revision)
    return UMI_STATUS_INVALID_STATE;
  *output = candidate;
  return UMI_STATUS_OK;
}

/*
 * Perform application experience form changeset through the module contract so client
 * applications do not duplicate its policy.
 */
/* Validate the entire changeset and captured values before publication.
 * This replaces partial application and rejects stale same-revision content. The prior implementation is retained for review. */
#if 0
UmiStatus umi_application_experience_form_changeset_apply(
    UmiApplicationExperienceFormModel *model,
    const UmiApplicationExperienceFormChangeset *changeset, int reverse) {
  size_t index;
  uint64_t expected_revision;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (model == NULL || model->schema == NULL || changeset == NULL ||
      (reverse != 0 && reverse != 1) ||
      changeset->count > UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  expected_revision = reverse ? changeset->target_revision : changeset->base_revision;
  /* Apply this branch only when its contract condition is satisfied. */
  if (model->revision != expected_revision)
    return UMI_STATUS_INVALID_STATE;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < changeset->count; ++index) {
    const UmiApplicationExperienceFieldChange *change = &changeset->changes[index];
    size_t field_index =
        umi_application_experience_form_schema_index(model->schema, change->field_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (field_index == SIZE_MAX)
      return UMI_STATUS_NOT_FOUND;
    model->present[field_index] = reverse ? change->before_present : change->after_present;
    model->values[field_index] = reverse ? change->before_value : change->after_value;
  }
  model->revision = reverse ? changeset->base_revision : changeset->target_revision;
  return UMI_STATUS_OK;
}
#endif
UmiStatus umi_application_experience_form_changeset_apply(
    UmiApplicationExperienceFormModel *model,
    const UmiApplicationExperienceFormChangeset *changeset, int reverse)
{
    if (model == NULL || model->schema == NULL || changeset == NULL ||
        (reverse != 0 && reverse != 1) || changeset->count > UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_application_experience_form_schema_validate(model->schema);
    if (status != UMI_STATUS_OK) return status;
    if (changeset->target_revision < changeset->base_revision ||
        (changeset->count != 0U && changeset->target_revision == changeset->base_revision) ||
        model->revision != (reverse ? changeset->target_revision : changeset->base_revision))
        return UMI_STATUS_INVALID_STATE;
    size_t indices[UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY];
    /* Validate every captured precondition and target value before applying
     * the first field. The same checks protect normal edits, undo and redo. */
    for (size_t i = 0U; i < changeset->count; ++i) {
        const UmiApplicationExperienceFieldChange *change = &changeset->changes[i];
        if (memchr(change->field_id, 0, sizeof change->field_id) == NULL ||
            (change->before_present != 0 && change->before_present != 1) ||
            (change->after_present != 0 && change->after_present != 1)) return UMI_STATUS_INVALID_ARGUMENT;
        indices[i] = umi_application_experience_form_schema_index(model->schema, change->field_id);
        if (indices[i] == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
        for (size_t j = 0U; j < i; ++j)
            if (indices[j] == indices[i]) return UMI_STATUS_ALREADY_EXISTS;
        const UmiApplicationExperienceField *field = &model->schema->fields[indices[i]];
        int from_present = reverse ? change->after_present : change->before_present;
        int to_present = reverse ? change->before_present : change->after_present;
        const UmiApplicationExperienceValue *from = reverse ? &change->after_value : &change->before_value;
        const UmiApplicationExperienceValue *to = reverse ? &change->before_value : &change->after_value;
        if (model->present[indices[i]] != from_present ||
            (from_present && (umi_application_experience_value_validate(from) != UMI_STATUS_OK ||
             umi_application_experience_value_validate(&model->values[indices[i]]) != UMI_STATUS_OK ||
             !umi_application_experience_value_equal(from, &model->values[indices[i]]))))
            return UMI_STATUS_INVALID_STATE;
        if (field->read_only) return UMI_STATUS_PERMISSION_DENIED;
        if (to_present) {
            status = umi_application_experience_field_accepts(field, to);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    for (size_t i = 0U; i < changeset->count; ++i) {
        const UmiApplicationExperienceFieldChange *change = &changeset->changes[i];
        model->present[indices[i]] = reverse ? change->before_present : change->after_present;
        model->values[indices[i]] = reverse ? change->before_value : change->after_value;
    }
    model->revision = reverse ? changeset->base_revision : changeset->target_revision;
    return UMI_STATUS_OK;
}

/*
 * Find application experience form changeset while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceFieldChange *
umi_application_experience_form_changeset_at(const UmiApplicationExperienceFormChangeset *changeset,
                                             size_t index) {
  return changeset != NULL && changeset->count <= UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY &&
                 index < changeset->count
             ? &changeset->changes[index]
             : NULL;
}
