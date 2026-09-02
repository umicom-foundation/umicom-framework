/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_schema.h
 *
 * PURPOSE:
 *   Group reusable fields into a versioned form schema that can drive dialogs,
 *   inspectors, settings pages, wizards and headless validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_SCHEMA_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_SCHEMA_H

#include "umicom/application/experience/field.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience form schema data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceFormSchema {
  uint32_t struct_size;
  uint32_t api_version;
  const char *form_id;
  const char *title;
  const char *description;
  const char *submit_command_id;
  const UmiApplicationExperienceField *fields;
  size_t field_count;
} UmiApplicationExperienceFormSchema;

/**
 * Check that application experience form schema satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_experience_form_schema_validate(const UmiApplicationExperienceFormSchema *schema);
/**
 * Provide the application experience form schema field operation used by this module and
 * its client applications.
 */
const UmiApplicationExperienceField *
umi_application_experience_form_schema_field(const UmiApplicationExperienceFormSchema *schema,
                                             const char *field_id);
/**
 * Provide the application experience form schema index operation used by this module and
 * its client applications.
 */
size_t
umi_application_experience_form_schema_index(const UmiApplicationExperienceFormSchema *schema,
                                             const char *field_id);

#ifdef __cplusplus
}
#endif

#endif
