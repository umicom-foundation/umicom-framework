/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/types.c
 *
 * PURPOSE:
 *   Convert application experience enums to durable text and validate the
 *   identifiers shared by commands, fields, forms and state profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/types.h"

#include <ctype.h>
#include <string.h>

const char *umi_application_experience_value_type_text(UmiApplicationExperienceValueType type) {
  switch (type) {
  case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
    return "boolean";
  case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
    return "integer";
  case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL:
    return "decimal";
  case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
    return "text";
  default:
    return "none";
  }
}

const char *umi_application_experience_command_risk_text(UmiApplicationExperienceCommandRisk risk) {
  switch (risk) {
  case UMI_APPLICATION_EXPERIENCE_COMMAND_SAFE:
    return "safe";
  case UMI_APPLICATION_EXPERIENCE_COMMAND_CONFIRM:
    return "confirm";
  case UMI_APPLICATION_EXPERIENCE_COMMAND_RESTRICTED:
    return "restricted";
  default:
    return "unknown";
  }
}

const char *umi_application_experience_field_kind_text(UmiApplicationExperienceFieldKind kind) {
  static const char *const NAMES[] = {"unknown", "text",    "multiline", "boolean",
                                      "integer", "decimal", "choice",    "date",
                                      "time",    "search",  "secret"};
  return kind >= UMI_APPLICATION_EXPERIENCE_FIELD_TEXT &&
                 kind <= UMI_APPLICATION_EXPERIENCE_FIELD_SECRET
             ? NAMES[(size_t)kind]
             : NAMES[0];
}

const char *umi_application_experience_ui_state_text(UmiApplicationExperienceUiStateKind state) {
  static const char *const NAMES[] = {
      "unknown", "ready",   "empty", "loading", "busy",
      "success", "warning", "error", "offline", "permission-required"};
  return state >= UMI_APPLICATION_EXPERIENCE_UI_READY &&
                 state <= UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED
             ? NAMES[(size_t)state]
             : NAMES[0];
}

int umi_application_experience_identifier_valid(const char *identifier) {
  size_t index;
  size_t length;
  if (identifier == NULL || identifier[0] == '\0')
    return 0;
  length = strlen(identifier);
  if (length >= UMI_APPLICATION_EXPERIENCE_ID_CAPACITY || !isalnum((unsigned char)identifier[0]))
    return 0;
  for (index = 1U; index < length; ++index) {
    unsigned char character = (unsigned char)identifier[index];
    if (!isalnum(character) && character != '.' && character != '-' && character != '_')
      return 0;
  }
  return 1;
}
