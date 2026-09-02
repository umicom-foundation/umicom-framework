/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/parameter.h
 *
 * PURPOSE:
 *   Describe typed command parameters, defaults and required values in a
 *   frontend-neutral form that command palettes and dialogs can share.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_PARAMETER_H
#define UMICOM_APPLICATION_EXPERIENCE_PARAMETER_H

#include "umicom/application/experience/value.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience parameter data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceParameter {
  const char *parameter_id;
  const char *label;
  const char *description;
  UmiApplicationExperienceValueType value_type;
  int required;
  int secret;
  UmiApplicationExperienceValue default_value;
} UmiApplicationExperienceParameter;

/**
 * Check that application experience parameter satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_experience_parameter_validate(const UmiApplicationExperienceParameter *parameter);
/**
 * Provide the application experience parameter accepts operation used by this module and
 * its client applications.
 */
UmiStatus
umi_application_experience_parameter_accepts(const UmiApplicationExperienceParameter *parameter,
                                             const UmiApplicationExperienceValue *value);

#ifdef __cplusplus
}
#endif

#endif
