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

/*
 * Perform application experience form changeset through the module contract so client
 * applications do not duplicate its policy.
 */
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
