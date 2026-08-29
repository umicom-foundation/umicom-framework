/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_schema.c
 *
 * PURPOSE:
 *   Validate form schemas and resolve fields through stable identifiers rather
 *   than frontend widget addresses or application-specific indexes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/form_schema.h"

#include <string.h>

size_t
umi_application_experience_form_schema_index(const UmiApplicationExperienceFormSchema *schema,
                                             const char *field_id) {
  size_t index;
  if (schema == NULL || field_id == NULL ||
      schema->field_count > UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY)
    return SIZE_MAX;
  for (index = 0U; index < schema->field_count; ++index) {
    if (strcmp(schema->fields[index].field_id, field_id) == 0)
      return index;
  }
  return SIZE_MAX;
}

const UmiApplicationExperienceField *
umi_application_experience_form_schema_field(const UmiApplicationExperienceFormSchema *schema,
                                             const char *field_id) {
  size_t index = umi_application_experience_form_schema_index(schema, field_id);
  return index == SIZE_MAX ? NULL : &schema->fields[index];
}

UmiStatus
umi_application_experience_form_schema_validate(const UmiApplicationExperienceFormSchema *schema) {
  size_t index;
  if (schema == NULL || schema->title == NULL || schema->description == NULL ||
      schema->submit_command_id == NULL || schema->title[0] == '\0' ||
      !umi_application_experience_identifier_valid(schema->form_id) ||
      !umi_application_experience_identifier_valid(schema->submit_command_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (schema->struct_size < sizeof(*schema) ||
      schema->api_version != UMI_APPLICATION_EXPERIENCE_API_VERSION || schema->field_count == 0U ||
      schema->field_count > UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY || schema->fields == NULL)
    return UMI_STATUS_INVALID_STATE;
  for (index = 0U; index < schema->field_count; ++index) {
    size_t previous;
    if (umi_application_experience_field_validate(&schema->fields[index]) != UMI_STATUS_OK)
      return UMI_STATUS_INVALID_STATE;
    for (previous = 0U; previous < index; ++previous) {
      if (strcmp(schema->fields[previous].field_id, schema->fields[index].field_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}
