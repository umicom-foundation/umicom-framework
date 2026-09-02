/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/ui_state.h
 *
 * PURPOSE:
 *   Represent ready, empty, loading, busy, success, warning, error, offline and
 *   permission states with portable text and progress information.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_UI_STATE_H
#define UMICOM_APPLICATION_EXPERIENCE_UI_STATE_H

#include "umicom/application/experience/form_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience ui state data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceUiState {
  UmiApplicationExperienceUiStateKind kind;
  char title[UMI_APPLICATION_EXPERIENCE_TITLE_CAPACITY];
  char message[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  char action_command_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint32_t progress_percent;
  uint64_t revision;
  int blocking;
  int announce;
} UmiApplicationExperienceUiState;

/**
 * Initialise application experience ui state from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_experience_ui_state_init(UmiApplicationExperienceUiState *state,
                                                   UmiApplicationExperienceUiStateKind kind,
                                                   const char *title, const char *message,
                                                   const char *action_command_id);
/**
 * Provide the application experience ui state progress operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_ui_state_progress(UmiApplicationExperienceUiState *state,
                                                       uint32_t progress_percent);
/**
 * Check that application experience ui state satisfies its contract before another service
 * relies on it.
 */
UmiStatus
umi_application_experience_ui_state_validate(const UmiApplicationExperienceUiState *state);
/**
 * Provide the application experience ui state interactive operation used by this module
 * and its client applications.
 */
int umi_application_experience_ui_state_interactive(const UmiApplicationExperienceUiState *state);

#ifdef __cplusplus
}
#endif

#endif
