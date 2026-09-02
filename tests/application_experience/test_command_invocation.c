/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_command_invocation.c
 *
 * PURPOSE:
 *   Verify parameterised command registration and invocation validation,
 *   including required values and explicit confirmation.
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
  const UmiApplicationExperienceParameter parameters[] = {{"target.path",
                                                           "Target",
                                                           "The item to remove.",
                                                           UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
                                                           1,
                                                           0,
                                                           {0}}};
  const UmiApplicationExperienceCommand command = {sizeof(UmiApplicationExperienceCommand),
                                                   UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                   "umicom.command.target.remove",
                                                   "Remove target",
                                                   "Remove the selected target.",
                                                   "Edit",
                                                   "umicom.permission.target.remove",
                                                   UMI_APPLICATION_EXPERIENCE_COMMAND_CONFIRM,
                                                   UMI_APPLICATION_EXPERIENCE_COMMAND_CHANGES_DATA,
                                                   parameters,
                                                   1U};
  UmiApplicationExperienceCommandRegistry registry;
  UmiApplicationExperienceInvocation invocation;
  UmiApplicationExperienceValue value;
  umi_application_experience_command_registry_init(&registry);
  assert(umi_application_experience_command_registry_register(&registry, &command) ==
         UMI_STATUS_OK);
  assert(umi_application_experience_invocation_init(&invocation, command.command_id, "request.1",
                                                    "umicom.component.explorer") == UMI_STATUS_OK);
  assert(umi_application_experience_value_text(&value, "draft.txt") == UMI_STATUS_OK);
  assert(umi_application_experience_invocation_set(&invocation, "target.path", &value) ==
         UMI_STATUS_OK);
  assert(umi_application_experience_invocation_validate(&command, &invocation) ==
         UMI_STATUS_PERMISSION_DENIED);
  invocation.confirmed = 1;
  assert(umi_application_experience_invocation_validate(&command, &invocation) == UMI_STATUS_OK);
  return 0;
}
