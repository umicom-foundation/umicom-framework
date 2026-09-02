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

/**
 * Represent the application experience state transition data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceStateTransition {
  UmiApplicationExperienceUiStateKind from_kind;
  UmiApplicationExperienceUiStateKind to_kind;
  char reason[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  uint64_t sequence;
} UmiApplicationExperienceStateTransition;

/**
 * Provide the application experience state transition allowed operation used by this
 * module and its client applications.
 */
int umi_application_experience_state_transition_allowed(
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind);
/**
 * Initialise application experience state transition from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_experience_state_transition_create(
    UmiApplicationExperienceStateTransition *transition,
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind,
    const char *reason, uint64_t sequence);

#ifdef __cplusplus
}
#endif

#endif
