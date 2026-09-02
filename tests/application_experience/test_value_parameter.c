/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_value_parameter.c
 *
 * PURPOSE:
 *   Verify typed values, bounded text, required parameters and type checking
 *   behave consistently before a command is invoked.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/experience.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  UmiApplicationExperienceValue value;
  UmiApplicationExperienceValue duplicate;
  UmiApplicationExperienceParameter parameter = {"document.name",
                                                 "Name",
                                                 "A name for the document.",
                                                 UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
                                                 1,
                                                 0,
                                                 {0}};
  assert(umi_application_experience_value_text(&value, "Roadmap") == UMI_STATUS_OK);
  duplicate = value;
  assert(umi_application_experience_value_equal(&value, &duplicate));
  assert(umi_application_experience_parameter_validate(&parameter) == UMI_STATUS_OK);
  assert(umi_application_experience_parameter_accepts(&parameter, &value) == UMI_STATUS_OK);
  umi_application_experience_value_clear(&value);
  assert(umi_application_experience_parameter_accepts(&parameter, &value) ==
         UMI_STATUS_INVALID_STATE);
  return 0;
}
