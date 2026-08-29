/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_state_store.c
 *
 * PURPOSE:
 *   Verify standard UI states transition with ordered, inspectable reasons and
 *   preserve the current state independently from a frontend.
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

int main(void) {
  UmiApplicationExperienceUiState loading;
  UmiApplicationExperienceUiState ready;
  UmiApplicationExperienceStateStore store;
  assert(umi_application_experience_state_loading(&loading, "project") == UMI_STATUS_OK);
  assert(umi_application_experience_state_ready(&ready, "project") == UMI_STATUS_OK);
  assert(umi_application_experience_state_store_init(&store, &loading) == UMI_STATUS_OK);
  assert(umi_application_experience_state_store_transition(
             &store, &ready, "Project loading completed.") == UMI_STATUS_OK);
  assert(store.current.kind == UMI_APPLICATION_EXPERIENCE_UI_READY);
  assert(store.history_count == 1U);
  assert(umi_application_experience_state_store_history_at(&store, 0U) != NULL);
  return 0;
}
