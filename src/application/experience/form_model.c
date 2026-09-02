/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_model.c
 *
 * PURPOSE:
 *   Initialise, edit and query form data while preserving typed values,
 *   revision tracking and an explicit saved boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/form_model.h"

#include <string.h>

/*
 * Initialise application experience form model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus
umi_application_experience_form_model_init(UmiApplicationExperienceFormModel *model,
                                           const UmiApplicationExperienceFormSchema *schema) {
  size_t index;
  UmiStatus status = umi_application_experience_form_schema_validate(schema);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status != UMI_STATUS_OK || model == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(model, 0, sizeof(*model));
  model->schema = schema;
  model->revision = 1U;
  model->saved_revision = 1U;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < schema->field_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (schema->fields[index].default_value.type != UMI_APPLICATION_EXPERIENCE_VALUE_NONE) {
      model->values[index] = schema->fields[index].default_value;
      model->present[index] = 1;
    }
  }
  return UMI_STATUS_OK;
}

/*
 * Copy application experience form model into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_experience_form_model_set(UmiApplicationExperienceFormModel *model,
                                                    const char *field_id,
                                                    const UmiApplicationExperienceValue *value) {
  size_t index;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (model == NULL || model->schema == NULL || value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  index = umi_application_experience_form_schema_index(model->schema, field_id);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (model->schema->fields[index].read_only)
    return UMI_STATUS_PERMISSION_DENIED;
  status = umi_application_experience_field_accepts(&model->schema->fields[index], value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (!model->present[index] ||
      !umi_application_experience_value_equal(&model->values[index], value)) {
    model->values[index] = *value;
    model->present[index] = 1;
    model->revision += 1U;
  }
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience form model get operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceValue *
umi_application_experience_form_model_get(const UmiApplicationExperienceFormModel *model,
                                          const char *field_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (model == NULL || model->schema == NULL)
    return NULL;
  index = umi_application_experience_form_schema_index(model->schema, field_id);
  return index == SIZE_MAX || !model->present[index] ? NULL : &model->values[index];
}

/*
 * Provide the application experience form model mark saved operation used by this module
 * and its client applications.
 */
void umi_application_experience_form_model_mark_saved(UmiApplicationExperienceFormModel *model) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (model != NULL)
    model->saved_revision = model->revision;
}

/*
 * Provide the application experience form model dirty operation used by this module and
 * its client applications.
 */
int umi_application_experience_form_model_dirty(const UmiApplicationExperienceFormModel *model) {
  return model != NULL && model->revision != model->saved_revision;
}
