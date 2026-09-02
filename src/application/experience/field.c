/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/field.c
 *
 * PURPOSE:
 *   Validate form field metadata and typed values consistently before any
 *   frontend or domain service consumes user input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/field.h"

#include <string.h>

/*
 * Provide the application experience field option operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceFieldOption *
umi_application_experience_field_option(const UmiApplicationExperienceField *field,
                                        const char *option_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (field == NULL || option_id == NULL ||
      field->option_count > UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < field->option_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(field->options[index].option_id, option_id) == 0)
      return &field->options[index];
  }
  return NULL;
}

/*
 * Check that application experience field satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_experience_field_validate(const UmiApplicationExperienceField *field) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (field == NULL || field->label == NULL || field->help_text == NULL ||
      field->label[0] == '\0' || !umi_application_experience_identifier_valid(field->field_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (field->kind < UMI_APPLICATION_EXPERIENCE_FIELD_TEXT ||
      field->kind > UMI_APPLICATION_EXPERIENCE_FIELD_SECRET ||
      field->value_type < UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN ||
      field->value_type > UMI_APPLICATION_EXPERIENCE_VALUE_TEXT ||
      (field->required != 0 && field->required != 1) ||
      (field->read_only != 0 && field->read_only != 1) ||
      (field->enforce_number_range != 0 && field->enforce_number_range != 1) ||
      (field->enforce_length_range != 0 && field->enforce_length_range != 1) ||
      (field->enforce_number_range && field->minimum_number > field->maximum_number) ||
      (field->enforce_length_range && field->minimum_length > field->maximum_length) ||
      field->option_count > UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY ||
      (field->option_count > 0U && field->options == NULL))
    return UMI_STATUS_INVALID_STATE;
  /* Apply this branch only when its contract condition is satisfied. */
  if (field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE && field->option_count == 0U)
    return UMI_STATUS_INVALID_STATE;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < field->option_count; ++index) {
    size_t previous;
    const UmiApplicationExperienceFieldOption *option = &field->options[index];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (option->label == NULL || option->label[0] == '\0' ||
        !umi_application_experience_identifier_valid(option->option_id) ||
        (option->disabled != 0 && option->disabled != 1))
      return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Use the stable identifier comparison to choose the matching record or policy. */
      if (strcmp(field->options[previous].option_id, option->option_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (field->default_value.type != UMI_APPLICATION_EXPERIENCE_VALUE_NONE &&
      field->default_value.type != field->value_type)
    return UMI_STATUS_INVALID_STATE;
  return umi_application_experience_value_validate(&field->default_value);
}

/*
 * Provide the application experience field accepts operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_field_accepts(const UmiApplicationExperienceField *field,
                                                   const UmiApplicationExperienceValue *value) {
  double number;
  size_t length;
  UmiStatus status = umi_application_experience_field_validate(field);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status != UMI_STATUS_OK || value == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_experience_value_validate(value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Apply this branch only when its contract condition is satisfied. */
  if (umi_application_experience_value_empty(value))
    return field->required ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
  /* Apply this branch only when its contract condition is satisfied. */
  if (value->type != field->value_type)
    return UMI_STATUS_INVALID_STATE;
  /* Apply this branch only when its contract condition is satisfied. */
  if (value->type == UMI_APPLICATION_EXPERIENCE_VALUE_TEXT) {
    length = strlen(value->text_value);
    /* Apply this branch only when its contract condition is satisfied. */
    if (field->enforce_length_range &&
        (length < field->minimum_length || length > field->maximum_length))
      return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (field->kind == UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE) {
      const UmiApplicationExperienceFieldOption *option =
          umi_application_experience_field_option(field, value->text_value);
      /*
       * Protect caller-owned memory by checking that required state is available before it is
       * used.
       */
      if (option == NULL || option->disabled)
        return UMI_STATUS_INVALID_STATE;
    }
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (value->type == UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER ||
      value->type == UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL) {
    number = value->type == UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER ? (double)value->integer_value
                                                                     : value->decimal_value;
    /* Apply this branch only when its contract condition is satisfied. */
    if (field->enforce_number_range &&
        (number < field->minimum_number || number > field->maximum_number))
      return UMI_STATUS_INVALID_STATE;
  }
  return UMI_STATUS_OK;
}
