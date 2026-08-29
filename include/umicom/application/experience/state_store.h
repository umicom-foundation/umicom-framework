/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/state_store.h
 *
 * PURPOSE:
 *   Keep the current component UI state and a bounded transition history for
 *   diagnostics, recovery, tests and frontend refreshes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STATE_STORE_H
#define UMICOM_APPLICATION_EXPERIENCE_STATE_STORE_H

#include "umicom/application/experience/state_transition.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceStateStore {
  UmiApplicationExperienceUiState current;
  UmiApplicationExperienceStateTransition history[UMI_APPLICATION_EXPERIENCE_HISTORY_CAPACITY];
  size_t history_count;
  uint64_t next_sequence;
} UmiApplicationExperienceStateStore;

UmiStatus
umi_application_experience_state_store_init(UmiApplicationExperienceStateStore *store,
                                            const UmiApplicationExperienceUiState *initial_state);
UmiStatus
umi_application_experience_state_store_transition(UmiApplicationExperienceStateStore *store,
                                                  const UmiApplicationExperienceUiState *next_state,
                                                  const char *reason);
const UmiApplicationExperienceStateTransition *
umi_application_experience_state_store_history_at(const UmiApplicationExperienceStateStore *store,
                                                  size_t index);

#ifdef __cplusplus
}
#endif

#endif
