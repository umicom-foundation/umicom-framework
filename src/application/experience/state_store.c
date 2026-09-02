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

/*
 * Initialise application experience state store from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus
umi_application_experience_state_store_init(UmiApplicationExperienceStateStore *store,
                                            const UmiApplicationExperienceUiState *initial_state) {
  UmiStatus status = umi_application_experience_ui_state_validate(initial_state);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status != UMI_STATUS_OK || store == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(store, 0, sizeof(*store));
  store->current = *initial_state;
  store->next_sequence = 1U;
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience state store transition operation used by this module
 * and its client applications.
 */
UmiStatus
umi_application_experience_state_store_transition(UmiApplicationExperienceStateStore *store,
                                                  const UmiApplicationExperienceUiState *next_state,
                                                  const char *reason) {
  UmiApplicationExperienceStateTransition transition;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (store == NULL || next_state == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_experience_ui_state_validate(next_state);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_application_experience_state_transition_create(
        &transition, store->current.kind, next_state->kind, reason, store->next_sequence);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find application experience state store history while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiApplicationExperienceStateTransition *
umi_application_experience_state_store_history_at(const UmiApplicationExperienceStateStore *store,
                                                  size_t index) {
  return store != NULL && store->history_count <= UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY &&
                 index < store->history_count
             ? &store->history[index]
             : NULL;
}
