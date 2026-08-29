/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/state_transition.h
 *
 * PURPOSE:
 *   Validate UI-state changes and preserve their reason so tests and frontend
 *   adapters can explain how a component reached its current condition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STATE_TRANSITION_H
#define UMICOM_APPLICATION_EXPERIENCE_STATE_TRANSITION_H

#include "umicom/application/experience/ui_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceStateTransition {
  UmiApplicationExperienceUiStateKind from_kind;
  UmiApplicationExperienceUiStateKind to_kind;
  char reason[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  uint64_t sequence;
} UmiApplicationExperienceStateTransition;

int umi_application_experience_state_transition_allowed(
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind);
UmiStatus umi_application_experience_state_transition_create(
    UmiApplicationExperienceStateTransition *transition,
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind,
    const char *reason, uint64_t sequence);

#ifdef __cplusplus
}
#endif

#endif
