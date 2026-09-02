/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_dispatcher.c
 *
 * PURPOSE:
 *   Verify a validated and enabled command reaches its handler exactly once
 *   and returns a correlated completion sequence.
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
 * Exercise handle refresh and return a clear result when the behaviour no longer matches
 * its contract.
 */
static UmiStatus handle_refresh(const UmiApplicationExperienceInvocation *invocation,
                                UmiApplicationExperienceCommandResult *result, void *user_data) {
  int *calls = user_data;
  assert(invocation != NULL);
  *calls += 1;
  result->message[0] = 'O';
  result->message[1] = 'K';
  result->message[2] = '\0';
  return UMI_STATUS_OK;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
  const UmiApplicationExperienceCommand command = {sizeof(UmiApplicationExperienceCommand),
                                                   UMI_APPLICATION_EXPERIENCE_API_VERSION,
                                                   "umicom.command.refresh",
                                                   "Refresh",
                                                   "Refresh the current view.",
                                                   "View",
                                                   NULL,
                                                   UMI_APPLICATION_EXPERIENCE_COMMAND_SAFE,
                                                   0U,
                                                   NULL,
                                                   0U};
  UmiApplicationExperienceCommandRegistry registry;
  UmiApplicationExperienceDispatcher dispatcher;
  UmiApplicationExperiencePermissionSet permissions;
  UmiApplicationExperienceContext context;
  UmiApplicationExperienceInvocation invocation;
  UmiApplicationExperienceCommandResult result;
  int calls = 0;
  umi_application_experience_command_registry_init(&registry);
  umi_application_experience_dispatcher_init(&dispatcher);
  umi_application_experience_permission_set_init(&permissions);
  umi_application_experience_context_default(&context);
  assert(umi_application_experience_command_registry_register(&registry, &command) ==
         UMI_STATUS_OK);
  assert(umi_application_experience_dispatcher_bind(&dispatcher, command.command_id, handle_refresh,
                                                    &calls) == UMI_STATUS_OK);
  assert(umi_application_experience_invocation_init(&invocation, command.command_id,
                                                    "request.refresh.1",
                                                    "umicom.component.view") == UMI_STATUS_OK);
  assert(umi_application_experience_dispatcher_dispatch(&dispatcher, &registry, &permissions,
                                                        &context, &invocation,
                                                        &result) == UMI_STATUS_OK);
  assert(calls == 1);
  assert(result.completed_sequence == 1U);
  return 0;
}
