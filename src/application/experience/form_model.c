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

UmiStatus
umi_application_experience_form_model_init(UmiApplicationExperienceFormModel *model,
                                           const UmiApplicationExperienceFormSchema *schema) {
  size_t index;
  UmiStatus status = umi_application_experience_form_schema_validate(schema);
  if (status != UMI_STATUS_OK || model == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(model, 0, sizeof(*model));
  model->schema = schema;
  model->revision = 1U;
  model->saved_revision = 1U;
  for (index = 0U; index < schema->field_count; ++index) {
    if (schema->fields[index].default_value.type != UMI_APPLICATION_EXPERIENCE_VALUE_NONE) {
      model->values[index] = schema->fields[index].default_value;
      model->present[index] = 1;
    }
  }
  return UMI_STATUS_OK;
}

UmiStatus umi_application_experience_form_model_set(UmiApplicationExperienceFormModel *model,
                                                    const char *field_id,
                                                    const UmiApplicationExperienceValue *value) {
  size_t index;
  UmiStatus status;
  if (model == NULL || model->schema == NULL || value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  index = umi_application_experience_form_schema_index(model->schema, field_id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  if (model->schema->fields[index].read_only)
    return UMI_STATUS_PERMISSION_DENIED;
  status = umi_application_experience_field_accepts(&model->schema->fields[index], value);
  if (status != UMI_STATUS_OK)
    return status;
  if (!model->present[index] ||
      !umi_application_experience_value_equal(&model->values[index], value)) {
    model->values[index] = *value;
    model->present[index] = 1;
    model->revision += 1U;
  }
  return UMI_STATUS_OK;
}

const UmiApplicationExperienceValue *
umi_application_experience_form_model_get(const UmiApplicationExperienceFormModel *model,
                                          const char *field_id) {
  size_t index;
  if (model == NULL || model->schema == NULL)
    return NULL;
  index = umi_application_experience_form_schema_index(model->schema, field_id);
  return index == SIZE_MAX || !model->present[index] ? NULL : &model->values[index];
}

void umi_application_experience_form_model_mark_saved(UmiApplicationExperienceFormModel *model) {
  if (model != NULL)
    model->saved_revision = model->revision;
}

int umi_application_experience_form_model_dirty(const UmiApplicationExperienceFormModel *model) {
  return model != NULL && model->revision != model->saved_revision;
}
