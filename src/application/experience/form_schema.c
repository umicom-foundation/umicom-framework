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

/*
 * Provide the application experience form schema index operation used by this module and
 * its client applications.
 */
size_t
umi_application_experience_form_schema_index(const UmiApplicationExperienceFormSchema *schema,
                                             const char *field_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (schema == NULL || field_id == NULL ||
      schema->field_count > UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY)
    return SIZE_MAX;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < schema->field_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(schema->fields[index].field_id, field_id) == 0)
      return index;
  }
  return SIZE_MAX;
}

/*
 * Provide the application experience form schema field operation used by this module and
 * its client applications.
 */
const UmiApplicationExperienceField *
umi_application_experience_form_schema_field(const UmiApplicationExperienceFormSchema *schema,
                                             const char *field_id) {
  size_t index = umi_application_experience_form_schema_index(schema, field_id);
  return index == SIZE_MAX ? NULL : &schema->fields[index];
}

/*
 * Check that application experience form schema satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_experience_form_schema_validate(const UmiApplicationExperienceFormSchema *schema) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (schema == NULL || schema->title == NULL || schema->description == NULL ||
      schema->submit_command_id == NULL || schema->title[0] == '\0' ||
      !umi_application_experience_identifier_valid(schema->form_id) ||
      !umi_application_experience_identifier_valid(schema->submit_command_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (schema->struct_size < sizeof(*schema) ||
      schema->api_version != UMI_APPLICATION_EXPERIENCE_API_VERSION || schema->field_count == 0U ||
      schema->field_count > UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY || schema->fields == NULL)
    return UMI_STATUS_INVALID_STATE;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < schema->field_count; ++index) {
    size_t previous;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_application_experience_field_validate(&schema->fields[index]) != UMI_STATUS_OK)
      return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Keep the operation inside its valid bounds before reading, writing or adding data. */
      if (strcmp(schema->fields[previous].field_id, schema->fields[index].field_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}
