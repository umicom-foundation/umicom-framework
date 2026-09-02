/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_standard_states.c
 *
 * PURPOSE:
 *   Verify empty, loading, offline, permission and error state constructors
 *   produce valid messages and recovery commands.
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
  UmiApplicationExperienceUiState state;
  assert(umi_application_experience_state_empty(&state, "projects",
                                                "umicom.command.project.create") == UMI_STATUS_OK);
  assert(state.kind == UMI_APPLICATION_EXPERIENCE_UI_EMPTY);
  assert(umi_application_experience_state_offline(&state, "umicom.command.service.reconnect") ==
         UMI_STATUS_OK);
  assert(!umi_application_experience_ui_state_interactive(&state));
  assert(umi_application_experience_state_permission_required(
             &state, "umicom.command.access.request") == UMI_STATUS_OK);
  assert(state.announce);
  assert(umi_application_experience_state_error(&state, "The project could not be opened.",
                                                "umicom.command.project.retry") == UMI_STATUS_OK);
  return 0;
}
