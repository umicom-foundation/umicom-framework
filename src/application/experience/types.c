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

/*
 * Provide the application experience value type text operation used by this module and its
 * client applications.
 */
const char *umi_application_experience_value_type_text(UmiApplicationExperienceValueType type) {
  /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the application experience command risk text operation used by this module and
 * its client applications.
 */
const char *umi_application_experience_command_risk_text(UmiApplicationExperienceCommandRisk risk) {
  /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the application experience field kind text operation used by this module and its
 * client applications.
 */
const char *umi_application_experience_field_kind_text(UmiApplicationExperienceFieldKind kind) {
  static const char *const NAMES[] = {"unknown", "text",    "multiline", "boolean",
                                      "integer", "decimal", "choice",    "date",
                                      "time",    "search",  "secret"};
  return kind >= UMI_APPLICATION_EXPERIENCE_FIELD_TEXT &&
                 kind <= UMI_APPLICATION_EXPERIENCE_FIELD_SECRET
             ? NAMES[(size_t)kind]
             : NAMES[0];
}

/*
 * Provide the application experience ui state text operation used by this module and its
 * client applications.
 */
const char *umi_application_experience_ui_state_text(UmiApplicationExperienceUiStateKind state) {
  static const char *const NAMES[] = {
      "unknown", "ready",   "empty", "loading", "busy",
      "success", "warning", "error", "offline", "permission-required"};
  return state >= UMI_APPLICATION_EXPERIENCE_UI_READY &&
                 state <= UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED
             ? NAMES[(size_t)state]
             : NAMES[0];
}

/*
 * Check that application experience identifier satisfies its contract before another
 * service relies on it.
 */
int umi_application_experience_identifier_valid(const char *identifier) {
  size_t index;
  size_t length;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (identifier == NULL || identifier[0] == '\0')
    return 0;
  length = strlen(identifier);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (length >= UMI_APPLICATION_EXPERIENCE_ID_CAPACITY || !isalnum((unsigned char)identifier[0]))
    return 0;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 1U; index < length; ++index) {
    unsigned char character = (unsigned char)identifier[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (!isalnum(character) && character != '.' && character != '-' && character != '_')
      return 0;
  }
  return 1;
}
