/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/state_store.c
 *
 * PURPOSE:
 *   Apply valid state transitions atomically and retain recent reasons without
 *   dynamic memory or dependencies on a frontend event loop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/state_store.h"

#include <string.h>

UmiStatus
umi_application_experience_state_store_init(UmiApplicationExperienceStateStore *store,
                                            const UmiApplicationExperienceUiState *initial_state) {
  UmiStatus status = umi_application_experience_ui_state_validate(initial_state);
  if (status != UMI_STATUS_OK || store == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(store, 0, sizeof(*store));
  store->current = *initial_state;
  store->next_sequence = 1U;
  return UMI_STATUS_OK;
}

UmiStatus
umi_application_experience_state_store_transition(UmiApplicationExperienceStateStore *store,
                                                  const UmiApplicationExperienceUiState *next_state,
                                                  const char *reason) {
  UmiApplicationExperienceStateTransition transition;
  UmiStatus status;
  if (store == NULL || next_state == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_experience_ui_state_validate(next_state);
  if (status == UMI_STATUS_OK)
    status = umi_application_experience_state_transition_create(
        &transition, store->current.kind, next_state->kind, reason, store->next_sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (store->history_count >= UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY) {
    (void)memmove(&store->history[0], &store->history[1],
                  (UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY - 1U) * sizeof(store->history[0]));
    store->history_count -= 1U;
  }
  store->history[store->history_count++] = transition;
  store->current = *next_state;
  store->current.revision += 1U;
  store->next_sequence += 1U;
  return UMI_STATUS_OK;
}

const UmiApplicationExperienceStateTransition *
umi_application_experience_state_store_history_at(const UmiApplicationExperienceStateStore *store,
                                                  size_t index) {
  return store != NULL && store->history_count <= UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY &&
                 index < store->history_count
             ? &store->history[index]
             : NULL;
}
