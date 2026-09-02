/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral values used by commands, forms, UI states
 *   and complete application experience profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_TYPES_H
#define UMICOM_APPLICATION_EXPERIENCE_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_EXPERIENCE_API_VERSION 1U
#define UMI_APPLICATION_EXPERIENCE_ID_CAPACITY 192U
#define UMI_APPLICATION_EXPERIENCE_TITLE_CAPACITY 256U
#define UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY 512U
#define UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY 16U
#define UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY 128U
#define UMI_APPLICATION_EXPERIENCE_PERMISSION_CAPACITY 64U
#define UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY 64U
#define UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY 64U
#define UMI_APPLICATION_EXPERIENCE_OPTION_CAPACITY 32U
#define UMI_APPLICATION_EXPERIENCE_ISSUE_CAPACITY 64U
#define UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY 64U

/**
 * List the named application experience value type values accepted by this public
 * contract.
 */
typedef enum UmiApplicationExperienceValueType {
  UMI_APPLICATION_EXPERIENCE_VALUE_NONE = 0,
  UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN = 1,
  UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER = 2,
  UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL = 3,
  UMI_APPLICATION_EXPERIENCE_VALUE_TEXT = 4
} UmiApplicationExperienceValueType;

/**
 * List the named application experience command risk values accepted by this public
 * contract.
 */
typedef enum UmiApplicationExperienceCommandRisk {
  UMI_APPLICATION_EXPERIENCE_COMMAND_SAFE = 1,
  UMI_APPLICATION_EXPERIENCE_COMMAND_CONFIRM = 2,
  UMI_APPLICATION_EXPERIENCE_COMMAND_RESTRICTED = 3
} UmiApplicationExperienceCommandRisk;

/**
 * List the named application experience command availability values accepted by this
 * public contract.
 */
typedef enum UmiApplicationExperienceCommandAvailability {
  UMI_APPLICATION_EXPERIENCE_COMMAND_HIDDEN = 1,
  UMI_APPLICATION_EXPERIENCE_COMMAND_DISABLED = 2,
  UMI_APPLICATION_EXPERIENCE_COMMAND_ENABLED = 3,
  UMI_APPLICATION_EXPERIENCE_COMMAND_BUSY = 4
} UmiApplicationExperienceCommandAvailability;

/**
 * List the named application experience field kind values accepted by this public
 * contract.
 */
typedef enum UmiApplicationExperienceFieldKind {
  UMI_APPLICATION_EXPERIENCE_FIELD_TEXT = 1,
  UMI_APPLICATION_EXPERIENCE_FIELD_MULTILINE = 2,
  UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN = 3,
  UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER = 4,
  UMI_APPLICATION_EXPERIENCE_FIELD_DECIMAL = 5,
  UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE = 6,
  UMI_APPLICATION_EXPERIENCE_FIELD_DATE = 7,
  UMI_APPLICATION_EXPERIENCE_FIELD_TIME = 8,
  UMI_APPLICATION_EXPERIENCE_FIELD_SEARCH = 9,
  UMI_APPLICATION_EXPERIENCE_FIELD_SECRET = 10
} UmiApplicationExperienceFieldKind;

/**
 * List the named application experience validation timing values accepted by this public
 * contract.
 */
typedef enum UmiApplicationExperienceValidationTiming {
  UMI_APPLICATION_EXPERIENCE_VALIDATE_MANUALLY = 1,
  UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_CHANGE = 2,
  UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_BLUR = 3,
  UMI_APPLICATION_EXPERIENCE_VALIDATE_ON_SUBMIT = 4
} UmiApplicationExperienceValidationTiming;

/**
 * List the named application experience ui state kind values accepted by this public
 * contract.
 */
typedef enum UmiApplicationExperienceUiStateKind {
  UMI_APPLICATION_EXPERIENCE_UI_READY = 1,
  UMI_APPLICATION_EXPERIENCE_UI_EMPTY = 2,
  UMI_APPLICATION_EXPERIENCE_UI_LOADING = 3,
  UMI_APPLICATION_EXPERIENCE_UI_BUSY = 4,
  UMI_APPLICATION_EXPERIENCE_UI_SUCCESS = 5,
  UMI_APPLICATION_EXPERIENCE_UI_WARNING = 6,
  UMI_APPLICATION_EXPERIENCE_UI_ERROR = 7,
  UMI_APPLICATION_EXPERIENCE_UI_OFFLINE = 8,
  UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED = 9
} UmiApplicationExperienceUiStateKind;

/**
 * List the named application experience density values accepted by this public contract.
 */
typedef enum UmiApplicationExperienceDensity {
  UMI_APPLICATION_EXPERIENCE_DENSITY_COMFORTABLE = 1,
  UMI_APPLICATION_EXPERIENCE_DENSITY_COMPACT = 2
} UmiApplicationExperienceDensity;

/**
 * List the named application experience issue severity values accepted by this public
 * contract.
 */
typedef enum UmiApplicationExperienceIssueSeverity {
  UMI_APPLICATION_EXPERIENCE_ISSUE_INFORMATION = 1,
  UMI_APPLICATION_EXPERIENCE_ISSUE_WARNING = 2,
  UMI_APPLICATION_EXPERIENCE_ISSUE_ERROR = 3
} UmiApplicationExperienceIssueSeverity;

/**
 * Provide the application experience value type text operation used by this module and its
 * client applications.
 */
const char *umi_application_experience_value_type_text(UmiApplicationExperienceValueType type);
/**
 * Provide the application experience command risk text operation used by this module and
 * its client applications.
 */
const char *umi_application_experience_command_risk_text(UmiApplicationExperienceCommandRisk risk);
/**
 * Provide the application experience field kind text operation used by this module and its
 * client applications.
 */
const char *umi_application_experience_field_kind_text(UmiApplicationExperienceFieldKind kind);
/**
 * Provide the application experience ui state text operation used by this module and its
 * client applications.
 */
const char *umi_application_experience_ui_state_text(UmiApplicationExperienceUiStateKind state);
/**
 * Check that application experience identifier satisfies its contract before another
 * service relies on it.
 */
int umi_application_experience_identifier_valid(const char *identifier);

#ifdef __cplusplus
}
#endif

#endif
